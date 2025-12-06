"""
FOR each available ingredient:
    FOR ingredient ranges:
        IF available ingredient in range:
            ADD to count
            SKIP to next available ingredient

RETURN count
"""

def get_ingredient_info(input: list) -> tuple[list, list]:
    """
    Takes list of string values and parses this into separate lists.

    Args:
        input (list): list of strings (the puzzle input)

    Returns:
        tuple[list, list]: two lists, one of 'fresh ingredients' and the other of available ingredients
    """
    
    fresh: list = []
    available: list = []
    for row in input:
        # if there's a hyphen, we're looking at 'fresh' ingredients
        if row.find('-') != -1:
            num_strings: list = row.split('-')
            fresh.append([int(num_strings[0]), int(num_strings[1])])
        # if row is empty then skip
        elif row == '':
            continue
        # otherwise we're in the available ingredients values
        else:
            available.append(int(row))
    return (fresh, available)

def count_ingredients(input: list) -> int:
    """
    Takes puzzle input and count ingredients that are 'fresh'

    Args:
        input (list): the puzzle input, a collection of numbers

    Returns:
        int: a count of the ingredients that answers the puzzle
    """
    
    fresh_ingredient_list: list = []
    available_ingredient_list: list = []
    fresh_ingredient_list, available_ingredient_list = get_ingredient_info(input)
    
    count: int = 0
    for available in available_ingredient_list:
        for fresh_range in fresh_ingredient_list:
            if available in range(fresh_range[0], fresh_range[1] + 1):
                count += 1
                break
    
    return count
