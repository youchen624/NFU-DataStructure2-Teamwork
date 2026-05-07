#ifndef GRAPH_H
#define GRAPH_H
#define LL long long

#include <vector>

using Vertex = int;
using Weight_t = LL;

/*  // useless
typedef struct {
    Vertex u, v;
} Edge;
typedef struct {
    Vertex u, v;
    Weight_t weigh;
} WEdge;
*/

class Graph {
    /**
     * @property a non-empty set of vertices and a set of undirected edges.
     * where each edge is a pair of vertices.
     */
public:
    virtual ~Graph() {};
    // destructor

    //
    // getter

    virtual bool is_empty() const { return !n; };
    // return true if graph has no vertices

    virtual size_t number_of_vertices() const { return n; };
    // return number of vertices in the graph

    virtual size_t number_of_edges() const { return e; };
    // return number of edges in the graph

    virtual size_t degree(Vertex u) const = 0;
    // return number of edges incident to vertex u

    virtual bool exists_edge(Vertex u, Vertex v) const = 0;
    // return true if graph has the edge (u, v)

    //
    // modify

    virtual void insert_vertex(Vertex v) = 0;
    // insert vertex v into graph; v has no incident edges

    virtual void insert_edge(Vertex u, Vertex v) = 0;
    // insert edge (u, v) into graph

    virtual void delete_vertex(Vertex v) = 0;
    // delete v and all edges incident to it

    virtual void delete_edge(Vertex u, Vertex v) = 0;
    // delete edge (u, v) from the graph

    //
    // algorithm

    virtual void DFS(Vertex start) = 0;

protected:
    size_t n;                      // number of vertices
    size_t e;                      // number of edges
};

#endif // GRAPH_H