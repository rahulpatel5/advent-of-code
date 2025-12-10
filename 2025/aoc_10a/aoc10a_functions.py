import itertools
import copy

"""
FOR each machine:
    FOR each button combination:
        IF combination configures machine AND is <fewest presses:
            STORE as fewest presses
    ADD fewest presses to total

RETURN total number of presses
"""

def flip_switches(initial_config: list, combo: tuple) -> list:
    """
    Returns new configuration from pressing buttons in combo.

    Args:
        initial_config (list): initial configuration of machine
        combo (tuple): combination of button presses to make

    Returns:
        list: end configuration of machine after pressing buttons
    """
    # kept going wrong because of shallow list copies
    new_config: list = copy.deepcopy(initial_config)
    for buttons in combo:
        for button in buttons:
            if new_config[button] == 0:
                new_config[button] = 1
            elif new_config[button] == 1:
                new_config[button] = 0
    
    return new_config

def count_fewest_button_presses(input: list) -> int:
    """
    Takes manual (puzzle input) and gets fewest button presses to configure machines.

    Args:
        input (list): string values making up manual (puzzle input)

    Returns:
        int: count of fewest button presses to solve puzzle
    """
    
    presses: int = 0
    
    for row in input:
        if row.find('[') == -1:
            continue
        
        final_config_str: str = row[row.find('[') + 1 : row.find(']')]
        final_configuration: list = []
        for char in final_config_str:
            if char == '#':
                final_configuration.append(1)
            elif char == '.':
                final_configuration.append(0)
            else:
                raise ValueError("Got unexpected value in machine configuration.")
        
        # get button press choices
        index: int = row.find(']')
        schematics: list = []
        while row.find('(', index) != -1:
            start: int = row.find('(', index)
            end: int = row.find(')', index)
            numbers: str = row[start + 1 : end]
            schematics.append(list(map(int, numbers.split(','))))
            index = end + 1
        
        initial_configuration = [0] * len(final_configuration)
        fewest: int = 0
        for i in range(1, len(schematics) + 1):
            for combo in itertools.combinations(schematics, i):
                if fewest > 0 and len(combo) >= fewest:
                    continue
                
                test_config: list = flip_switches(initial_configuration, combo)
                if test_config == final_configuration and (fewest == 0 or len(combo) < fewest):
                    fewest = len(combo)
        
        presses += fewest
    
    return presses
