"""
FOR each range:
    IF number has even number of digits:
        GET digits for first half of number
        IF first half repeated twice is in range:
            ADD number to sum

RETURN sum
"""

def sum_invalid_ids(input: str) -> int:
    """
    Takes input and sums 'invalid IDs'.

    Args:
        input (str): string containing ID ranges

    Returns:
        int: sum of invalid IDs from the input
    """
    
    invalid_id_sum: int = 0
    
    for num_range in input.split(","):
        first: str = num_range.split("-")[0]
        first_num: int = int(first)
        second: str = num_range.split("-")[1]
        second_num: int = int(second)
        
        # skip if both numbers have the same, odd number of digits
        if len(first) == len(second) and len(first) % 2 != 0:
            continue
        
        # it's a pain if the numbers are >1 order of magnitude apart
        # identify if this is the case and raise an error
        # otherwise, we'll ignore this case
        if len(second) - len(first) > 1:
            raise ValueError("Numbers are more than 1 order of magnitude apart. The code doesn't work and needs to be changed.")
        
        # if first number has an odd number of digits, increase number
        if len(first) % 2 != 0:
            first_num = 10 ** len(first)
            first = str(first_num)
        # or if second number has odd digits, reduce number
        elif len(second) % 2 != 0:
            second_num = 10 ** (len(second) - 1) - 1
            second = str(second_num)
        
        # now generate the initial first half of the first number
        first_num__first_half: int = first_num // (10 ** (len(first) // 2))
        second_num__first_half: int = second_num // (10 ** (len(second) // 2))
        
        for i in range(first_num__first_half, second_num__first_half + 1):
            num_to_check: int = i * (10 ** len(str(i))) + i
            if num_to_check >= first_num and num_to_check <= second_num:
                invalid_id_sum += num_to_check
    
    return invalid_id_sum
