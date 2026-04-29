#include <iostream>
#include <vector>

class Graph {
    /**
     * @property a non-empty set of vertices and a set of undirected edges.
     * where each edge is a pair of vertices.
     */
public:
    virtual ~Graph() {};
    // destructor

    bool is_empty() const { return !n; };
    // return true if graph has no vertices

    int number_of_vertices() const { return n; };
    // return number of vertices in the graph

    int number_of_edges() const { return e; };
    // return number of edges in the graph

    virtual int degree(int u) const = 0;
    // return number of edges incident to vertex u

    virtual bool exists_edge(int u, int v) const = 0;
    // return true if graph has the edge (u, v)

    virtual void insert_vertex(int v) const = 0;
    // insert vertex v into graph; v has no incident edges

    virtual void insert_edge(int u, int v) const = 0;
    // insert edge (u, v) into graph

    virtual void delete_vertex(int v) const = 0;
    // delete v and all edges incident to it

    virtual void delete_edge(int u, int v) const = 0;
    // delete edge (u, v) from the graph

private:
    int n;                      // number of vertices
    int e;                      // number of edges
};