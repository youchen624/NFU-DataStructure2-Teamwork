#include "LinkedGraph.h"


size_t LinkedGraph::degree(Vertex u) const {
    // return number of edges incident to vertex u
    if (number_of_vertices() <= u) throw std::out_of_range("Out of Range");
    return data[u].size();
};

bool LinkedGraph::exists_edge(Vertex u, Vertex v) const {
    // return true if graph has the edge (u, v)
    auto the = data.find(v);
    if (the == data.end()) return false;
    for (const Vertex& n : the->second) {
        if (n == v) return true;
    }
    return false;
};

void LinkedGraph::insert_vertex(Vertex v) {
    // insert vertex v into graph; v has no incident edges
    data.emplace(v, std::list<Vertex>{});
};

void LinkedGraph::insert_edge(Vertex u, Vertex v) {
    // insert edge (u, v) into graph

    // #TODO check exist? then insert (directly?)
};

void LinkedGraph::delete_vertex(Vertex v) {
    // delete v and all edges incident to it
};

void LinkedGraph::delete_edge(Vertex u, Vertex v) {
    // delete edge (u, v) from the graph
};