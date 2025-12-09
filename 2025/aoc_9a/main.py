from aoc9a_functions import get_largest_area
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """7,1
11,1
11,7
9,7
9,5
2,5
2,3
7,3
""".splitlines()
    
    #choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row.rstrip())
    
    area: int = get_largest_area(input)
    print(f"largest area: {area}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
