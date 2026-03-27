# Contributing Guide

## Coding Standards

All code must follow the project coding standards documented in
[docs/coding-standards.md](docs/coding-standards.md). Key points:

- Run `clang-format` before committing (or use `pre-commit install` to automate it).
- Follow the naming conventions: PascalCase classes, camelCase methods, `_` suffix members.
- C++11 only — no C++14/17/20 features.
- Use the SDK exception hierarchy for error handling.

## Testing Requirements For API Changes

Any pull request that adds, removes, or changes API behavior must include related
automated tests (C++ or Python wrapper side).

If a test cannot be added in the same pull request, the PR description must include:

- Why test coverage cannot be added now
- Risk assessment for not adding coverage
- Follow-up issue or plan to close the gap

## Test Naming Convention

Use test names aligned with testcase IDs from the project test plan:

- test_TC_<MODULE>_<CASE_ID>_<DESCRIPTION>

Examples:

- test_TC_CPP_010_001_frame_basic_integrity
- test_TC_PYTHON_003_002_context_create_destroy

## Shared Setup And Reuse

Prefer shared helpers/fixtures instead of per-test duplicated setup and teardown.

- Reuse existing fixture/helper patterns in tests/
- Keep device lifecycle and cleanup in shared utilities when possible
- Keep no-hardware and hardware paths explicitly separated

## Test Registration In CI

When adding new automated tests, ensure they are reachable by CI:

- C++ tests: wire target into tests/CMakeLists or existing test targets used by workflows
- Python tests: make sure CI job discovers/runs them in workflow test commands
- If introducing a new test suite, add/adjust workflow invocation accordingly

## Templates

### Template A: No-Hardware Test

```cpp
int test_TC_CPP_XXX_001_nohw_example() {
	auto ctx = std::make_shared<ob::Context>();
	if(ctx == nullptr) {
		return 1;
	}
	auto list = ctx->queryDeviceList();
	if(list == nullptr) {
		return 1;
	}
	return 0;
}
```

### Template B: Hardware-Dependent Test

```cpp
// Pseudocode pattern: explicitly gate hardware-dependent path.
int test_TC_CPP_XXX_002_hw_example() {
	auto pipeline = std::make_shared<ob::Pipeline>();
	auto config   = std::make_shared<ob::Config>();
	config->enableStream(OB_STREAM_DEPTH);
	pipeline->start(config);
	auto fs = pipeline->waitForFrames(3000);
	pipeline->stop();
	return fs ? 0 : 1;
}
```

## Code Review Checklist

- [ ] Code is `clang-format` clean (CI will verify this)
- [ ] No new clang-tidy warnings introduced
- [ ] Naming follows conventions (see [coding standards](docs/coding-standards.md))
- [ ] API change has corresponding automated tests, or waiver is documented
- [ ] Test names are aligned with TC naming convention
- [ ] No-hardware validation path is covered where feasible
- [ ] New tests are wired into CI execution path
- [ ] Risk and rollback impact are described for behavior changes
- [ ] New public API has Doxygen comments (`@brief`, `@param`, `@return`)
