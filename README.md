# cppsdk

## Build

**How to Compiler this project.**

```bash
bazel clean --expunge
bazel mod tidy
bazel build //sdk/...
```

## Test

**How to run unit tests by target.**

```bash
bazel test //sdk/tests:segment_tree_test

# If need more info
# bazel test --test_verbose_timeout_warnings --test_output=all //sdk/tests/...
```

**How to run all unit tests.**

```bash
bazel test //...

```