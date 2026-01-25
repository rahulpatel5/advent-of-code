import numpy as np
import itertools

"""
FOR each region:
    IF all presents fit in region:
        ADD to count

RETURN count

would be annoying to handle things with strings
try converting the presents to a numpy array to speed things up

this type of problem seems to be a polyomino packing problem
seems hard. May need to look up an algorithm if (when) I get stuck

how might I get polyomino packing to work?
FOR each polyomino:
    FIND position and orientation that minimises empty space
but how to know if empty space has been minimised?
can't look at whole region since empty spaces would be same
need to focus on the region where packing has occurred
want to both minimise the overall region and reduce empty space

could look at this space by space (within reason)
I already know this would take too long to parse the puzzle input
but will be useful to see how long it takes

doesn't even solve 2 problems in a reasonable time
definitely too slow to solve this
"""

def get_presents(input: list) -> np.ndarray:
    """
    Take puzzle input and extract present information.
    """
    presents: list = []
    present: list = []
    for row in input:
        if "x" in row:
            break
        
        if ":" in row:
            continue
        if row == "":
            presents.append(np.array(present))
            present = []
            continue
        binary_row: list = [1 if c == '#' else 0 for c in list(row)]
        present.append(binary_row)
    return np.array(presents)

def find_best_way_to_fit_present(filled_region: np.ndarray, present: np.ndarray) -> np.ndarray:
    """
    Find the best position and orientation to add the present to the tree region.
    """
    best_region: np.ndarray = np.array(filled_region)
    min_overall_region: int = 999
    min_empty_spaces: int = 999
    
    # know from input that no shape has missing row or column
    for iy, ix in np.ndindex(filled_region.shape[0] - (present.shape[0] - 1), filled_region.shape[1] - (present.shape[1] - 1)):
        # check each orientation of the present
        for i in range(4):
            present_orientation: np.ndarray = np.rot90(np.array(present), k=i)
            
            padded_present: np.ndarray = np.pad(present_orientation, ((iy, filled_region.shape[0] - present.shape[0] - iy), (ix, filled_region.shape[1] - present.shape[1] - ix)), 'constant')
            filled_region += padded_present
            
            # stop if we have overlap
            if len(np.where(filled_region > 1)[0]) > 0:
                filled_region -= padded_present
                continue
            
            # otherwise, check if this is the best match so far
            filled_area: tuple = np.where(filled_region == 1)
            row_range: int = np.max(filled_area[0]) - np.min(filled_area[0]) + 1
            col_range: int = np.max(filled_area[1]) - np.min(filled_area[1]) + 1
            empty_spaces: int = len(np.where(filled_region[np.min(filled_area[0]):np.max(filled_area[0]), np.min(filled_area[1]):np.max(filled_area[1])] == 0)[0])
            if (row_range * col_range < min_overall_region) or ((row_range * col_range == min_overall_region) and (empty_spaces < min_empty_spaces)):
                best_region = np.array(filled_region)
                min_overall_region = row_range * col_range
                min_empty_spaces = empty_spaces
            
            filled_region -= padded_present
    
    # if adding present fails
    if min_overall_region == 999:
        best_region[0, 0] = -1
    
    return best_region

def can_fit_presents_in_region(tree_region: np.ndarray, quantities: np.ndarray, presents: np.ndarray) -> bool:
    """
    Checks if numbers (quantities) of shapes (presents) can fit in region.
    """
    # initial check that this is possible
    total_area: int = tree_region.shape[0] * tree_region.shape[1]
    present_row_sums: np.ndarray = np.sum(presents, axis=(1,2))
    min_possible_present_area: int = np.sum(np.matmul(quantities, present_row_sums))
    if total_area < min_possible_present_area:
        return False
    
    # can use tree_region as template to add/subtract presents
    # use position to test adding present and test none >1
    # faster to revert change than to store the state
    # try setting up permutations
    non_zero_quantities: np.ndarray = quantities[quantities > 0]
    shape_indices: np.ndarray = np.array(np.where(quantities > 0)[0])
    repeating_quantities: np.ndarray = np.repeat(shape_indices, non_zero_quantities)
    # use set to handle duplicate permutations
    quantity_permutations: set = set(itertools.permutations(repeating_quantities))
    
    # for each permutation, check if adding shapes in this order
    # will fit into the region
    target_presents: int = np.sum(quantities)
    # faster to exclude any permutation that won't work
    # e.g. if not possible for ABC then don't run ABCDE, ABCEF etc
    failed_perm: set = set()
    for perm in quantity_permutations:
        skip: bool = False
        for failed in failed_perm:
            if perm[:len(failed)] == failed:
                skip = True
                break
        if skip:
            continue
        
        filled_region: np.ndarray = np.array(tree_region)
        added_presents: int = 0
        for idx, present in enumerate(perm):
            filled_region = find_best_way_to_fit_present(filled_region, presents[present])
            # if can't add present, put -1 in top-left corner
            if filled_region[0, 0] == -1:
                failed_perm.add(perm[:idx + 1])
                break
            added_presents += 1
        if added_presents == target_presents:
            return True
    
    return False

def get_regions(input: list) -> int:
    """
    Take input and find which regions can fit presents.

    Args:
        input (list): puzzle input

    Returns:
        int: count of regions that fit presents
    """
    
    presents: np.ndarray = get_presents(input)
    
    fit_presents: int = 0
    for row in input:
        if not "x" in row:
            continue
        
        dimensions: list = row.split(":")[0].split("x")
        dimensions = list(map(int, dimensions))
        tree_region: np.ndarray = np.zeros(shape = dimensions, dtype=int)
        
        initial_quantities: list = row.split(":")[1].split(" ")
        initial_quantities = [n for n in initial_quantities if n != ""]
        initial_quantities = list(map(int, initial_quantities))
        quantities: np.ndarray = np.array(initial_quantities)
        
        if can_fit_presents_in_region(tree_region, quantities, presents):
            fit_presents += 1
        print(fit_presents)
    
    return fit_presents
