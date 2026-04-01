from __future__ import annotations

from pathlib import Path

from examples_test_utils import discover_cases, dump_json, manifest_argument_parser


def main() -> int:
    parser = manifest_argument_parser("Generate examples test manifest")
    args = parser.parse_args()

    workspace_root = Path(args.workspace_root).resolve()
    output_path = Path(args.output).resolve()
    overrides_path = Path(args.overrides).resolve() if args.overrides else None

    manifest = discover_cases(workspace_root, overrides_path)
    dump_json(output_path, manifest)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())