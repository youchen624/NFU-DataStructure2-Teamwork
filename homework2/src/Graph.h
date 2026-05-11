#ifndef GRAPH_H
#define GRAPH_H
#define LL long long

// allow getDFS(Vertext v) that v NOT includes in Graph
// disable it if you want NOT allow call with a NOT existing Vertex
#define ALLOW_DFS_START_FROM_NOT_EXISTS

#include <exception>
#include <vector>

using Vertex = int;
using Weight_t = double;
using Order_t = size_t;

// COMPONENTS | EMPTY
struct Empty { };

// COMPONENTS | directed ?
struct Direction {
    struct Directed { static constexpr bool is_directed = true; };
    struct Undirected { static constexpr bool is_directed = false; };
};

// COMPONENTS | weight ?
template <typename T = double>
struct Weight {
    struct Type {
        using ValueType = T;
        static constexpr bool is_weight = true;
    };
    struct None {
        using ValueType = Empty;
        static constexpr bool is_weight = false;
    };
};

// COMPONENTS | storage ...
template <typename WeightType, typename Is_Directed>
struct Storage {
    struct Linked {             // LinkedList
        // static constexpr bool is_weight = WeighType::is_weight;
        using Weight_t = WeightType::ValueType;
        using NB_t = std::conditional_t<    // V -> nb鄰居
            WeightType::is_weight,   // ?:
            std::unordered_map<Vertex, Weight_t>,   // { V : W }
            std::unordered_set<Vertex>                      // { V... }
        >;

        // { Vertex : { Vertex : Weight_t... }... } / { Vertex : { Vertex... } }
        std::unordered_map<Vertex, NB_t> data;
        size_t e;                       // numbers of edges


        //
        // func | getter

        // return true if graph has no vertices
        bool is_empty() const { return !data.size(); };

        // return number of vertices in the graph
        size_t number_of_vertices() const { return data.size(); };

        // return number of edges in the graph
        size_t number_of_edges() const { return e; };

        // return number of edges incident to vertex u
        size_t degree(Vertex u) const {
            auto the = data.find(u);
            if (the == data.end()) return 0;
            if constexpr (Is_Directed::is_directed) {       // IF
                size_t t = the->second.size();
                for (const auto& pair : data) {
                    t += pair.second.count(u); // set::contains // C20
                }
                return t;
            } else {                                                          // ELSE
                return the->second.size();
            }
        };

        // return true if graph has the edge (u, v)
        bool exists_edge(Vertex u, Vertex v) const {
            auto the = data.find(u);
            if (the == data.end()) return false;
            else return the->second.count(v);
        };

        //
        // func | modify

        // insert vertex v into graph; v has no incident edges
        void insert_vertex(Vertex v) {
            if constexpr (WeightType::is_weight) {       // IF
                data.emplace(v, std::unordered_map<Vertex, Weight_t>{});
            } else {                                                       // ELSE
                data.emplace(v, std::unordered_set<Vertex>{});
            }
        };

        // insert edge (u, v) into graph
        void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) {
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
            if constexpr (Is_Directed::is_directed) {     // IF
                if (!data[u].count(v)) ++e;
                insert_vertex(v);    // possible no exists
                if constexpr (WeightType::is_weight) data[u][v] = w;   // IF
                else data[u].insert(v);                                                  // ELSE
            } else {                                                       // ELSE
                if (!data[u].count(v)) ++e;
                if constexpr (WeightType::is_weight) {                        // IF
                    data[u][v] = w;
                    data[v][u] = w;
                } else {                                                                         //ELSE
                    data[u].insert(v);
                    data[v].insert(u);
                }
            }
        };

        // delete v and all edges incident to it
        void delete_vertex(Vertex v) {
            auto the = data.find(v);
            if (the == data.end()) return;

            // edges part
            if constexpr (Is_Directed::is_directed) {                           // IF
                for (auto& [_, sec] : data) if (sec.erase(v)) --e;
            } else {                                                                              // ELSE
                for (const auto& it : the->second) data.at(it).erase(v);
            }

            // vertices part
            e -= the->second.size();
            data.erase(v);
        };

        // delete edge (u, v) from the graph
        void delete_edge(Vertex u, Vertex v) {
            auto the = data.find(u);
            if (the == data.end()) return;
            if (the->second.erase(v)) {
                if constexpr (!Is_Directed::is_directed) {      // IF
                    data.at(v).erase(u); // data[v].erase(u);
                }
                --e;
            }
        };


        //
        // algorithm

