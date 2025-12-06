from aoc6a_functions import do_maths_homework
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """123 328  51 64 
 45 64  387 23 
  6 98  215 314
*   +   *   +  
""".splitlines()
    
    #choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row.rstrip())
    
    total: int = do_maths_homework(input)
    print(f"grand total: {total}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
