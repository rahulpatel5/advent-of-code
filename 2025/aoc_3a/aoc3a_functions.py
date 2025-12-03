"""
FOR each line:
    SEARCH for largest number (9) in the earliest position
    IF this number isn't in the last position:
        FIND largest number following this number
    ELSE:
        SEARCH for the next largest number (8)
        REPEAT steps above
    
    ADD largest number to sum

RETURN sum
"""

def get_largest_number(row: str) -> int:
    """
    Take string and find largest possible two-digit number.

    Args:
        row (str): string of digits to find largest number from

    Returns:
        int: largest two-digit number in the string
    """
    
    first_num_str: str = ""
    position1: int = 0
    for i in range(9, 0, -1):
        position1 = row.find(str(i))
        if (position1 != -1) and (position1 != len(row.replace("\n", "")) - 1):
            first_num_str = str(i)
            break
    
    second_num_str: str = ""
    position2: int = position1 + 1
    for j in range(9, 0, -1):
        # keep searching until reaching the end of the row
        index: int = position1 + 1
        while index != -1:
            position2 = row.find(str(j), index)
            index = position2
            if index != -1:
                index += 1
            if (position2 != -1) and (position2 > position1):
                second_num_str = str(j)
                break
        if second_num_str != "":
            break
    
    return int(first_num_str + second_num_str)

def get_joltage(input: list) -> int:
    """
    Take strings and finds sum of largest two digit numbers.

    Args:
        input (list): list of string, which is the puzzle input

    Returns:
        int: the sum of the largest two-digit numbers that could be formed
    """
    
    sum: int = 0
    
    for row in input:
        largest: int = get_largest_number(row)
        sum += largest
    
    return sum
