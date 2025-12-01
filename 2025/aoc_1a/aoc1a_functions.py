"""
FOR each instruction:
    TURN dial in specified direction and amount
    IF new position is target position:
        INCREMENT final count

RETURN final count
"""

def read_rotation(rotation: str) -> tuple:
    """
    Takes string and finds direction and amount of rotation.

    Args:
        rotation (str): combined data of direction and amount to turn dial

    Returns:
        tuple: 1) the direction to turn the dial and 2) the amount to turn the dial
    """
    
    direction = rotation[0]
    amount = int(rotation[1:])
    
    return (direction, amount)

def update_position(position: int, direction: str, turn_amount: int, dial_limit) -> int:
    """
    Takes the starting position of dial and turns the dial by the set direction and amount.

    Args:
        position (int): starting position of the dial
        direction (str): the direction to turn the dial
        turn_amount (int): the amount that the dial is turned
        dial_limit (int): the maximum value for the dial

    Returns:
        int: the new position of the dial after turning
    """
    
    if direction == "L":
        position -= turn_amount
    elif direction == "R":
        position += turn_amount
    else:
        print(f"Direction to turn is: {direction}")
        raise ValueError("Got an unexpected direction to turn the dial")
    
    while (position < 0 or position > dial_limit):
        if position < 0:
            position += (dial_limit + 1)
        elif position > dial_limit:
            position -= (dial_limit + 1)
    
    return position

def count_zeroes(rotations_list: list, target_position: int, start_position: int, dial_limit: int) -> int:
    """
    Takes puzzle input and counts the number of times that zero is landed on.

    Args:
        rotations_list (list): puzzle input of rotations
        target_position (int): target position to count when landed on
        start_position (int): position that dial starts at
        dial_limit (int): upper point that dial loops around

    Returns:
        int: count of number of zeroes in puzzle input
    """
    
    zero_count = 0
    if start_position == target_position:
        zero_count += 1
    
    current_position = start_position
    for rotation in rotations_list:
        (direction, amount) = read_rotation(rotation)
        current_position = update_position(current_position, direction, amount, dial_limit)
        
        if current_position == target_position:
            zero_count += 1
    
    return zero_count
