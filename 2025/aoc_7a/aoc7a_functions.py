import re

"""
FIND start position

FOR each row:
    TRACK current column for beam(s)
    IF splitter in same column as a beam:
        SPLIT beam - remove existing and add to adjacent if not there
        ADD to count of splits

RETURN count
"""

def checkNoAdjacentSplitters(input: list) -> None:
    """
    Checks if there are adjacent splitters and, if so, raises an error.
    """
    for row in input:
        if "^^" in row:
            raise ValueError("Assumed that there are no adjacent splitters. But found an instance of this.")

def find_starting_column(input: list) -> tuple[int, int]:
    """
    Takes input and finds starting column, as a list object.
    """
    for row_index, row in enumerate(input):
        if 'S' in row:
            return (row.find('S'), row_index)
    raise ValueError("Did not find a starting position.")

def count_beam_splits(input: list) -> int:
    """
    Takes puzzle input (diagram) and counts number of times beam splits.

    Args:
        input (list): diagram (puzzle input)

    Returns:
        int: count of times that beams split
    """
    
    # start by checking something we'll assume
    # that no splitters are in adjacent positions
    checkNoAdjacentSplitters(input)
    
    (beam_start, start_row) = find_starting_column(input)
    beams: list = [beam_start]
    split_count: int = 0
    for row in input[start_row + 1:]:
        if row.find('^') == -1:
            continue
        
        splitter_locations: list = [match.start() for match in re.finditer(re.escape('^'), row)]
        for s in splitter_locations:
            if s not in beams:
                continue
            
            # only add split beams if they're not already there
            # we checked no adjacent splitters so this should work fine
            if (s - 1) not in beams:
                beams.append(s - 1)
            if (s + 1) not in beams:
                beams.append(s + 1)
            beams.remove(s)
            split_count += 1
    
    return split_count
