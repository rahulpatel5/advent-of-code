from aoc3a_functions import get_joltage
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """987654321111111
811111111111119
234234234234278
818181911112111
""".splitlines()
    
    #choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row)
    
    joltage: int = get_joltage(input)
    print(f"total joltage: {joltage}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
