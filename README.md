# cppsdk

## Bootstrap

```
sudo bash ./bootstrap.sh
```

## Developer

### Vscode

**settings.json**

```
{
    "C_Cpp.default.compileCommands": "${workspaceFolder}/compile_commands.json"
}
```
**compile_commands.json**

```
bazel run @wolfd_bazel_compile_commands//:generate_compile_commands

```

### Format

```
find . \( -name "*.h" -o -name "*.cpp" \) -print0 | xargs -0 clang-format -i
```

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
