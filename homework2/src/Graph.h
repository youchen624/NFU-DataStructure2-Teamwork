#ifndef GRAPH_H
#define GRAPH_H
#define LL long long

// allow getDFS(Vertext v) and getBFS(Vertex v) that v NOT includes in Graph
// disable it if you want NOT allow call with a NOT existing Vertex
#define ALLOW_FS_START_FROM_NOT_EXISTS

#include <exception>
#include <optional>
// #include <type_traits>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>

using Vertex = int;
using Weight_t_d = double;
using Order_t = size_t;

typedef struct {
    Vertex u, v;
} Edge;
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
    // using in Undirection-Graph only

    // connected components | [ [ Vertex... ]... ]
    std::vector<std::vector<Vertex>> components;
    // for Undirected-Graph

    // BCC | [ [ Edge... ]... ]
    std::vector<std::vector<Edge>> bcc_edges;
    // for Undirected-Graph

    // SCC
    // for Directed-Graph

    // spanning trees (allowed forest)
    std::vector<Edge> tree_edges;            // T
    // std::vector<Edge> none_tree_edges; // N
} DFS_Result;

typedef struct {
    std::vector<Vertex> order;
    std::unordered_map<Vertex, Vertex> parent;
    std::unordered_map<Vertex, std::vector<Vertex>> children;
} BFS_Result;

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

        //
        // data

        // { Vertex : { Vertex : Weight_t... }... } / { Vertex : { Vertex... } }
        std::unordered_map<Vertex, NB_t> data;
        size_t e = 0;                       // numbers of edges


        //
        // func | helper
        template<typename T>
        static Vertex get_npos(const T& item) {
if constexpr (WeightType::is_weight)
            return item.first;
else
            return item;
        };


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
            // insert_vertex(u);    // possible no exists
            insert_vertex(v);    // possible no exists
            if (!data[u].count(v)) ++e;

            // helper
            auto add_edge = [&](Vertex from, Vertex to, Weight_t weight) {
if constexpr (WeightType::is_weight)
                data[from][to] = weight;
else
                data[from].insert(to);
            };

            add_edge(u, v, w);
if constexpr (!Is_Directed::is_directed) {     // IFN
            add_edge(v, u, w);
}
/*
else {                                                       // ELSE
if constexpr (WeightType::is_weight) {                        // IF
data[u][v] = w;
data[v][u] = w;
} else {                                                                         //ELSE
data[u].insert(v);
data[v].insert(u);
}
}
*/
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

        // #TODO move to namespace, not in Graph/Storage class/struct anymore
        DFS_Result getDFS(Vertex start) const {
            if (is_empty()) return {};

            // start from NOT exists
            if (data.find(start) == data.end())
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
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
            // stack for SCC ? // std::stack<Vertex> stk;

            // res.dfn => visited ? { Vertex : Order_t }


            // resolved //~~ # TO DO NOT WORK in all instances~~
            // rec function
            std::function<void(Vertex, std::optional<Vertex>)> rec  // REC BEGIN ==== ==== |
            = [&](Vertex pos, std::optional<Vertex> par) {  // par = std::nullopt
                on_stack.insert(pos);                // BEGIN stack

                // if (!res.dfn.count(pos)) // always true
                components_ptr->push_back(pos); // components

                // order
                res.order.push_back(pos);
                res.dfn[pos] = res.low_link[pos] = counter;
                ++counter;
                
                size_t children_counting = 0;
                auto const& the = data.at(pos);
                for (auto const& item : the) {                          // FOR BEGIN ==== ==== |
                    const Vertex npos = get_npos(item);
if constexpr (!Is_Directed::is_directed) {  // undirected
                    // in undirected case, bypass direct-parent
                    if (par.has_value() && par.value() == npos) continue;
}
                    // iterate all childrens
                    if (res.dfn.find(npos) == res.dfn.end()) {
                        // never visited
                        res.parent[npos] = pos;
                        res.children[pos].push_back(npos);
                        res.tree_edges.push_back({pos, npos}); // tree
                        ++children_counting;        //
                        rec(npos, pos);                                           // CALL recursive
                        res.low_link[pos] = std::min(
                            res.low_link[pos],
                            res.low_link[npos]
                        );

if constexpr (!Is_Directed::is_directed) {  // undirected
                        // if (u != start && low[v] >= dfn[u])
                        if (par.has_value() && res.low_link.at(npos) >= res.dfn.at(pos))
                            res.articulation_points.insert(pos);
}
                    } else {                                                     // been visited
if constexpr (Is_Directed::is_directed) {   // directed
                        if (on_stack.count(npos)) {
                            // AND is in current DFS stack
                            // (pos -> npos) is a back-edge
                            res.low_link[pos] = std::min(
                                res.low_link[pos],
                                res.dfn[npos]
                            );
                        }
} else {                                                      // undirected
                        res.low_link[pos] = std::min(
                            res.low_link[pos],
                            res.dfn[npos]
                        );
}
                    }   // NOT in current DFS stack // else { }
                }
if constexpr (!Is_Directed::is_directed) {  // root articulation points
                if (!par.has_value() && children_counting > 1) res.articulation_points.insert(pos);
}
                on_stack.erase(pos);                // END stack
            };                                                          // END REC
            // exe
            rec(start, std::nullopt);
            for (auto const& v : data) {
                // for isolated => forest
                if (!res.dfn.count(v.first)) {
                    res.components.emplace_back();
                    components_ptr = &res.components.back();
                    rec(v.first, std::nullopt);
                }
            }
            return res;
        };


/*
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





/*  // useless
typedef struct {
    Vertex u, v;
    Weight_t weigh;
} WEdge;
*/


// virtual basic class | for pointer using only
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
class BasicGraph : public Graph {
private:
    Storage_P storage;
public:

    //
    // getter
    bool is_empty() const override { return storage.is_empty(); };
    size_t number_of_vertices() const override { return storage.number_of_vertices(); };
    size_t number_of_edges() const override { return storage.number_of_edges(); };
    size_t degree(Vertex u) const override { return storage.degree(u); };
    bool exists_edge(Vertex u, Vertex v) const override { return storage.exists_edge(u, v); };

    //
    // modify

    void insert_vertex(Vertex v) override { storage.insert_vertex(v); };
    void insert_edge(Vertex u, Vertex v) override {
        storage.insert_edge(u, v);
    };
    void insert_edge(Vertex u, Vertex v, typename Storage_P::Weight_t w) {
        storage.insert_edge(u, v, w);
    };
    void delete_vertex(Vertex v) override { storage.delete_vertex(v); };
    void delete_edge(Vertex u, Vertex v) override { storage.delete_edge(u, v); };

    //
    // algorithm

    // # TO DO here
    DFS_Result getDFS(Vertex start) const override { return storage->getDFS(start); };

    DFS_Result getDFS() const override {
        if (is_empty()) return {};
        return getDFS(storage.data.begin()->first);
    };

    void getBFS(Vertex start) override {}
    
    std::vector<Vertex> getBCComponents() const override { return {}; }
    std::unordered_set<Vertex> getArticulationPoints() {};
};

//
// classes

using DiLinkedGraph = BasicGraph<Storage<Weight<>::None, Direction::Directed>::Linked>;
using UndiLinkedGraph = BasicGraph<Storage<Weight<>::None, Direction::Undirected>::Linked>;
using WDiLinkedGraph = BasicGraph<Storage<Weight<Weight_t_d>::Type, Direction::Directed>::Linked>;
using WUndiLinkedGraph = BasicGraph<Storage<Weight<Weight_t_d>::Type, Direction::Undirected>::Linked>;

#endif // GRAPH_H
