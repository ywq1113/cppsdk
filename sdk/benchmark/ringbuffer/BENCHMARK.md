# Benchmark Results

```
Run on (24 X 4691.1 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x12)
  L1 Instruction 32 KiB (x12)
  L2 Unified 1024 KiB (x12)
  L3 Unified 32768 KiB (x1)
Load Average: 0.27, 0.31, 0.33
***WARNING*** Library was built as DEBUG. Timings may be affected.
-------------------------------------------------------------------------------------------------
Benchmark                                       Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------
BM_SPSC_Throughput<MyQ>/1024/256/0      100000377 ns    102287583 ns            7 drop_rate=0 ops/s=14.0465M/s
BM_SPSC_Throughput<MyQ>/8192/256/0      100000387 ns    104084810 ns            7 drop_rate=0 ops/s=14.4297M/s
BM_SPSC_Throughput<MyQ>/65536/256/0     100000354 ns    104082408 ns            7 drop_rate=0 ops/s=19.3126M/s
BM_SPSC_Throughput<MyQ>/8192/256/1      100000497 ns    104083200 ns            7 drop_rate=0 ops/s=12.4356M/s p50_ns=64 p95_ns=64 p99_ns=8.192k
BM_SPSC_Throughput<BoostQ>/1024/256/0   100000472 ns    100000578 ns            7 drop_rate=131.257n ops/s=21.7674M/s
BM_SPSC_Throughput<BoostQ>/8192/256/0   100000423 ns    100000501 ns            7 drop_rate=0 ops/s=13.6936M/s
BM_SPSC_Throughput<BoostQ>/65536/256/0  100000356 ns     99996743 ns            7 drop_rate=0 ops/s=6.9532M/s
BM_SPSC_Throughput<BoostQ>/8192/256/1   100000442 ns    100000480 ns            7 drop_rate=0 ops/s=10.4982M/s p50_ns=64 p95_ns=4.096k p99_ns=4.096k
```

**Throughput**

Small capacity: BoostQ is faster (~1.5×).

Large capacity: MyQ is more stable and even outperforms BoostQ, which suggests it suffers less performance degradation when crossing cache lines.

**Latency stability**

Median latency (p50): Identical for both.

High-percentile latency (p95/p99): MyQ is more stable with lower variance compared to BoostQ.

**Analysis**

p50: MyQ vs BoostQ (64ns vs 64ns)，maybe less race condition or cache hit

p95/p99: MyQ vs BoostQ（64ns/8.192μs vs 4.096μs/4.096μs）means that BoostQ demonstrates more stable performance in the tail latencies (p95/p99), indicating reduced jitter compared to MyQ.