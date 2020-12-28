# oh my cpu
## A cpu benchmark

This is a cpu tool for testing the floating-points peak performance. Now it supports linux and x86-64 platform. It can automatically recognize the x86 instruction sets and select the proper set to do test.

To compile with source code, just run this:
```bash
# build:
make
```

To compile with all functions use this (danger):
```bash
# Danger, Possible "illegal hardware instruction" error
make compile-all
```

And to run, this:
```bash
# run:
./oh_my_cpu 4 # threads number
```


---

> Example on Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz (4 cores):

```bash
$ ./oh_my_cpu 1
This system has 4 processors configured and 4 processors available.
Using 1 cores
Thread(s): 1

avx fp32 perf: 47.5982 gflops.
avx fp64 perf: 24.4189 gflops.
sse fp32 perf: 24.4304 gflops.
sse fp64 perf: 12.0871 gflops.
```

```bash
# If you do not specify the number of cores, he will use the maximum available
$ ./oh_my_cpu
This system has 4 processors configured and 4 processors available.
Using 4 cores
Thread(s): 4

avx fp32 perf: 91.6911 gflops.
avx fp64 perf: 44.7396 gflops.
sse fp32 perf: 43.3244 gflops.
sse fp64 perf: 21.5457 gflops.
```
