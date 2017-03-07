# Game-of-Life-OpenMP
Game of Life implementation using OpenMP. Boards are commented out to increase runtime. Runtimes output into a text file called `execution_times.txt`. 

### Compilation Instructions

`g++ GoL_OMP.cpp -O3 -fopenmp`

### Execution Instructions 

`./a.out [matrix size] [generations] [threads]`

ex. `./a.out 1000 1000 5`

