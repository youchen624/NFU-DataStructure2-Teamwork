#ifndef W_DI_LINKED_GRAPH_H
#define W_DI_LINKED_GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "Graph.h"


class WDiLinkedGraph : public Graph {
    /***
     * this class doesn't mater weight logic when modifying
     */
public:
    WDiLinkedGraph() {};
    ~WDiLinkedGraph() = default;
    // destructor

    //
    // getter

    virtual bool is_empty() const override { return data.empty(); };
    // return true if graph has no vertices

    virtual size_t number_of_vertices() const override { return data.size(); };
    // return number of vertices in the graph

    // no modify
    // virtual size_t number_of_edges() const { return e; };
    // return number of edges in the graph

    virtual size_t degree(Vertex u) const override;
    // return number of edges incident to vertex u

    virtual bool exists_edge(Vertex u, Vertex v) const override;
    // return true if graph has the edge (u, v)

    //
    // modify-type

    virtual void insert_vertex(Vertex v) override;
    // insert vertex v into graph; v has no incident edges

    virtual void insert_edge(Vertex u, Vertex v) override;
    // insert edge (u, v) into graph

    virtual void insert_edge(Vertex u, Vertex v, Weight_t weight);
    // insert edge (u, v) into graph with weight
    // "weight" will be replaced if the edge has already exists

    virtual void delete_vertex(Vertex v) override;
    // delete v and all edges incident to it

    virtual void delete_edge(Vertex u, Vertex v) override;
    // delete edge (u, v) from the graph

    //
    // algorithm

    virtual void getDFS(Vertex start) override;

protected:
    // { Vertex : { Vertex: Weight_t... } }... }
    std::unordered_map<Vertex, std::unordered_map<Vertex, Weight_t>> data;
    // strictly, Vertices in Graph is not orderly
};

#endif // W_DI_LINKED_GRAPH_H