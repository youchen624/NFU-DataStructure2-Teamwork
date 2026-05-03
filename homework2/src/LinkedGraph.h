#ifndef LINKED_GRAPH
#define LINKED_GRAPH

#include <unordered_map>
#include <list>
#include <stdexcept>
#include "Graph.h"


class LinkedGraph : public Graph {
public:
    LinkedGraph();
    ~LinkedGraph();
    // destructor

    // size_t number_of_vertices() const override;


    size_t degree(Vertex u) const override;
    // return number of edges incident to vertex u

    bool exists_edge(Vertex u, Vertex v) const override;
    // return true if graph has the edge (u, v)

    void insert_vertex(Vertex v) override;
    // insert vertex v into graph; v has no incident edges

    void insert_edge(Vertex u, Vertex v) override;
    // insert edge (u, v) into graph

    void delete_vertex(Vertex v) override;
    // delete v and all edges incident to it

    void delete_edge(Vertex u, Vertex v) override;
    // delete edge (u, v) from the graph

protected:
    std::unordered_map<Vertex, std::list<Vertex>> data;
    // strictly, Vertices in Graph is not orderly
};

#endif