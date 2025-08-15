# Benchmark Results

## GCC

### No SIMD
```
Run on (24 X 4691.08 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 1024 KiB (x12)
  L3 Unified 32768 KiB (x1)
Load Average: 0.23, 0.10, 0.08
***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_Accumulate    16312296 ns     15849065 ns           44
BM_SegmentTree    1642744 ns      1596081 ns          439
```

### SIMD

```
Run on (24 X 4691.08 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 1024 KiB (x12)
  L3 Unified 32768 KiB (x1)
Load Average: 0.01, 0.06, 0.07
***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_Accumulate    16112041 ns     15666956 ns           45
BM_SegmentTree    1625792 ns      1580890 ns          439
```

## Clang

### No SIMD
```
Run on (24 X 4691.1 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 1024 KiB (x12)
  L3 Unified 32768 KiB (x1)
Load Average: 0.31, 0.43, 0.20
***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_Accumulate    64678818 ns     64677593 ns           11
BM_SegmentTree    1377342 ns      1377306 ns          509
```

### SIMD

```
Run on (24 X 4691.1 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 1024 KiB (x12)
  L3 Unified 32768 KiB (x1)
Load Average: 0.12, 0.28, 0.17
***WARNING*** ASLR is enabled, the results may have unreproducible noise in them.
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
BM_Accumulate     9774769 ns      9774806 ns           72
BM_SegmentTree    1367841 ns      1367845 ns          512
```
