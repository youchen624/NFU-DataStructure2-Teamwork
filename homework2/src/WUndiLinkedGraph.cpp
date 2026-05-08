#include "WUndiLinkedGraph.h"

// WUndiLinkedGraph

// return true if graph has the edge (u, v)
// bool WUndiLinkedGraph::exists_edge(Vertex u, Vertex v) const {
//     return
//         WDiLinkedGraph::exists_edge(u, v) ||
//         WDiLinkedGraph::exists_edge(v, u);
// };

size_t WUndiLinkedGraph::degree(Vertex u) const {
    auto the = data.find(u);
    if (the == data.end()) return 0;
    else return the->second.size();
};

// insert edge (u, v) into graph
void WUndiLinkedGraph::insert_edge(Vertex u, Vertex v) {
    WUndiLinkedGraph::insert_edge(u, v, Weight_t());
};
// insert edge (u, v) into graph with weight
// "weight" will be replaced if the edge has already exists
void WUndiLinkedGraph::insert_edge(Vertex u, Vertex v, Weight_t weight) {
    if (!exists_edge(u, v)) ++e;
        data[u][v] = weight;
        data[v][u] = weight;
};

// delete v and all edges incident to it
/*
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
*/

// delete edge (u, v) from the graph
void WUndiLinkedGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the != data.end() && the->second.erase(v)) {
        --e;
        return;
    }
    auto othe = data.find(v);
    if (othe != data.end() && othe->second.erase(u))
        --e;
};

// delete v and all edges incident to it
void WUndiLinkedGraph::delete_vertex(Vertex v) {
    // check
    auto the = data.find(v);
    if (the == data.end()) return;
    // edge part (others)
    for (const auto& it : the->second) {
        data.at(it.first).erase(v); // data[it].erase(v);
    }
    // vertex part
    e -= the->second.size();
    data.erase(the);
};
    
void WUndiLinkedGraph::DFS(Vertex start) {
    if (data.find(start) == data.end()) return;
    std::unordered_set<Vertex> visited; // make sure only once
    std::function<void(Vertex)> rec = [&](Vertex pos) {
        visited.insert(pos);
        auto const& the = data.at(pos);
        for (auto const& it : the) {
            if (visited.count(it.first)) continue;
            rec(it.first);
        }
    };
    // exe
    rec(start);
}