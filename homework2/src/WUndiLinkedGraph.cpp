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
    WUndiLinkedGraph::insert_edge(u, v, Weight_t());
};
// insert edge (u, v) into graph with weight
// "weight" will be replaced if the edge has already exists
void WUndiLinkedGraph::insert_edge(Vertex u, Vertex v, Weight_t weight) {
    if (!data[u].count(v) || !data[v].count(u)) ++e;
    data[u][v] = weight;
};

// delete v and all edges incident to it
void WUndiLinkedGraph::delete_vertex(Vertex v) {
    // vertex part
    auto the = data.find(v);
    if (the == data.end()) return;
    e -= the->second.size();
    data.erase(the);

    // edges part (others)
    for (auto& pair : data) {
        if (pair.second.erase(v)) --e;
    }
};

// delete edge (u, v) from the graph
void WUndiLinkedGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the == data.end()) return;
    if (the->second.erase(v)) --e;
};