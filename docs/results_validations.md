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
0.155242old_objective: 31.0111 new objective: 31.0114
old_objective: 30.9857 new objective: 30.9862
first objective: 40.4344 last objective: 30.9862
===============================================================================
All tests passed (325224 assertions in 14 test cases)
```

I wont go over all test cases but each file has a special tag if you want to run those tests independently. To do this
follow the information in the table.


The tests that are most important are going to be testing our metrics and results. We want to show that our algorithm 
converges to some local minimum solution for the given objective. We also want to show that the inverse cumulative functions
we generated result to valid pdfs when converted back. 