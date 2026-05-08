#include "WDiLinkedGraph.h"

// return number of edges incident to vertex u
size_t WDiLinkedGraph::degree(Vertex u) const {
    auto the = data.find(u);
    if (the == data.end()) return 0;
    else {
        size_t t = the->second.size();
        for (const auto& pair : data) {
            t += pair.second.count(u);
        }
        return t;
    }
};

// return true if graph has the edge (u, v)
bool WDiLinkedGraph::exists_edge(Vertex u, Vertex v) const {
    auto the = data.find(u);
    if (the == data.end()) return false;
    else return the->second.count(v);
};

// insert vertex v into graph; v has no incident edges
void WDiLinkedGraph::insert_vertex(Vertex v) {
    data.emplace(v, std::unordered_map<Vertex, Weight_t>{});
};


// insert edge (u, v) into graph
// "weight" will be replaced if the edge has already exists
void WDiLinkedGraph::insert_edge(Vertex u, Vertex v, Weight_t weight) {
    if (!data[u].count(v)) ++e;
    data[u][v] = weight;
    // data[u].insert({v, weight}).second && ++e;
};
// insert edge (u, v) into graph
void WDiLinkedGraph::insert_edge(Vertex u, Vertex v) {
    WDiLinkedGraph::insert_edge(u, v, Weight_t());
    // data[u].insert({v, Weight_t()}).second && ++e;
};

// delete v and all edges incident to it
void WDiLinkedGraph::delete_vertex(Vertex v) {
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
void WDiLinkedGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the == data.end()) return;
    if (the->second.erase(v)) --e;
};


void WDiLinkedGraph::DFS(Vertex start) {
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