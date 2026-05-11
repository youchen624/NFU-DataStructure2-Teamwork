#ifndef GRAPH_H
#define GRAPH_H
#define LL long long

#include <vector>

using Vertex = int;
using Weight_t = int; //LL;
using Order_t = size_t;

typedef struct {
    Vertex u, v;
} Edge;
/*  // useless
typedef struct {
    Vertex u, v;
    Weight_t weigh;
} WEdge;
*/

enum class GraphDirection_t {
    Undirection,
    Direction
};
typedef struct {
    ;

    // sequence of Vertices (i -> Vertex) | [ Vertex... ]
    std::vector<Vertex> order;

    // order of each Vertex (Vertex -> order) | { Vertex : Order_t... }
    std::unordered_map<Vertex, Order_t> dfn;

    // earliest reachability (closest to root) | { Vertex : Order_t... }
    std::unordered_map<Vertex, Order_t> low_link;

    // each Vertices' parent | { Vertext : Vertex... }
    std::unordered_map<Vertex, Vertex> parent;
    // each Vertices' childrens | { Vertext : [ Vertex... ]... }
    std::unordered_map<Vertex, std::vector<Vertex>> children;
    // std::unordered_map<Vertex, std::unordered_set<Vertex>> not better

    // articulation points | { Vertex... }
    std::unordered_set<Vertex> articulation_points;
    // using in Undirection Graph

    // spanning tree
    std::vector<Edge> tree_edges;            // T
    // std::vector<Edge> none_tree_edges; // N
} DFS_Result;

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
    // modify-type

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

    // get Depth-First Search
    virtual DFS_Result getDFS(Vertex start) = 0;
    // virtual void getDFS(Vertex start, std::vector<Vertex>& components) = 0; // for saving the order
    
    // get Breadth-First Search
    virtual void getBFS(Vertex start) = 0;
    // virtual void getBFS(Vertex start, std::vector<Vertex>& components) = 0; // for saving the order

    // get Connected Components
    virtual std::vector<std::vector<Vertex>> getComponents() = 0;

    // get Spanning Tree
    virtual std::vector<Edge> getSpanningTree() = 0;

    // get Biconnected Components
    virtual std::vector<Vertex> getBCC() = 0;

protected:
    size_t n;                      // number of vertices
    size_t e;                      // number of edges
};

#endif // GRAPH_H