from __future__ import annotations

import argparse
import os
import subprocess
import time

from pathlib import Path

from examples_test_utils import (
    case_dir_name,
    default_bag_path,
    default_preset_bin,
    detect_host_platform,
    dump_json,
    load_json,
    render_html_report,
    resolve_binary,
    resolve_example_workdir,
    substitute_tokens,
    summarize_results,
    write_junit_report,
)


def split_case_ids(raw: str) -> list[str]:
    return [item.strip() for item in raw.split(";") if item.strip()]


def make_result(case: dict, suite_name: str, platform: str, pool_name: str) -> dict:
    return {
        "suite_name": suite_name,
        "case_id": case["id"],
        "target": case["target"],
        "platform": platform,
        "pool_name": pool_name,
        "status": "failed",
        "duration_sec": 0.0,
        "message": "not executed",
        "saved_images": [],
        "log_path": "",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Run examples suite and emit report artifacts")
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--build-root", required=True)
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--case-ids", required=True)
    parser.add_argument("--suite-name", default="examples-suite")
    parser.add_argument("--pool-name", default="local")
    parser.add_argument("--platform", default="")
    parser.add_argument("--allow-destructive", action="store_true")
    args = parser.parse_args()

    manifest = load_json(Path(args.manifest), {})
    workspace_root = Path(manifest["workspace_root"]).resolve()
    build_root = Path(args.build_root).resolve()
    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    platform = args.platform or detect_host_platform()
    default_bag = default_bag_path(workspace_root)
    default_preset = default_preset_bin(workspace_root)

    selected_case_ids = set(split_case_ids(args.case_ids))
    cases = [case for case in manifest.get("cases", []) if case["id"] in selected_case_ids]
    case_map = {case["id"]: case for case in cases}
    ordered_cases = [case_map[case_id] for case_id in split_case_ids(args.case_ids) if case_id in case_map]

    results: list[dict] = []
    for case in ordered_cases:
        result = make_result(case, args.suite_name, platform, args.pool_name)
        case_output_dir = output_dir / "cases" / case_dir_name(case["id"])
        case_output_dir.mkdir(parents=True, exist_ok=True)
        log_path = case_output_dir / "run.log"
        result["log_path"] = str(log_path)

        if case.get("allow_destructive") and not args.allow_destructive:
            result["status"] = "skipped"
            result["message"] = "destructive case is disabled"
            log_path.write_text(result["message"], encoding="utf-8")
            results.append(result)
            continue

        binary = resolve_binary(build_root, case["target"])
        if binary is None:
            result["status"] = "failed"
            result["message"] = "binary not found"
            log_path.write_text(result["message"], encoding="utf-8")
            results.append(result)
            continue

        work_dir = resolve_example_workdir(build_root, binary)
        env = os.environ.copy()
        env["OB_EXAMPLE_TEST_MODE"] = "1"
        env["OB_EXAMPLE_TEST_OUTPUT_DIR"] = str(case_output_dir)
        env["OB_EXAMPLE_AUTO_KEY_INITIAL_DELAY_MS"] = str(case.get("auto_key_initial_delay_ms", 1800))
        env["OB_EXAMPLE_AUTO_KEY_INTERVAL_MS"] = str(case.get("auto_key_interval_ms", 1200))
        env["OB_EXAMPLE_AUTO_KEYS"] = ",".join(case.get("auto_keys", ["ESC"]))

        token_context = {
            "CASE_OUTPUT_DIR": str(case_output_dir),
            "DEFAULT_BAG": default_bag,
            "DEFAULT_PRESET_BIN": default_preset,
            "WORKSPACE_ROOT": str(workspace_root),
        }
        stdin_lines = [substitute_tokens(item, token_context) for item in case.get("stdin_lines", [])]
        stdin_text = ""
        if stdin_lines:
            stdin_text = "\n".join(stdin_lines) + "\n"

        started_at = time.monotonic()
        try:
            run_kwargs = {
                "args": [str(binary)],
                "text": True,
                "cwd": str(work_dir),
                "env": env,
                "timeout": int(case.get("timeout_sec", 20)),
                "check": False,
            }
            if stdin_text:
                run_kwargs["input"] = stdin_text

            with log_path.open("w", encoding="utf-8") as log_handle:
                run_kwargs["stdout"] = log_handle
                run_kwargs["stderr"] = subprocess.STDOUT
                completed = subprocess.run(**run_kwargs)

            result["duration_sec"] = time.monotonic() - started_at

            saved_images = sorted(str(path) for path in case_output_dir.glob("*.png"))
            result["saved_images"] = saved_images

            if completed.returncode != 0:
                result["status"] = "failed"
                result["message"] = f"exit code {completed.returncode}"
            elif case.get("supports_png_save") and not saved_images:
                result["status"] = "failed"
                result["message"] = "expected PNG output was not generated"
            else:
                result["status"] = "passed"
                result["message"] = "ok"
        except subprocess.TimeoutExpired as exc:
            result["duration_sec"] = time.monotonic() - started_at
            result["status"] = "failed"
            result["message"] = f"timeout after {case.get('timeout_sec', 20)}s"

        results.append(result)

    payload = {
        "generated_at": manifest.get("generated_at"),
        "suite_name": args.suite_name,
        "pool_name": args.pool_name,
        "platform": platform,
        "stats": summarize_results(results),
        "results": results,
    }
    dump_json(output_dir / "results.json", payload)
    write_junit_report(results, output_dir / "junit.xml", args.suite_name)
    render_html_report(results, output_dir / "report.html", f"Examples Suite - {args.suite_name}")
    return 0 if payload["stats"]["failed"] == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())