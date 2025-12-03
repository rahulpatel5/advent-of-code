from aoc2a_functions import sum_invalid_ids
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124"
    
    #choose which input to use
    # input = test_input
    input = input_file.read().strip()
    
    invalid_ids: int = sum_invalid_ids(input)
    print(f"sum of invalid IDs: {invalid_ids}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
