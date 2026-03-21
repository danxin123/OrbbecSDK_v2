#!/usr/bin/env python3
"""
Release Candidate policy gate.

Policy defaults:
- overall success rate >= 98%
- P0 pass rate == 100% (optional if no P0 data and allow_missing_p0=true)
- benchmark has no 3-sigma regression (optional if baseline/current not present)
"""

import argparse
import csv
import json
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Dict, List, Tuple


def find_xml_files(root: Path) -> List[Path]:
    return sorted(root.rglob("*.xml")) if root.exists() else []


def collect_junit_stats(reports_dir: Path) -> Tuple[int, int, int, int, int, int]:
    total = failed = errors = skipped = 0
    p0_total = p0_failed = 0

    for xml_file in find_xml_files(reports_dir):
        try:
            root = ET.parse(xml_file).getroot()
        except Exception:
            continue

        for suite in root.findall(".//testsuite"):
            total += int(suite.attrib.get("tests", 0) or 0)
            failed += int(suite.attrib.get("failures", 0) or 0)
            errors += int(suite.attrib.get("errors", 0) or 0)
            skipped += int(suite.attrib.get("skipped", 0) or 0)

        for case in root.findall(".//testcase"):
            combined = (case.attrib.get("name", "") + " " + case.attrib.get("classname", "")).lower()
            if "p0" not in combined:
                continue
            p0_total += 1
            if (
                case.find("failure") is not None
                or case.find("error") is not None
                or case.find("skipped") is not None
            ):
                p0_failed += 1

    return total, failed, errors, skipped, p0_total, p0_failed


def metric_direction(metric: str) -> str:
    return "higher_is_better" if metric in {"depth_fps", "color_fps", "ir_fps"} else "lower_is_better"


def load_baseline(path: Path) -> Dict[str, Tuple[float, float, int]]:
    rows: Dict[str, Tuple[float, float, int]] = {}
    if not path.exists():
        return rows
    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            metric = row.get("metric", "")
            if not metric:
                continue
            rows[metric] = (
                float(row.get("mean", 0) or 0),
                float(row.get("stddev", 0) or 0),
                int(row.get("sample_count", 0) or 0),
            )
    return rows


def load_current(path: Path) -> Dict[str, float]:
    rows: Dict[str, float] = {}
    if not path.exists():
        return rows
    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            metric = row.get("metric", "")
            if not metric:
                continue
            value = row.get("value", row.get("mean", 0))
            rows[metric] = float(value or 0)
    return rows


def benchmark_regressions(baseline_csv: Path, current_csv: Path, sigma: float) -> List[str]:
    baseline = load_baseline(baseline_csv)
    current = load_current(current_csv)
    out: List[str] = []

    for metric, (mean, stddev, sample_count) in baseline.items():
        if metric not in current:
            continue
        value = current[metric]
        direction = metric_direction(metric)

        if sample_count < 2 or stddev == 0:
            if mean == 0:
                continue
            delta = abs(value - mean) / mean
            regression = (direction == "higher_is_better" and value < mean and delta > 0.10) or (
                direction == "lower_is_better" and value > mean and delta > 0.10
            )
            if regression:
                out.append(f"{metric}: current={value:.2f}, baseline={mean:.2f}, fallback>10%")
            continue

        threshold = mean - sigma * stddev if direction == "higher_is_better" else mean + sigma * stddev
        regression = value < threshold if direction == "higher_is_better" else value > threshold
        if regression:
            out.append(
                f"{metric}: current={value:.2f}, baseline={mean:.2f}±{stddev:.2f}, threshold={threshold:.2f}"
            )

    return out


