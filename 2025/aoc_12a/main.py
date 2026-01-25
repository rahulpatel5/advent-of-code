from aoc12a_functions import get_regions
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """0:
###
##.
##.

1:
###
##.
.##

2:
.##
###
##.

3:
##.
###
##.

4:
###
#..
###

5:
###
.#.
###

4x4: 0 0 0 0 2 0
12x5: 1 0 1 0 2 2
12x5: 1 0 1 0 3 2
""".splitlines()
    
    #choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row.rstrip())
    
    regions: int = get_regions(input)
    print(f"regions that fit presents: {regions}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
