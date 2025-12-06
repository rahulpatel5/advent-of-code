from aoc5a_functions import count_ingredients
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """3-5
10-14
16-20
12-18

1
5
8
11
17
32
""".splitlines()
    
    # choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row.rstrip())
    
    ingredients: int = count_ingredients(input)
    print(f"fresh ingredients: {ingredients}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
