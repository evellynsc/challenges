# Report
## Find the best lineup

Lineup Generator goal is to choose a subset of nine players in a set of players in order to respect some constraints and guarantee the best combination according some criteria. Because we are interested in finding the best solution from all feasible solutions, a natural way of looking to this problem is as an [optimization problem][optimization]. So it is possible to write the problem as a mathematical model and try to solve it using [mathematical integer program][ip] and very well consolidated tools/algorithms for optimization.

Before presenting the model, let's introduce some notation.
Consider <img src="http://mathurl.com/25htm3r.png" alt="A"> the set of players, <img src="http://mathurl.com/3aj257d.png" alt="T"> the set of teams and <img src="http://mathurl.com/22kf4dt.png" alt="P"> the set of possible positions a player can assume. Each player <img src="http://mathurl.com/5ldlsl.png" alt="i"> has some caracteristics like team, position, salary, ownership, score and so on.

Let's associate for each player <img src="http://mathurl.com/gsbq25f.png" alt="i_in_A"> a binary variable <img src="http://mathurl.com/2az2c7m.png" alt="x_i">, that assumes value 1 if player <img src="http://mathurl.com/5ldlsl.png" alt="i"> is choose for the lineup and 0, otherwise.
Let's also associate for each team <img src="http://mathurl.com/hr89jym.png" alt="j_in_T"> a binary variable <img src="http://mathurl.com/oujkp62.png" alt="z_j">, that assumes value 1 if at least one player <img src="http://mathurl.com/5ldlsl.png" alt="i"> of team <img src="http://mathurl.com/6u2lg2j.png" alt="j"> is choose for the lineup and 0, otherwise. This is a very important variable to impose some mandatory constraints to the problem.
Let's define <img src="http://mathurl.com/zv4xfq9.png" alt="L_k_in_Z"> the necessary number of players for position <img src="http://mathurl.com/zdzqyah.png" alt="k_in_P"> in a valid lineup.

The criteria used to evaluate the lineup is the player scores, so the objective function is to maximize the score sum for the nine selected players. The mathematical model that aims to find the best lineup possible is as following.

<img src="http://mathurl.com/gq4rsjf.png" alt="objective">

subject to

<img src="http://mathurl.com/jlp6zpl.png" alt="constraints">

The objective function, as said before, is to maximize the score sum of the selected players. Constraints (1), (2) and (3) impose some bounds on salary and ownership of players. Constraints (4), (5) and (6) garantees that players from at least three teams will be selected in one lineup. Constraint (7) prevents that no more than four players from the same team will be selected. Constraint (8) tells how many players according to their positions will be selected. Constraint (9) impose the rule that no other player from the same team of the player in the D position can be selected.


## Generate many lineups

The section above presented a mathematical model to find the best possible lineup. However we are interested in generating many lineups 'a little' less valuable than the best that still feasible. We can achieve this in an interative manner, using the same model and adding new constraints as following.

Let <img src="http://mathurl.com/32b5w4t.png" alt="invalidate_solution"> be the set of players of the best lineup given by the model above.
After finding <img src="http://mathurl.com/32b5w4t.png" alt="invalidate_solution">, we add a new constraint to the model that turns <img src="http://mathurl.com/32b5w4t.png" alt="invalidate_solution"> into a invalid lineup. We can achieve this by the constraint below.

<img src="http://mathurl.com/z29ycr2.png" alt="invalidate_solution">

This constraint says that at leat one player from <img src="http://mathurl.com/32b5w4t.png" alt="invalidate_solution"> has to be different. So everytime the best lineup is found, we add the constraint of that lineup and solve the model again. We do this until we have a suficient number of  lineups.

## Perfomance



The lineup generator performance was measured in a machine with a Intel Core i5 CPU @ 2.67GHz and 4 GB of memory RAM, using the output of linux command `time`. The real time measured is reported in the graphic below for generating 1, 10, 50, 100, 200, 400 and 800. Notice that the runtime increase exponentially with the number of lineups.


<img src="performance.png" alt="performance" width="600" height="600">


It is worth to say that the solver used (CBC) doesn't have a good performance in comparison with other solvers, see the graphic below. Probably choose another solver can leverage the performance.

<img src="http://scip.zib.de/images/2016_06_13.png" alt="invalidate_solution">


[ip]: https://en.wikipedia.org/wiki/Integer_programming "Integer Programming"
[optimization]: https://en.wikipedia.org/wiki/Optimization_problem "Optimization Problem"
