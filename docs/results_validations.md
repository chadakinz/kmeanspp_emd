# Results

In this text we will go over how to run tests and how we verified the algorithm was working as intended. We will also go over 
some issues and areas of improvements.

To run all of the tests, from the root directory run:

```
./build/tests
```
You should see an output like:

```terminal
Randomness seeded to: 4119050203
0.155242 old_objective: 31.0111 new objective: 31.0114
old_objective: 30.9857 new objective: 30.9862
first objective: 40.4344 last objective: 30.9862
===============================================================================
All tests passed (325224 assertions in 14 test cases)
```

This text won't go over all test cases, but each file has a special tag if you want to run those tests independently. To do this
follow the information in the table.


To show this algorithm is doing as intended, we need to show that for each iteration of our algorithm, the objective 
decreases until convergence. The second test we need to show is that the resultant percent point functions generated 
from our algorithms are in fact valid probability mass functions.

To show that our algorithm is in fact converging to a local minimum, we present the `test_get_objective` inside the 
`test_wkmeans_metrics.cpp` file. This test runs the algorithm for 200 steps on 5 test files that each have 6000 sample points. 

<a href="../scripts/graphs/plot_test_file_10.png">View Implementation File</a>