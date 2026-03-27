# Coding Standards Rollout Plan

This document describes how to enforce the coding standards defined in `docs/coding-standards.md`.

---

## Phase 1: Pre-commit Hook (Local Enforcement)

**Goal**: Catch formatting issues before they reach CI.

### 1.1 Install pre-commit framework

```bash
pip install pre-commit
```

### 1.2 Create `.pre-commit-config.yaml`

```yaml
repos:
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v18.1.8   # match your team's clang-format version
    hooks:
      - id: clang-format
        types_or: [c, c++]
        # Only check project source, not 3rdparty
        files: ^(src|include|tests|examples|tools)/

  - repo: https://github.com/cheshirekow/cmake-format-precommit
    rev: v0.6.13
    hooks:
      - id: cmake-format
        additional_dependencies: [pyyaml]
```

### 1.3 Activate

```bash
# Install hooks into .git/hooks/
pre-commit install

# Run on all files once (baseline)
pre-commit run --all-files
```

**Team onboarding**: Add `pre-commit install` to the developer setup instructions (README or `scripts/env_setup/`).

---

## Phase 2: CI Format Check (Gate Enforcement)

**Goal**: No unformatted code merges into main.

### 2.1 Add format-check job to `pr-gate.yml`

```yaml
  format-check:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v4

      - name: Install clang-format
        run: sudo apt-get install -y clang-format-18

      - name: Check C++ formatting
        run: |
          find src include tests examples tools \
            \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) \
            -not -path '*/3rdparty/*' \
            | xargs clang-format-18 --dry-run --Werror

      - name: Install cmake-format
        run: pip install cmake-format pyyaml

      - name: Check CMake formatting
        run: |
          find . -name 'CMakeLists.txt' -o -name '*.cmake' \
            | grep -v 3rdparty \
            | xargs cmake-format --check
```

### 2.2 Add clang-tidy job (incremental)

Only check changed files to avoid blocking the entire repo on legacy code:

```yaml
  clang-tidy-diff:
    runs-on: ubuntu-22.04
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Configure
        run: cmake -S . -B build -DOB_ENABLE_CLANG_TIDY=OFF -DOB_BUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

      - name: Build
        run: cmake --build build --parallel 4

      - name: Run clang-tidy on changed files
        run: |
          git diff --name-only --diff-filter=ACMR origin/main... -- '*.cpp' '*.hpp' '*.h' \
            | grep -E '^(src|include)/' \
            | xargs -r clang-tidy -p build --warnings-as-errors='*'
```

---

## Phase 3: Incremental Cleanup of Existing Code

**Goal**: Bring existing code into compliance without a single giant PR.

### 3.1 Format pass (low risk)

clang-format is a safe, mechanical transformation. It can be done in one PR per module:

| PR | Scope | Files |
|----|-------|-------|
| 1 | `src/shared/` | ~30 files |
| 2 | `src/core/` | ~20 files |
| 3 | `src/filter/` | ~15 files |
| 4 | `src/platform/` | ~50 files |
| 5 | `src/device/` | ~80 files |
| 6 | `src/pipeline/` + `src/media/` | ~20 files |
| 7 | `src/impl/` + `src/context/` | ~20 files |
| 8 | `include/` | ~25 files |
| 9 | `tests/` + `examples/` | ~40 files |

Each PR:
```bash
find src/shared -name '*.cpp' -o -name '*.hpp' -o -name '*.h' | xargs clang-format -i
git add -A && git commit -m "style: format src/shared with clang-format"
```

**Tip**: After the format PR merges, run `git blame --ignore-rev <commit-hash>` to keep blame useful. Collect format commit hashes in a `.git-blame-ignore-revs` file:

```bash
echo "# clang-format pass" >> .git-blame-ignore-revs
echo "<commit-hash>" >> .git-blame-ignore-revs
git config blame.ignoreRevsFile .git-blame-ignore-revs
```

### 3.2 clang-tidy fixes (higher risk, staged)

clang-tidy changes semantics (e.g., `modernize-use-override` adds `override`). These need code review.

**Priority order** (by safety):
1. `modernize-use-override` — add missing `override` keywords
2. `modernize-use-nullptr` — replace `0`/`NULL` with `nullptr`
3. `modernize-use-equals-default` — default constructors/destructors
4. `readability-redundant-string-cstr` — remove unnecessary `.c_str()`
5. `performance-unnecessary-copy-initialization` — avoid copies
6. Other checks — one check per PR

```bash
# Fix one check at a time
clang-tidy -p build --fix --checks='-*,modernize-use-override' src/core/**/*.cpp
```

---

## Phase 4: IDE Integration

### 4.1 VS Code

Add `.vscode/settings.json` to the repo (or document in README):

```json
{
    "editor.formatOnSave": true,
    "C_Cpp.clang_format_style": "file",
    "C_Cpp.codeAnalysis.clangTidy.enabled": true,
    "C_Cpp.codeAnalysis.clangTidy.config": "",
    "files.trimTrailingWhitespace": true,
    "files.insertFinalNewline": true
}
```

### 4.2 CLion

CLion reads `.clang-format` and `.clang-tidy` automatically. Enable:
- Settings > Editor > Code Style > C/C++ > "Enable ClangFormat"
- Settings > Editor > Inspections > C/C++ > Clang-Tidy checks

### 4.3 EditorConfig

Create `.editorconfig` at repo root for editors that don't read clang-format:

```ini
root = true

[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
trim_trailing_whitespace = true

[*.{cpp,hpp,h,c}]
indent_style = space
indent_size = 4

[CMakeLists.txt]
indent_style = space
indent_size = 4

[*.{yml,yaml}]
indent_style = space
indent_size = 2
```

---

## Phase 5: Team Adoption

### 5.1 Checklist for code reviewers

- [ ] `clang-format` clean (CI will catch this, but review for intent)
- [ ] No new clang-tidy warnings introduced
- [ ] Naming follows conventions (PascalCase classes, camelCase methods, `_` suffix members)
- [ ] New public API has Doxygen comments with `@brief`, `@param`, `@return`
- [ ] New tests follow `test_TC_<MODULE>_<CASE_ID>_<DESCRIPTION>` naming
- [ ] No C-style casts in new C++ code
- [ ] Exceptions use the correct type from the hierarchy

### 5.2 Documentation updates

- Add a link to `docs/coding-standards.md` from `README.md` and `CONTRIBUTING.md`.
- Add `pre-commit install` to the developer setup script (`scripts/env_setup/`).

---

## Summary of Deliverables

| Item | File / Location | Status |
|------|-----------------|--------|
| Coding standards document | `docs/coding-standards.md` | Created |
| clang-format config | `.clang-format` | Exists |
| clang-tidy config | `.clang-tidy` | Exists |
| cmake-format config | `.cmake-format.py` | Exists |
| pre-commit config | `.pre-commit-config.yaml` | To create |
| CI format-check job | `.github/workflows/pr-gate.yml` | To add |
| CI clang-tidy-diff job | `.github/workflows/pr-gate.yml` | To add |
| EditorConfig | `.editorconfig` | To create |
| git-blame-ignore-revs | `.git-blame-ignore-revs` | To create (after format PRs) |
| Reviewer checklist | `docs/coding-standards-rollout.md` | Created |
