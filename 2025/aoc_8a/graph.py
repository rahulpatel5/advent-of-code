class Network:
    vertices = 1
    network = {}
    
    def __init__(self, vertices):
        self.vertices = vertices
        # for node in range(vertices):
        #     self.network[node] = set()
    
    def get_edges(self, node):
        return self.network[node]
    
    def add_edge(self, node, linked_node):
        if node not in self.network:
            self.network[node] = set()
        self.network[node].add(linked_node)
    
    def get_graph(self, node):
        graph = set()
        graph.add(node)
        curr_nodes = [node]
        while len(curr_nodes) > 0:
            next_nodes = []
            for prev_node in curr_nodes:
                for connection in self.network[prev_node]:
                    if connection not in graph:
                        graph.add(connection)
                        next_nodes.append(connection)
            curr_nodes = next_nodes
        return graph

    def get_size_of_graph(self, node):
        return len(self.get_graph(node))
    
    def get_sizes_of_each_graph(self):
        graph_sizes = []
        node_collection = set()
        for node in self.network:
            if node in node_collection:
                continue
            curr_graph = self.get_graph(node)
            for curr_node in curr_graph:
                node_collection.add(curr_node)
            graph_sizes.append(len(curr_graph))
        return graph_sizes
