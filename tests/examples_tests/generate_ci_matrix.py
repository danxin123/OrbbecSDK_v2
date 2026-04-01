from __future__ import annotations

import argparse

from pathlib import Path

from examples_test_utils import dump_json, generate_matrix, load_json


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate CI matrix for examples tests")
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--runner-pools", default="scripts/runner/runner_pools.json")
    parser.add_argument("--output-matrix", required=True)
    parser.add_argument("--output-unassigned", required=True)
    parser.add_argument("--batch-size", type=int, default=6)
    args = parser.parse_args()

    manifest = load_json(Path(args.manifest), {})
    runner_pools = load_json(Path(args.runner_pools), {})
    matrix_payload = generate_matrix(manifest, runner_pools, args.batch_size)

    dump_json(Path(args.output_matrix), matrix_payload["matrix"])
    dump_json(Path(args.output_unassigned), matrix_payload["unassigned"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())