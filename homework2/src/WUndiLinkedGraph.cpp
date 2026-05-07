#include "WUndiLinkedGraph.h"

// WUndiLinkedGraph

// return true if graph has the edge (u, v)
bool WUndiLinkedGraph::exists_edge(Vertex u, Vertex v) const {
    return
        WDiLinkedGraph::exists_edge(u, v) ||
        WDiLinkedGraph::exists_edge(v, u);
};

// insert edge (u, v) into graph
void WUndiLinkedGraph::insert_edge(Vertex u, Vertex v) {
    ;
};
// insert edge (u, v) into graph with weight
void WUndiLinkedGraph::insert_edge(Vertex u, Vertex v, Weight_t weight) {
    // #TODO 雙向檢查
    if (!data[u].count(v)) ++e;
    data[u][v] = weight;
};

// delete v and all edges incident to it
void WUndiLinkedGraph::delete_vertex(Vertex v) {};

// delete edge (u, v) from the graph
void WUndiLinkedGraph::delete_edge(Vertex u, Vertex v) {};