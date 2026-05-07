#include "DiLinkedGraph.h"


// return number of edges incident to vertex u
size_t DiLinkedGraph::degree(Vertex u) const {
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
bool DiLinkedGraph::exists_edge(Vertex u, Vertex v) const {
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
void DiLinkedGraph::insert_vertex(Vertex v) {
    data.emplace(v, std::unordered_set<Vertex>{});
};

// insert edge (u, v) into graph
void DiLinkedGraph::insert_edge(Vertex u, Vertex v) {
    data[u].insert(v).second && ++e;
    // std::unorder_map::[u] maintains uniqueness AND ensure u exists inside
    // std::unordered_set::insert maintains uniqueness
    /*
    auto the = data.find(u);
    if (the == data.end()) {
        data.emplace(u, std::unordered_set<Vertex>{v});
        ++e;
    } else the->second.insert(v).second && ++e;
    */
};

// delete v and all edges incident to it
void DiLinkedGraph::delete_vertex(Vertex v) {
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
void DiLinkedGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the == data.end()) return;
    if (the->second.erase(v)) --e;
};