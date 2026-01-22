import numpy as np
import cvxpy
import math

"""
FOR each machine:
    FOR each button combination:
        IF combo meets joltage requirement AND combo <fewest presses:
            STORE as fewest presses
    ADD fewest presses to total

RETURN total

use permutations to try different sequences of button combinations
for now, assume that if any longer button sequence can be pressed
that this will lead to the fewest button presses
(there could be situations where skipping longer ones is better,
but don't want to get into that headache unless I have to)

there may be a smarter way to do this
have information about which combinations contribute for each button
and we have the number of presses we need for each joltage, e.g.
{5, 5, 5, 5}
(0, 1), (1, 2, 3), (1, 3), (2, 3)
since there's only one button affecting 0, we know that has to be pressed
if it's split between two buttons, some combo of the two must add up to 5
i.e. can form and solve algebraic equations to get solutions
assume equations will be linear, so only have one solution

there are sometimes more variables than equations
so Gaussian elimination or Cramer's rule won't work for these equations
note: some variables (button presses) may be 0

I'll start with using an iterative approach to try to find solutions
doing this checking each iteration doesn't work - it takes too long
try an approach that jumps around to find a solution, e.g. using gradients

use max amount for each set of buttons AND
that sum of sets of buttons must add up to certain amounts (using joltages)
find one solution that fits the sum criteria THEN
look at difference with puzzle and how sets of buttons affect output
to figure out what to change to fully solve a sub-problem
the main issue left will be getting the fewest number of presses BUT
should be ok since have ordered sets by number of buttons
so can prioritise the earlier sets

use numpy's linear algebra functions to find the inverse matrix, s(-1)
based on the logic:
solution * schematics = joltages
so, solution = joltages * s(-1)
this hopefully gets to the solution
different functions for square vs non-square matrices
issue is float precision, so need to check solutions to find a working one

issue is that solution to a matrix product could be unique or many
check if matrix is invertible to check if it's unique
otherwise, need to use a different approach to get the best of many

using the least squares solver of cvxpy
found that inverted square matrix approach can give wrong solutions
but also seem to be getting wrong solutions with cvxpy
need to resort to looking up other approaches to figure out what is wrong
"""

def get_joltage_requirements(row: str) -> np.ndarray:
    """
    Get joltage requirements from puzzle input.
    """
    start: int = row.find('{')
    end: int = row.find('}')
    numbers: str = row[start + 1 : end]
    return np.array(list(map(int, numbers.split(','))))

def get_schematics(row: str, joltage: np.ndarray) -> np.ndarray:
    """
    Get a matrix of button presses from puzzle input.
    """
    schematics: list = []
    zero_array: np.ndarray = np.zeros(joltage.shape, dtype=int)
    index: int = row.find(']')
    row_index: int = 0
    while row.find('(', index) != -1:
        start: int = row.find('(', index)
        end: int = row.find(')', index)
        numbers: str = row[start + 1 : end]
        inner_array: np.ndarray = np.array(zero_array)
        for num in list(map(int, numbers.split(','))):
            inner_array[num] = 1
        schematics.append(inner_array)
        index = end + 1
        row_index += 1
    return np.array(schematics)

def reorder_schematics(schematics: np.ndarray) -> np.ndarray:
    """
    Reorder the sets of button presses in descending order of presses.
    """
    changed: bool = True
    while (changed):
        changed = False
        prev_sz: int = 0
        for idx, arr in enumerate(schematics):
            if idx == 0:
                prev_sz = np.sum(arr)
                continue
            if np.sum(arr) > prev_sz:
                schematics[[idx - 1, idx]] = schematics[[idx, idx - 1]]
                changed = True
            prev_sz = np.sum(arr)
    
    return schematics

def matrix_multiplication(matrix1: np.ndarray, matrix2: np.ndarray) -> np.ndarray:
    """
    Gets the matrix product of two matrices.
    """
    return np.matmul(matrix1, matrix2)

