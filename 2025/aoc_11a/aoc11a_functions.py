"""
FOR each path from 'you':
    IF path reaches 'end':
        ADD to count

RETURN count

how to identify and stop any paths that repeat?
don't worry about it in first instance. Fix if it becomes a problem
"""

def get_out_paths(input: list) -> int:
    """
    Take devices and find all paths to the end.

    Args:
        input (list): information on devices (puzzle input)

    Returns:
        int: count of paths leading to end
    """
    
    devices: dict = {}
    for row in input:
        device: str = row[0 : row.find(':')]
        
        outputs: list = []
        space: int = row.find(' ')
        while space != -1:
            nextSpace: int = row.find(' ', space + 1)
            output: str = ""
            if nextSpace != -1:
                output = row[space + 1 : nextSpace]
            else:
                output = row[space + 1 : ]
            outputs.append(output)
            space = nextSpace
        
        devices[device] = outputs
    
    paths: list = ["you"]
    paths_to_end: int = 0
    while len(paths) > 0:
        new_paths: list = []
        for path in paths:
            for output in devices[path]:
                if output == "out":
                    paths_to_end += 1
                else:
                    new_paths.append(output)
        paths = new_paths
    
    return paths_to_end
