from aoc8a_functions import get_product_of_largest_circuits
import time

def main() -> None:
    start: float = time.time()
    
    # read puzzle input
    input_file = open("input.txt", "r")
    
    test_input = """162,817,812
57,618,57
906,360,560
592,479,940
352,342,300
466,668,158
542,29,236
431,825,988
739,650,466
52,470,668
216,146,977
819,987,18
117,168,530
805,96,715
346,949,466
970,615,88
941,993,340
862,61,35
984,92,344
425,690,689
""".splitlines()
    
    #choose which input to use
    
    # test inputs
    # input = test_input
    # connections = 10
    
    # puzzle inputs
    input = []
    for row in input_file:
        input.append(row.rstrip())
    connections = 1000
    
    circuits: int = get_product_of_largest_circuits(input, connections)
    print(f"product of largest circuits: {circuits}")
    
    end: float = time.time()
    print(f"time taken: {(end - start):.3f} sec")

if __name__ == "__main__":
    main()