def main() -> int:
    parser = argparse.ArgumentParser(description="RC policy gate")
    parser.add_argument("--reports-dir", default="all_reports")
    parser.add_argument("--min-success-rate", type=float, default=98.0)
    parser.add_argument("--allow-missing-p0", action="store_true")
    parser.add_argument("--benchmark-baseline", default="")
    parser.add_argument("--benchmark-current", default="")
    parser.add_argument("--sigma", type=float, default=3.0)
    parser.add_argument("--job-results", default="")
    parser.add_argument("--require-hw-p0", action="store_true")
    parser.add_argument("--output-json", default="reports/rc_gate_result.json")
    parser.add_argument("--output-md", default="reports/rc_gate_result.md")
    args = parser.parse_args()

    total, failed, errors, skipped, p0_total, p0_failed = collect_junit_stats(Path(args.reports_dir))

    # Fallback: evaluate required job outcomes when test-level XML is unavailable.
    if total == 0 and args.job_results:
        parts = [x.strip().lower() for x in args.job_results.split(",") if x.strip()]
        considered = [x for x in parts if x not in {"skipped", "cancelled", "neutral"}]
        if considered:
            total = len(considered)
            passed_jobs = sum(1 for x in parts if x == "success")
            failed = max(total - passed_jobs, 0)
            errors = 0
            skipped = 0

    passed = max(total - failed - errors - skipped, 0)
    success_rate = (passed / total * 100.0) if total > 0 else 0.0
    p0_rate = ((p0_total - p0_failed) / p0_total * 100.0) if p0_total else 0.0

    reasons: List[str] = []

    if total == 0:
        reasons.append("No JUnit XML data found in reports")

    if success_rate < args.min_success_rate:
        reasons.append(f"Overall success rate {success_rate:.2f}% < {args.min_success_rate:.2f}%")

    if p0_total == 0 and not args.allow_missing_p0:
        reasons.append("No P0 testcase found in JUnit XML")
    elif p0_total > 0 and p0_rate < 100.0:
        reasons.append(f"P0 pass rate {p0_rate:.2f}% < 100%")

    if args.require_hw_p0:
        hw_p0_cases = 0
        for xml_file in find_xml_files(Path(args.reports_dir)):
            try:
                root = ET.parse(xml_file).getroot()
            except Exception:
                continue
            for case in root.findall(".//testcase"):
                combined = (case.attrib.get("name", "") + " " + case.attrib.get("classname", "")).lower()
                if "hw" in combined and "p0" in combined:
                    hw_p0_cases += 1
        if hw_p0_cases == 0:
            reasons.append("Hardware P0 required but no hw+p0 testcase found in JUnit XML")

    regressions: List[str] = []
    baseline = Path(args.benchmark_baseline) if args.benchmark_baseline else None
    current = Path(args.benchmark_current) if args.benchmark_current else None
    if baseline and current and baseline.exists() and current.exists():
        regressions = benchmark_regressions(baseline, current, args.sigma)
        if regressions:
            reasons.append(f"Benchmark regression detected ({len(regressions)} metrics)")

    result = {
        "total_tests": total,
        "passed": passed,
        "failed": failed,
        "errors": errors,
        "skipped": skipped,
        "success_rate": round(success_rate, 2),
        "min_success_rate": args.min_success_rate,
        "p0_total": p0_total,
        "p0_pass_rate": round(p0_rate, 2),
        "benchmark_regressions": regressions,
        "passed_gate": len(reasons) == 0,
        "fail_reasons": reasons,
    }

    out_json = Path(args.output_json)
    out_json.parent.mkdir(parents=True, exist_ok=True)
    out_json.write_text(json.dumps(result, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    lines = [
        "# RC Policy Gate Result",
        "",
        f"- Required success rate: {args.min_success_rate:.2f}%",
        f"- Current success rate: {success_rate:.2f}%",
        f"- Total tests: {total}, Passed: {passed}, Failed: {failed}, Errors: {errors}, Skipped: {skipped}",
        f"- P0 tests: {p0_total}, P0 pass rate: {p0_rate:.2f}%",
        "",
    ]
    if regressions:
        lines.append("## Benchmark Regressions")
        lines.extend([f"- {x}" for x in regressions])
        lines.append("")

    if reasons:
        lines.append("## Gate Decision: FAIL")
        lines.extend([f"- {x}" for x in reasons])
    else:
        lines.append("## Gate Decision: PASS")

    Path(args.output_md).write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps(result, ensure_ascii=False))

    return 0 if result["passed_gate"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