def matrix_solver(schematics: np.ndarray, joltages: np.ndarray, fixed_position: int, fixed_value: int, sentinel_value: int) -> int:
    """
    Uses cvxpy package to solve matrix multiplication equation/problem.
    """
    # cvxpy documentation: https://www.cvxpy.org/examples/applications/nonneg_matrix_fact.html
    solution = cvxpy.Variable(shape=(1, schematics.shape[0]), integer=True, nonneg=True)
    
    fixed_constraint = [solution[0, fixed_position] == fixed_value]
    # add weights to prioritise earlier indices
    # this will minimise the number of button presses
    weights: np.ndarray = np.arange(1, solution.shape[1] + 1)
    
    obj = cvxpy.Minimize(cvxpy.sum_squares(joltages - solution @ schematics) + 1e-6 * (weights @ solution.T))
    prob = cvxpy.Problem(obj, fixed_constraint)
    prob.solve(solver=cvxpy.SCIP)
    # need to handle floating point accuracy issues
    solved = np.array(solution.value)
    solved = np.rint(solved)
    # make sure we have converted to integer correctly
    assert np.isclose(solved, np.array(solution.value), atol=1e-6).all(), f"Got a solution that wasn't converted correctly to an integer solution.\n{np.array(solution.value)}\nvs:\n{solved}"
    if not (joltages == matrix_multiplication(solved, schematics)).all():
        return sentinel_value
    if (solved < 0).any():
        raise ValueError(f"Have a solution with a negative element:\n{solved}")
    return round(np.sum(solved))

def find_answer_lower_bound(joltages: np.ndarray, schematics: np.ndarray) -> int:
    """
    Find lower bound to solutions to puzzle.
    """
    # absolute minimum is the max number of presses in one position
    abs_min: int = np.max(joltages)
    row_sums: np.ndarray = np.sum(schematics, axis=1)
    max_row: int = np.max(row_sums)
    other_min: int = math.ceil(np.sum(joltages) / max_row)
    return other_min if (other_min > abs_min) else abs_min

def solve_with_matrices(schematics: np.ndarray, joltages: np.ndarray) -> int:
    """
    Find fewest presses through computational solution.

    Args:
        schematics (np.ndarray): matrix of button presses, longest first
        joltages (np.ndarray): joltage requirements for problem

    Returns:
        int: fewest button presses to solve the problem
    """
    sentinel_value: int = 999999
    min_solution: int = sentinel_value
    min_possible_answer: int = find_answer_lower_bound(joltages, schematics)
    
    row_sums: np.ndarray = np.sum(schematics, axis=1)
    used_sizes: list = []
    is_first_loop: bool = True
    while ((min_solution == sentinel_value) and (len(used_sizes) < len(np.unique(row_sums)))) or is_first_loop:
        # discount any button press sizes that have already been looked at
        adjusted_arr: np.ndarray = np.full(row_sums.shape, True)
        for size in used_sizes:
            adjusted_arr = adjusted_arr & (row_sums != size)
        next_most_used_button: int = np.max(row_sums[adjusted_arr])
        used_sizes.append(next_most_used_button)
        next_buttons: tuple = np.where(row_sums == next_most_used_button)
        
        for fixed_position in next_buttons[0]:
            # find max number of button presses for this position
            position_row: np.ndarray = schematics[fixed_position, :]
            row_dot: np.ndarray = position_row * joltages
            max_value: int = np.min(row_dot[row_dot > 0])
            
            for fixed_value in range(max_value + 1, 0, -1):
                new_min: int = matrix_solver(schematics, joltages, fixed_position, fixed_value, sentinel_value)
                if new_min < min_solution:
                    min_solution = new_min
                    if min_solution == min_possible_answer:
                        return min_solution
        
        is_first_loop = False
    
    if min_solution == sentinel_value:
        raise ValueError(f"Didn't find a solution for:\n{schematics}\n{joltages}")
    if min_solution < min_possible_answer:
        raise ValueError(f"Got a solution ({min_solution}) smaller than the expected smallest solution ({min_possible_answer}), for:\n{schematics}\n{joltages}")
    return min_solution

def get_total_presses(input: list) -> int:
    """
    Takes joltage requirements and returns fewest button presses.

    Args:
        input (list): button presses and joltage requirements (puzzle)

    Returns:
        int: fewest number of button presses
    """
    
    presses: int = 0
    
    for row in input:
        if row.find('{') == -1:
            continue
        
        joltages: np.ndarray = get_joltage_requirements(row)
        # get button press choices
        schematics: np.ndarray = get_schematics(row, joltages)
        schematics: np.ndarray = reorder_schematics(schematics)
        
        min: int = solve_with_matrices(schematics, joltages)
        presses += int(round(min))
    
    return presses
