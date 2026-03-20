#!/usr/bin/env python3
import argparse
import json
from datetime import datetime, timedelta, timezone
from pathlib import Path


def load_json(path: Path, default):
    if not path.exists():
        return default
    return json.loads(path.read_text(encoding="utf-8"))


def save_json(path: Path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def normalize_report(report: dict, workflow: str):
    total = int(report.get("total", report.get("total_cases", 0)))
    passed = int(report.get("passed", report.get("passed_cases", 0)))
    failed = int(report.get("failed", report.get("failed_cases", 0)))

    if total <= 0:
        total = passed + failed

    pass_rate = (passed / total) if total > 0 else 0.0

    return {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "workflow": workflow,
        "total_cases": total,
        "passed_cases": passed,
        "failed_cases": failed,
        "pass_rate": pass_rate,
        "failed_case_names": report.get("failed_case_names", []),
    }


def trim_last_days(records, days=90):
    cutoff = datetime.now(timezone.utc) - timedelta(days=days)
    kept = []
    for r in records:
        ts = r.get("timestamp")
        if not ts:
            continue
        try:
            dt = datetime.fromisoformat(ts.replace("Z", "+00:00"))
        except ValueError:
            continue
        if dt >= cutoff:
            kept.append(r)
    return kept


def main():
    parser = argparse.ArgumentParser(description="Update dashboard data.json from CI report")
    parser.add_argument("--report", required=True, help="Path to normalized report json")
    parser.add_argument("--data", required=True, help="Path to dashboard data json")
    parser.add_argument("--workflow", default="unknown", help="Workflow name")
    args = parser.parse_args()

    report_path = Path(args.report)
    data_path = Path(args.data)

    report = load_json(report_path, {})
    records = load_json(data_path, [])

    records.append(normalize_report(report, args.workflow))
    records = trim_last_days(records, days=90)

    save_json(data_path, records)
    print(f"Updated {data_path} with {len(records)} records")


if __name__ == "__main__":
    main()
