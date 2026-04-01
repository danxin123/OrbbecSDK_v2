from __future__ import annotations

import argparse
import json

from pathlib import Path
from typing import Any


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def normalize(values: list[str]) -> set[str]:
    return {v.strip().lower() for v in values if v and v.strip()}


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate matrix for hw_full_test from global runner pools")
    parser.add_argument("--runner-pools", default="scripts/runner/runner_pools.json")
    parser.add_argument("--output-matrix", required=True)
    parser.add_argument("--platform", default="linux_x86_64")
    parser.add_argument("--required-capabilities", default="depth,color")
    parser.add_argument("--require-self-hosted", action="store_true", default=True)
    args = parser.parse_args()

    pools_payload = load_json(Path(args.runner_pools))
    required_caps = normalize([x for x in args.required_capabilities.split(",")])

    matrix = []
    for pool in pools_payload.get("pools", []):
        if pool.get("platform") != args.platform:
            continue

        runs_on = pool.get("runs_on", [])
        if args.require_self_hosted and "self-hosted" not in runs_on:
            continue

        caps = normalize(pool.get("capabilities", []))
        if not required_caps.issubset(caps):
            continue

        matrix.append(
            {
                "job_id": f"hw-full-{pool['name']}",
                "display_name": f"hw_full_test @ {pool['name']}",
                "pool_name": pool["name"],
                "platform": pool["platform"],
                "runs_on": json.dumps(runs_on),
                "device_models": ",".join(pool.get("device_models", [])),
            }
        )

    Path(args.output_matrix).parent.mkdir(parents=True, exist_ok=True)
    Path(args.output_matrix).write_text(json.dumps(matrix, ensure_ascii=False, indent=2), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
