# Contributing Guide

## Pull Request Requirements

Every pull request that adds or changes API behavior must include related tests.
If a test cannot be added, explain the reason in the pull request description.

## Test Naming

Use the format below for test function names:

- `test_TC_<MODULE>_<ID>_<DESCRIPTION>`

## Shared Fixtures / Helpers

Prefer shared helpers and common setup patterns from existing tests.
Avoid duplicating device setup and cleanup logic in each test case.

## Test Registration

When adding new automated test cases, ensure they are included in CI selection.

## Review Checklist

- [ ] API change has corresponding automated test
- [ ] No-hardware test path is considered where possible
- [ ] Test names are aligned with test case IDs
- [ ] CI workflow impact is documented in PR description
