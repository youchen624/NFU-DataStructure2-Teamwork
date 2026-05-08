#include "UndiLinkedGraph.h"


// return number of edges incident to vertex u
size_t UndiLinkedGraph::degree(Vertex u) const {
    auto the = data.find(u);
    if (the == data.end()) return 0;
    else return the->second.size();
    /*
    {
        size_t t = the->second.size();
        for (const auto& pair : data) {
            t += pair.second.count(u); // set::contains // C20
        }
        return t;
    }
    */
};

// insert edge (u, v) into graph
void UndiLinkedGraph::insert_edge(Vertex u, Vertex v) {
    data[u].insert(v);
    data[v].insert(u).second && ++e;
    // std::unorder_map::[u] maintains uniqueness AND ensure u exists inside
    // std::unordered_set::insert maintains uniqueness
    /*
    auto theA = data.find(u);
    auto theB = data.find(v);
    if (theA == data.end()) {
        data.emplace(u, std::unordered_set<Vertex>{v});
        ++e;
    } else theA->second.insert(v).second && ++e;
    if (theB == data.end()) {
        data.emplace(v, std::unordered_set<Vertex>{u});
        ++e;
    } else theB->second.insert(u).second && ++e;
    data[u].insert(v).second && ++e;
    */
};

// delete v and all edges incident to it
void UndiLinkedGraph::delete_vertex(Vertex v) {
    // check
    auto the = data.find(v);
    if (the == data.end()) return;
    // edge part (others)
    for (const auto& it : the->second) {
        data.at(it).erase(v); // data[it].erase(v);
    }
    // vertex part
    e -= the->second.size();
    data.erase(the);
}

// delete edge (u, v) from the graph
void UndiLinkedGraph::delete_edge(Vertex u, Vertex v) {
    auto the = data.find(u);
    if (the == data.end()) return;
    if (the->second.erase(v)) {
        data.at(v).erase(u); // data[v].erase(u);
        --e;
    }
};


void UndiLinkedGraph::DFS(Vertex start) {
    if (data.find(start) == data.end()) return;
    std::unordered_set<Vertex> visited; // make sure only once
    std::function<void(Vertex)> rec = [&](Vertex pos) {
        visited.insert(pos);
        auto const& the = data.at(pos);
        for (auto const& it : the) {
            if (visited.count(it)) continue;
            rec(it);
        }
    };
    // exe
    rec(start);
}