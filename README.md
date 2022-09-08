# SPM project
**Project for the course _Parallel &amp; Distributed Systems: Paradigms &amp; Models_ @ University of Pisa**

<p align="center">
  <img width=150px src="https://www.plan4res.eu/wp-content/uploads/2018/02/University-of-Pisa-Italy.png"/>
</p>

**Project track:** video motion detect

## Compile
All these commands are intended to be executed in the base directory of the project (`SPM-project`).

**Compile everything:** `make all`

**Sequential implementation:** `make sequential` or `make all`

**Parallel implementation with native C++ threads:** `make threads` or `make all`

**Parallel implementation with FastFlow:** `make ff` or `make all`

**Script to measure latencies of sequential operations:** `make seq_funcs_perf_eval` or `make all`

## Execute
All these commands are intended to be executed in the base directory of the project (`SPM-project`).

The arguments of the executables are indicated in angle brackets (`<>`).
Optional arguments are indicated in squared and angle brackets (`[<>]`).

**Sequential implementation:**
```
./bin/main_sequential.out <path to video>
```

**Parallel implementation with OpenMP pragmas:**
```
./bin/main_sequential.out <path to video> [<workers for rgb2gray>] [<workers for smoothing>] [<workers for motion detection>]
```

**Parallel implementation with native C++ threads:**
```
./bin/main_threads.out <path to video> <number of workers>
```

**Parallel implementation with native C++ threads and OpenMP pragmas:**
```
./bin/main_threads.out <path to video> <number of workers> [<workers for rgb2gray>] [<workers for smoothing>] [<workers for motion detection>]
```

**Parallel implementation with FastFlow:**
```
./bin/main_ff.out <path to video> <number of workers>
```

**Parallel implementation with FastFlow and OpenMP pragmas:**
```
./bin/main_ff.out <path to video> <number of workers> [<workers for rgb2gray>] [<workers for smoothing>] [<workers for motion detection>]
```

**Script to measure latencies of sequential operations:**
```
./bin/seq_funcs_perf_eval.out <path to video>
```
