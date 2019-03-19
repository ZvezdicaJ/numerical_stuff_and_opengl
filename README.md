### My mixed bag
The repository contains all sorts of things - sorting algorithms, vectorized trigonometric functions, various shape meshes,...

Build:
* create build directory
* cd build && cmake ..
* make
  * make sort_benchamrks
  * make runTests


### Running benchmarks
There is an unfinished script in benchmarks directory called run_benchmarks.sh. For now, all it does, is that it sets the cpu frequency to 2000MHz, or at least it is supposed to. Before running the script one has to build sort_benchmarks. Benchmarks require you to install [google benchmark](https://github.com/google/benchmark) library.

### Running tests
To run the tests make runTests. Tests require you to install [googletest](https://github.com/google/googletest) library.
