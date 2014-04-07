import sh
import networkx as nx

G = nx.DiGraph()

for line in sh.cat(sh.glob('sample_tsv/*')):
    pair = line.strip().split('\t')
    if len(pair) == 2:
        G.add_edge(pair[0], pair[1])

pr = nx.pagerank(G, max_iter=100, tol=1e-5)
for node, value in pr.iteritems():
    # For comparision with GraphLab implementation,
    # we scale up with the number of nodes.
    print node, value * G.number_of_nodes()
