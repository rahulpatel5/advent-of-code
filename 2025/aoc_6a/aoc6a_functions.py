"""
READ each line into separate lists, one per column

FOR each list:
    APPLY the maths operation (final value) to each number
    ADD the solution to the sum

RETURN the sum
"""

def perform_maths(maths_list: list) -> int:
    """
    Perform maths operations on each list and return sum of solutions.

    Args:
        maths_list (list): lists of each cephalopod maths problems

    Returns:
        int: grand total (sum) of each solution
    """
    
    sum: int = 0
    for problem in maths_list:
        problem_solution: int = 0
        if problem[-1] == '+':
            for num in problem[:-1]:
                problem_solution += int(num)
        elif problem[-1] == '*':
            problem_solution = 1
            for num in problem[:-1]:
                problem_solution *= int(num)
        else:
            raise ValueError("Got an unexpected maths operator (not '+' or '*').")

        sum += problem_solution
    
    return sum

def do_maths_homework(input: list) -> int:
    """
    Take puzzle input and return solution to the maths homework.

    Args:
        input (list): list of strings with numbers and maths operations (the puzzle input)

    Returns:
        int: the sum of the individual solutions
    """
    
    cephalopod_maths_lists: list = []
    list_size: int = -1
    for line in input:
        value_split: list = line.split()
        
        # for first loop, we need to set up separate nested lists
        if list_size == -1:
            list_size = len(value_split)
            for value in line.split():
                cephalopod_maths_lists.append([value])
        
        else:
            for i in range(list_size):
                cephalopod_maths_lists[i].append(value_split[i])
    
    return perform_maths(cephalopod_maths_lists)
