# CP-CPS

## Output redirection

As explained in the problem statement, the program reads from `stdin` and the solution is printed to `stdout`.
As debug information, the non-optimal solutions are printed to `stderr`. When running the program, this debug
information can be avoided redirecting stderr to null by adding `2>/dev/null` at the end of the command.

## How to run

There are two ways of running the program. First one is for a single instance.

```
make
./main < instance-name
```

The second one runs a script that redirects the output to some folder `out/` with the proper format.

```
./run_instances.sh
```

The second way is assuming a folder called `instances/` exists one level above the current one, as well
as it assumes that the `out/` folder already exists to output the solutions.