        /*
        
DFS_Result DiLinkedGraph::getDFS(Vertex start) const {
    if (is_empty()) return {};

    // start from NOT exists
    if (data.find(start) == data.end())
#ifndef ALLOW_DFS_START_FROM_NOT_EXISTS
        return {};                               // END
#else
        start = data.begin()->first;    // get a RND one
#endif

    DFS_Result res; // save result
    res.components.emplace_back();  // for save [[]]
    std::vector<Vertex>* components_ptr = &res.components.back();
    Order_t counter = 0;
    std::unordered_set<Vertex> on_stack;
    // determine that in a chain from parent, not from other chain

    // stack? for SCC // std::stack<Vertex> stk;

    std::function<void(Vertex)> rec = [&](Vertex pos) {
        on_stack.insert(pos);                // into stack

        // if (!res.dfn.count(pos)) // always true
        components_ptr->push_back(pos); // components

        // order
        res.order.push_back(pos);
        res.dfn[pos] = res.low_link[pos] = counter;
        ++counter;

        auto const& the = data.at(pos);
        for (auto const& npos : the) {
            // iterate all childrens
            if (res.dfn.find(npos) == res.dfn.end()) {
                // never visited
                res.parent[npos] = pos;
                res.children[pos].push_back(npos);
                res.tree_edges.push_back({pos, npos}); // tree
                rec(npos);                             // recursive
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.low_link[npos]
                );
            } else if (on_stack.count(npos)) {
                // been visited AND is in current DFS stack
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.dfn[npos]
                );
            }   // NOT in current DFS stack // else { }
        }
        on_stack.erase(pos);                // END stack
    };
    // exe
    rec(start);
    for (auto const& v : data) {
        // for isolated
        if (!res.dfn.count(v.first)) {
            res.components.emplace_back();
            components_ptr = &res.components.back();
            rec(v.first);
        }
    }
    return res;
}



DFS_Result DiLinkedGraph::getDFS() const {
    if (is_empty()) return {};
    else return getDFS(data.begin()->first);
}
// you should call this function with the DFS result
std::vector<std::vector<Vertex>> const DiLinkedGraph::getCComponents() const {
    return getDFS().components;
};
        */

    };
    struct Matrix {
        static constexpr bool is_weight = WeightType::is_weight;
        using Weight_t = WeightType::ValueType;
    };
};




typedef struct {
    Vertex u, v;
} Edge;

/*  // useless
typedef struct {
    Vertex u, v;
    Weight_t weigh;
} WEdge;
*/

// DFS results (for analyze)
typedef struct {
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

    // components | [ [ Vertex... ]... ]
    std::vector<std::vector<Vertex>> components;

    // spanning trees (allowed forest)
    std::vector<Edge> tree_edges;            // T
    // std::vector<Edge> none_tree_edges; // N
} DFS_Result;


class IGraph {
    /**
     * @property a non-empty set of vertices and a set of undirected edges.
     * where each edge is a pair of vertices.
     */
public:
    virtual ~IGraph() {};
    // destructor

    //
    // getter

    virtual bool is_empty() const = 0;
    // return true if graph has no vertices

    virtual size_t number_of_vertices() const = 0;
    // return number of vertices in the graph

    virtual size_t number_of_edges() const = 0;
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
    virtual DFS_Result getDFS(Vertex start) const = 0;
    virtual DFS_Result getDFS() const = 0;
    
    // get Breadth-First Search
    virtual void getBFS(Vertex start) = 0;
    // virtual void getBFS(Vertex start, std::vector<Vertex>& components) = 0; // for saving the order

    // get Connected Components
    virtual std::vector<std::vector<Vertex>> const& getCComponents(const DFS_Result &dfs) const {
        return dfs.components;
    };
    // you should call this function with the DFS result
    virtual std::vector<std::vector<Vertex>> const getCComponents() const {
        return getDFS().components;
    };

    // get Spanning Tree (return a Forest is possible)
    virtual std::vector<Edge> const& getSpanningTree(const DFS_Result& dfs) const {
        return dfs.tree_edges;
    };
    // you should call this function with the DFS result
    virtual std::vector<Edge> const getSpanningTree() const {
        return getDFS().tree_edges;
    };

    // get Biconnected Components
    virtual std::vector<Vertex> getBCComponents() const = 0;

protected:
    // size_t n;                      // number of vertices
    // size_t e;                      // number of edges
};

template <typename Storage_P>
class Graph : public IGraph {
private:
    Storage_P storage;
public:

    //
    // getter
    bool is_empty() const override { return storage.is_empty(); }
    size_t number_of_vertices() const override { return storage.number_of_vertices(); }
    size_t number_of_edges() const override { return storage.number_of_edges(); }
    size_t degree(Vertex u) const override { return storage.degree(u); }
    bool exists_edge(Vertex u, Vertex v) const override { return storage.exists_edge(u, v); }

    //
    // modify

    void insert_vertex(Vertex v) override { storage.insert_vertex(v); }
    void insert_edge(Vertex u, Vertex v) override {
        storage.insert_edge(u, v);
    }
    void insert_edge(Vertex u, Vertex v, typename Storage_P::Weight_t w) {
        storage.insert_edge(u, v, w);
    }
    void delete_vertex(Vertex v) override { storage.delete_vertex(v); }
    void delete_edge(Vertex u, Vertex v) override { storage.delete_edge(u, v); }

    //
    // algorithm

    // #TODO here
    DFS_Result getDFS(Vertex start) const override {}

    DFS_Result getDFS() const override {
        if (is_empty()) return {};
        return getDFS(storage.data.begin()->first);
    }

    void getBFS(Vertex start) override {}
    
    std::vector<Vertex> getBCComponents() const override { return {}; }
    std::unordered_set<Vertex> getArticulationPoints() {};
};

using DiLinkedGraph = Graph<Storage<Weight<>::None, Direction::Directed>::Linked>;
using UndiLinkedGraph = Graph<Storage<Weight<>::None, Direction::Undirected>::Linked>;
using WDiLinkedGraph = Graph<Storage<Weight<Weight_t>::Type, Direction::Directed>::Linked>;
using WUndiLinkedGraph = Graph<Storage<Weight<Weight_t>::Type, Direction::Undirected>::Linked>;

#endif // GRAPH_H
