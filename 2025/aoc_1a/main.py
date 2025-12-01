from aoc1a_functions import count_zeroes
import time

# read puzzle input
input_file = open("input.txt", "r")

test_input = """L68
L30
R48
L5
R60
L55
L1
L99
R14
L82""".splitlines()

# choose which input to use
# input = test_input
input = input_file

# collect input into a list
rotations_list = []
for line in input:
    rotations_list.append(line.strip())

start_time = time.time()

start_position = 50
target_position = 0
dial_limit = 99
number_of_zeroes = count_zeroes(rotations_list, target_position, start_position, dial_limit)
print(f"The number of zeroes was: {number_of_zeroes}")

end_time = time.time()
print(f"Code took {end_time - start_time:.3f}s to run.")
