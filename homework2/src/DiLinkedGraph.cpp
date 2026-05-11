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

DFS_Result DiLinkedGraph::getDFS(Vertex start) {
    if (data.find(start) == data.end()) return {};
    DFS_Result res; // save result
    Order_t counter = 0;
    std::unordered_set<Vertex> on_stack;
    // determine that in a chain from parent, not from other chain

    std::function<void(Vertex)> rec = [&](Vertex pos) {
        on_stack.insert(pos);                // into stack

        // order
        res.order.push_back(pos);
        res.dfn[pos] = res.low_link[pos] = counter;
        ++counter;

        auto const& the = data.at(pos);
        for (auto const& it : the) {        // iterate all childrens
            if (res.dfn.find(it) == res.dfn.end()) {
                // never visit
                res.parent[it] = pos;
                res.children[pos].push_back(it);
                rec(it);                                // recursive
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.low_link[it]
                );
            } else if (on_stack.count(it)) {
                // been visited AND is a parent
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.dfn[it]
                );
            }   // not a parent // else { }
        }
        on_stack.erase(pos);                // end stack
    };
    // exe
    rec(start);
    return res;
}

/*

DFS_Result DiLinkedGraph::getDFS(Vertex start) {
    if (data.find(start) == data.end()) return {};
    DFS_Result res;
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
*/