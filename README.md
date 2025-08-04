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

```

**How to run all unit tests.**

```bash
bazel test //...

```