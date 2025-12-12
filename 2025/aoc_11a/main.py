from aoc11a_functions import get_out_paths
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """aaa: you hhh
you: bbb ccc
bbb: ddd eee
ccc: ddd eee fff
ddd: ggg
eee: out
fff: out
ggg: out
hhh: ccc fff iii
iii: out
""".splitlines()
    
    #choose which input to use
    # input = test_input
    input = []
    for row in input_file:
        input.append(row.rstrip())
    
    paths: int = get_out_paths(input)
    print(f"different paths: {paths}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
