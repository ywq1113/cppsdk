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

## Benchmark

**How to run benchmark by target.**

```bash
bazel run -c opt //sdk/benchmark/segment_tree:segment_tree_benchmark_no_simd
bazel run -c opt //sdk/benchmark/segment_tree:segment_tree_benchmark_simd
```

Note: If you run a Bazel target without "-c opt", Bazel will build a debug binary or library by default. Add "-c opt" to build with release (optimized) settings for accurate.
