#include "LinkedDiGraph.h"


// return number of edges incident to vertex u
size_t LinkedDiGraph::degree(Vertex u) const {
    auto the = data.find(u);
    if (the == data.end()) return 0;
    else {
        size_t t = the->second.size();
        for (const auto& pair : data) {
            t += pair.second.count(u); // set::contains // C20
        }
        return t;
    }
};

// return true if graph has the edge (u, v)
bool LinkedDiGraph::exists_edge(Vertex u, Vertex v) const {
    auto the = data.find(u);
    if (the == data.end()) return false;
    else return the->second.count(v);   // set::contains // C20
    /*
    for (const Vertex& n : the->second) {
        if (n == v) return true;
    }
    return false;
    */
};

// insert vertex v into graph; v has no incident edges
void LinkedDiGraph::insert_vertex(Vertex v) {
    data.emplace(v, std::unordered_set<Vertex>{});
};

// insert edge (u, v) into graph
void LinkedDiGraph::insert_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the == data.end()) data.emplace(u, std::unordered_set<Vertex>{v});
    else the->second.insert(v); // std::unordered_set::insert maintains uniqueness
    ++e;
};

// delete v and all edges incident to it
void LinkedDiGraph::delete_vertex(Vertex v) {
    // vertex part
    auto the = data.find(v);
    if (the == data.end()) return;
    e -= the->second.size();
    data.erase(the);
    // edge part (others)
    for (auto& pair : data) {
        if (pair.second.erase(v)) --e;
    }
};

// delete edge (u, v) from the graph
void LinkedDiGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the != data.end()) {
        if (the->second.erase(v)) --e;
    }
};