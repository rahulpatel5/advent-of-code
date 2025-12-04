"""
FOR each roll of paper:
    COUNT rolls of paper in surrounding tiles
    IF <4:
        ADD one to sum

RETURN sum
"""

def check_for_roll_of_paper_in_tile(row_pos: int, col_pos: int, map: list) -> bool:
    """
    Checks if a roll of paper is at a specific tile in the map.

    Args:
        row_pos (int): row position of map that is being checked
        col_pos (int): column position of map that is being checked
        map (list): map (the puzzle input)

    Returns:
        bool: true if there is a roll of paper at the tile, false if not
    """
    
    return map[row_pos][col_pos] == '@'

def count_adjacent_tiles(row_index: int, col_index: int, map: list) -> int:
    """
    Takes position and finds count of rolls of paper in adjacent tiles.

    Args:
        row_index (int): row position in map
        col_index (int): column position in map
        map (list): map (the puzzle input)

    Returns:
        int: count of rolls of paper in adjacent tiles
    """
    
    count: int = 0
    
    # search above, if not on top row
    if row_index > 0:
        # search top left, if not on left edge
        if col_index > 0:
            if check_for_roll_of_paper_in_tile(row_index - 1, col_index - 1, map):
                count += 1
        # search directly above
        if check_for_roll_of_paper_in_tile(row_index - 1, col_index, map):
            count += 1
        # search top right, if not on right edge
        if col_index < len(map[0]) - 1:
            if check_for_roll_of_paper_in_tile(row_index - 1, col_index + 1, map):
                count += 1
    
    # search left, if not on left edge
    if col_index > 0:
        if check_for_roll_of_paper_in_tile(row_index, col_index - 1, map):
            count += 1
    # search right, if not on right edge
    if col_index < len(map[0]) - 1:
        if check_for_roll_of_paper_in_tile(row_index, col_index + 1, map):
            count += 1
    
    # seach below, if not on bottom edge
    if row_index < len(map) - 1:
        # search bottom left, if not on left edge
        if col_index > 0:
            if check_for_roll_of_paper_in_tile(row_index + 1, col_index - 1, map):
                count += 1
        # search directly below
        if check_for_roll_of_paper_in_tile(row_index + 1, col_index, map):
            count += 1
        # search bottom right, if not on right edge
        if col_index < len(map[0]) - 1:
            if check_for_roll_of_paper_in_tile(row_index + 1, col_index + 1, map):
                count += 1
    
    return count

def count_accessible_paper(input: list) -> int:
    """
    Takes puzzle input and counts how many rolls of paper can be accessed

    Args:
        input (list): puzzle input of map with rolls of paper marked

    Returns:
        int: count of accessible rolls of paper
    """
    
    count: int = 0
    
    for row_index, row in enumerate(input):
        for col_index, position in enumerate(row):
            if position == '.':
                continue
            
            adjacent_rolls: int = count_adjacent_tiles(row_index, col_index, input)
            if adjacent_rolls < 4:
                count += 1
    
    return count
