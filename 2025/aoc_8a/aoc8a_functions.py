from graph import Network
from operator import itemgetter

"""
first try naive approach:
WHILE connections up to max connections:
    MAKE connections between shortest distance between junctions

FOR n shortest connections:
    MAKE network with edges between nodes

GET size of each network

RETURN product of sizes of largest n networks
"""

def get_product_of_largest_circuits(input: list, connections: int) -> int:
    """
    Takes puzzle input and finds product of largest circuits.

    Args:
        input (list): list of numbers (puzzle input)
        connections (int): number of connections to make between junctions

    Returns:
        int: product of sizes of three largest circuits
    """
    
    # collect junctions info
    junctions: list = []
    for line in input:
        junctions.append(list(map(int, line.split(','))))
    
    # get distance for all connections
    distances: dict = {}
    for junction1 in range(len(junctions)):
        for junction2 in range(junction1 + 1, len(junctions)):
            distance: int = (junctions[junction1][0] - junctions[junction2][0]) ** 2 + (junctions[junction1][1] - junctions[junction2][1]) ** 2 + (junctions[junction1][2] - junctions[junction2][2]) ** 2
            
            distances[junction1, junction2] = distance
    
    # sort dictionary for the next step
    distances = {k: v for k, v in sorted(distances.items(), key=itemgetter(1))}
    
    network: Network = Network(len(junctions))
    connections_made: int = 0
    for nodes in distances:
        if connections_made >= connections:
            break
        network.add_edge(nodes[0], nodes[1])
        network.add_edge(nodes[1], nodes[0])
        connections_made += 1
    
    graph_sizes: list = network.get_sizes_of_each_graph()
    graph_sizes.sort(reverse=True)
    product: int = 1
    for sz in graph_sizes[:3]:
        product *= sz
    
    return product
