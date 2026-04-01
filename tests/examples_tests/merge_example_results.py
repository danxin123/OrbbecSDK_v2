from __future__ import annotations

import argparse

from pathlib import Path

from examples_test_utils import dump_json, load_json, render_html_report, summarize_results, write_junit_report


def main() -> int:
    parser = argparse.ArgumentParser(description="Merge examples suite results into aggregate reports")
    parser.add_argument("--input-dir", required=True)
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--title", default="Examples Aggregate Report")
    args = parser.parse_args()

    input_dir = Path(args.input_dir).resolve()
    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    merged_results = []
    for result_file in sorted(input_dir.rglob("results.json")):
        payload = load_json(result_file, {})
        merged_results.extend(payload.get("results", []))

    dump_json(output_dir / "results.json", {
        "stats": summarize_results(merged_results),
        "results": merged_results,
    })
    write_junit_report(merged_results, output_dir / "junit.xml", "examples-aggregate")
    render_html_report(merged_results, output_dir / "report.html", args.title)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())