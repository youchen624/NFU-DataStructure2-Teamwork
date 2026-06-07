// requires C++17 or above
#ifndef GRAPH_H
#define GRAPH_H
#define LL long long
#define ID_t size_t

// allow getDFS(Vertext v) and getBFS(Vertex v) that v NOT includes in Graph
// disable it if you want NOT allow call with a NOT existing Vertex
#define ALLOW_FS_START_FROM_NOT_EXISTS

// all #includes
#include <type_traits>
#include <exception>
#include <stdexcept>
#include <optional>
#include <functional>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <cmath>


namespace yGraph { // // namespace yGraph


using Vertex = int;
// if use others not double, have to figure out with "INF" problem
using Weight_t = double;
// is weight infinity
inline constexpr bool is_inf(Weight_t w) noexcept { return std::isinf(w); };
inline constexpr Weight_t get_inf() noexcept { return std::numeric_limits<Weight_t>::infinity(); };
using Order_t = size_t;

struct Edge {
    Vertex u, v;
    Weight_t weight = 0.0;

    explicit operator bool() const {
        return !(u == v);
    }

    bool operator<(const Edge& that) const {
        return this->weight < that.weight;
    }
    bool operator>(const Edge& that) const {
        return this->weight > that.weight;
    }
};

using Edges = std::vector<Edge>;
template<typename T>
using MinHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;

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
    std::vector<Edges> bcc_edges;
    // for Undirected-Graph

    // SCC
    // for Directed-Graph

    // spanning trees (allowed forest)
    Edges tree_edges;            // T
    // Edges none_tree_edges; // N
} DFS_Result;

typedef struct {
    // sequence of Vertices (i -> Vertex) | [ Vertex... ]
    std::vector<Vertex> order;

    // order of each Vertex (Vertex -> order) | { Vertex : Order_t... }
    std::unordered_map<Vertex, Order_t> bfn;

    // each Vertices' parent | { Vertext : Vertex... }
    // std::unordered_map<Vertex, Vertex> parent;
    // each Vertices' childrens | { Vertext : [ Vertex... ]... }
    // std::unordered_map<Vertex, std::vector<Vertex>> children;
} BFS_Result;

namespace {     // namespace null

struct FS_callback {
    void operator()(Vertex, Vertex, Vertex) const {};
};

};          // namespace null

// COMPONENTS | EMPTY
// struct Empty { };
// # TO DO REMOVE Empty; force all Edges have weight

// COMPONENTS | directed ?
struct Direction {
    struct Directed { static constexpr bool is_directed = true; };
    struct Undirected { static constexpr bool is_directed = false; };
};

// COMPONENTS | weight ?
// template <typename T = double>
struct Weight {
    struct Type {
        // using ValueType = T;
        static constexpr bool is_weighted = true;
    };
    struct None {
        // using ValueType = Empty;
        static constexpr bool is_weighted = false;
    };
};

// COMPONENTS | storage ...
template <typename WeightType, typename Is_Directed>
struct Storage {
    struct Linked {             // LinkedList
        static constexpr bool is_weighted = WeightType::is_weighted;
        // using Weight_t = WeightType::ValueType;
        static constexpr bool is_directed = Is_Directed::is_directed;
        using NB_t = std::conditional_t<    // V -> nb鄰居
            WeightType::is_weighted,   // ?:
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
        // get npos/Vertex
        static Vertex get_wv(const T& item) {
if constexpr (WeightType::is_weighted)
            return item.first;
else
            return item;
        };

        // get weight by (u, v); // you have to promise that is exist or it will returns 0
        inline Weight_t get_w(const Vertex u, const Vertex v) const {
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
if constexpr (is_weighted) {
            const auto it = data.find(u);
            if (it == data.end()) return 0;
            const auto& it_map = it->second;
            const auto v_it = it_map.find(v);
            if (v_it != it_map.end()) return v_it->second;
}
            return 0;
        };

        Edge get_edge(const Vertex pos, const Vertex npos) const { // , const Weight_t& w // auto get
            // think about should design as "return std::pair<Edge e, bool exist>" ?
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
            const auto it = data.find(pos);
            if (it == data.end() || it->second.find(npos) == it->second.end())
                return Edge{};
if constexpr (WeightType::is_weighted) {         // weight
            return Edge{pos, npos, data.at(pos).at(npos)};
} else {                                                          // non-weight
            return Edge{pos, npos};
}
        };

        Vertex _get_a_vertex() const {
            return data.begin()->first;
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
} else {                              // ELSE
            return the->second.size();
}
        };

        // return true if graph has the vertex u
        bool exists_vertex(Vertex u) const {
            return (data.find(u) != data.end());
        };

        // return true if graph has the edge (u, v)
        bool exists_edge(Vertex u, Vertex v) const {
            auto the = data.find(u);
            if (the == data.end()) return false;
            else return the->second.count(v);
        };

        // get all edges in Graph
        Edges get_edges() const {
            Edges edges;
            for (auto const& [pos, nbs] : data) {
                for (auto const& item : nbs) {
                    Vertex npos = get_wv(item);
if constexpr (!is_directed)
                    if (pos > npos) continue;
                    edges.push_back(get_edge(pos, npos));
                }
            }
            return edges;
        };

        // return all neighbors of Vertex u in Graph
        std::vector<Vertex> get_NBs(Vertex u) const {
            if (!data.count(u)) return {};
            std::vector<Vertex> res;
            res.reserve(data.size());
            for (auto const item : data.at(u)) {
                res.push_back(get_wv(item));
            }
            return res;
        };
        

        /**
         * // forEach neighbors
         * @param u Vertex
         * @param callback function(Vertex, Weight_t);
         */
        void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const {
            if (data.count(u)) {
                for (const auto& item : data.at(u)) {
                    const Vertex v = get_wv(item);
                    const Weight_t w = get_w(u, v);
                    callback(v, w);
                }
            }
        };

        /**
         * // forEach Vertex
         * @param callback function(Vertex)
         */
        void forEach_vertex(std::function<void(Vertex)> callback) const {
            for (const auto& [v, _] : data) {
                callback(v);
            }
        };

        //
        // func | modify

        // insert vertex v into graph; v has no incident edges
        void insert_vertex(Vertex v) {
if constexpr (WeightType::is_weighted) {       // IF
            data.emplace(v, std::unordered_map<Vertex, Weight_t>{});
} else {                                                       // ELSE
            data.emplace(v, std::unordered_set<Vertex>{});
}
        };

        // insert edge (u, v) into graph
        void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) {
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
            insert_vertex(u);    // possible no exists  // could be disappeared, bc data[u] is able to auto-insert
            insert_vertex(v);    // possible no exists
            if (!data[u].count(v)) ++e;

            // helper func
            auto add_edge = [&](Vertex from, Vertex to, Weight_t weight) {
if constexpr (WeightType::is_weighted)
                data[from][to] = weight;
else
                data[from].insert(to);
            };

            add_edge(u, v, w);
if constexpr (!is_directed) {     // IFN
            add_edge(v, u, w);
}
/*
else {                                                       // ELSE
if constexpr (WeightType::is_weighted) {                        // IF
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
if constexpr (Is_Directed::is_directed) {                      // IF
            for (auto& [_, sec] : data) if (sec.erase(v)) --e;
} else {                                                                         // ELSE
            for (auto& it : the->second) {
                data.at(get_wv(it)).erase(v);
    /*
    if constexpr (WeightType::is_weighted) {
                auto 
    } else {
                data.at(it)->second.erase(v);
    }
    */
            }
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
        // algorithm removed
        /*
        DFS_Result getDFS(Vertex start) const {
            if (is_empty()) return {};

            // start from NOT exists
            if (data.find(start) == data.end())
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
                return {};                               // END
                // or throw Error
#else
                start = _get_a_vertex();    // get a RND one
#endif

            DFS_Result res; // save result
            res.components.emplace_back();  // for save [[]]
            std::vector<Vertex>* components_ptr = &res.components.back();
            Order_t counter = 0;
            std::unordered_set<Vertex> on_stack;
            // determine that in a chain from parent, not from other chain
            // stack for SCC ? // std::stack<Vertex> stk;

            // bcc used for undirected Graph
            / *
            using bcc_stack_t = std::conditional_t<
            WeightType::is_weighted,
            WEdge<Weight_t>,
            Edge
            >;
            * /
            std::stack<Edge> bcc_stack;

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
                    const Vertex npos = get_wv(item);
if constexpr (!Is_Directed::is_directed) {  // undirected
                    // in undirected case, bypass direct-parent
                    if (par.has_value() && par.value() == npos) continue;
}
                    // iterate all childrens
                    auto const& dfn_npos = res.dfn.find(npos);
                    if (dfn_npos == res.dfn.end()) {      // never visited | #### #### | #### #### |

                        res.parent[npos] = pos;
                        res.children[pos].push_back(npos);
                        ++children_counting;        //

                        Edge e = get_edge(pos, npos);
                        res.tree_edges.push_back(e);            // spanning tree (forest)
if constexpr (!Is_Directed::is_directed) {  // undirected
                        bcc_stack.push(e);
    / *
    if constexpr (WeightType::is_weighted) {         // weight
                        Edge e = Edge{pos, npos, data.at(pos).at(npos)};
                        res.tree_edges.push_back(e); // tree
                        bcc_stack.push(e);
    } else {                                                          // non-weight
                        Edge e = Edge{pos, npos};
                        res.tree_edges.push_back(e); // tree
                        bcc_stack.push(e);
    }
    * /
}
                        //
                        //                                                // CALL recursive | BEGIN
                        rec(npos, pos);
                        //                                                // END recursive
                        //
                        res.low_link[pos] = std::min(
                            res.low_link[pos],
                            res.low_link[npos]
                        );                                      // update low-link

                        //
                        // # TO DO #HERE undi
                        // if never visited, push Edge{u, v}
                        // after rec(u,v) (u -> v)
                        // if low(v) >= dfn(u) meaning exist bcc
                        // start pushing from bcc_stack
                        // until edge {u, v}

if constexpr (!Is_Directed::is_directed) {  // undirected
                        // if (u != start && low[v] >= dfn[u])
                        // BCC
                        if (res.low_link.at(npos) >= res.dfn.at(pos)) {
                            if (par.has_value()) res.articulation_points.insert(pos);
                            Edges bcc_tmp;
                            while (true) {  // popping
                                auto edge = bcc_stack.top();
                                bcc_stack.pop();
                                bcc_tmp.push_back(edge);
                                if (
                                    (edge.u == pos && edge.v == npos) ||
                                    (edge.u == npos && edge.v == pos)
                                ) break;
                            }
                            res.bcc_edges.push_back(bcc_tmp);
                        }
                        // if (par.has_value() && res.low_link.at(npos) >= res.dfn.at(pos))
                            // res.articulation_points.insert(pos);
}

                    } else {                           // been visited | #### #### | #### #### |

if constexpr (Is_Directed::is_directed) {   // directed
                        if (on_stack.count(npos)) {
                            // AND is in current DFS stack
                            // (pos -> npos) is a back-edge
                            if (res.dfn[npos] < res.dfn[pos]) res.low_link[pos] = std::min(
                                res.low_link[pos],
                                res.dfn[npos]
                            );
                        }
} else {                                                      // undirected
                        res.low_link[pos] = std::min(
                            res.low_link[pos],
                            res.dfn[npos]
                        );

                        if (res.dfn[npos] < res.dfn[pos]) {
                            bcc_stack.push(get_edge(pos, npos));
                            / *
                            if constexpr (WeightType::is_weighted) {
                                bcc_stack.push(Edge{pos, npos, data.at(pos).at(npos)});
                            } else {
                                bcc_stack.push(Edge{pos, npos});
                            }
                            * /
                        }
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
        */

        /*
        BFS_Result getBFS(Vertex start) const {
            if (is_empty()) return {};

            // start from NOT exists
            if (data.find(start) == data.end())
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
                return {};                               // END
#else
                start = _get_a_vertex();    // get a RND one
#endif
            BFS_Result res;
            Order_t counter = 0;
            // res.bfn -> visited

            // in queueing
            std::queue<Vertex> queueing;
            const auto q_push = [&](const Vertex p) {
                queueing.push(p);
                res.order.push_back(p);
                res.bfn[p] = counter;   // is a visited mark same time
                ++counter;
            };

            // queueing.push(start);
            q_push(start);
            
            while (!queueing.empty()) {
                Vertex pos = queueing.front();
                queueing.pop();
                // res.order.push_back(pos);
                // res.bfn.insert(pos, counter);
                // ++counter;

                for (auto const& next : data.at(pos)) {
                    Vertex npos = get_wv(next);
                    if (res.bfn.count(npos)) continue;
                    q_push(npos);
                };
            }

            return res;
        };
       */ 


    };
    struct Matrix {
        static constexpr bool is_weighted = WeightType::is_weighted;
        // using Weight_t = WeightType::ValueType;
        static constexpr bool is_directed = Is_Directed::is_directed;

        // using id
        std::vector<std::vector<Weight_t>> data;    // [ [ w ] ] | [u][v] = w;
        // Bimap
        std::unordered_map<Vertex, ID_t> id;  // { Vertex : id } | v -> id
        std::vector<Vertex> vid;                        // { id : Vertex }  | id -> v

        size_t e = 0;   // numbers of edges

        //
        // func | helper
        Weight_t get_w(Vertex u, Vertex v) const {
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
if constexpr (is_weighted) {
            const auto u_it = id.find(u);
            const auto v_it = id.find(v);
            if ((u_it != id.end()) && (v_it != id.end()))
                return data[u_it->second][v_it->second];
}
            return 0;
        };

        Vertex _get_a_vertex() const {
            return vid[0];
        };

        // return Edge(u, v, w); // return default Edge if NOT exist
        Edge get_edge(Vertex u, Vertex v) const {
            // think about should design as "return std::pair<Edge e, bool exist>" ?
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
            const auto u_it = id.find(u);
            const auto v_it = id.find(v);
            if (
                (u_it == id.end()) ||
                (v_it == id.end()) ||
                is_inf(data[u_it->second][v_it->second])    // be marked as none by INF
            ) return Edge{};
if constexpr (is_weighted) {
            return Edge{u, v, data[u_it->second][v_it->second]};
} else {
            return Edge{u, v};
}
        };

        //
        // func | getter

        // return true if graph has no vertices
        bool is_empty() const { return !data.size(); };

        // return number of vertices in the graph
        size_t number_of_vertices() const { return data.size(); };

        // return number of edges in the graph
        size_t number_of_edges() const { return e; };

        // #TODO makes degree cache from a mem that update by modify functions
        // return number of edges incident to vertex u
        size_t degree(Vertex u) const {
            auto const it = id.find(u);     // [v : id]
            if (it == id.end()) return 0; // Vertex not exist => 0 degree
            size_t count = 0;
            for (size_t i = 0; i < data.size(); ++i) {
                if (i == it->second) ++i;   // bypass self
                if (!is_inf(data[i][it->second])) ++count;
if constexpr (Is_Directed::is_directed)
                if (!is_inf(data[it->second][i])) ++count;
            }
            return count;
        };

        // return true if graph has the vertex u
        bool exists_vertex(Vertex u) const {
            return (id.find(u) != id.end());
        };

        // return true if graph has the edge (u, v)
        bool exists_edge(Vertex u, Vertex v) const {
            if (u == v) return false;
            if ((!id.count(u)) || (!id.count(v))) return false;
            const ID_t pos = id.at(u);
            const ID_t npos = id.at(v);
            if (is_inf(data[pos][npos])) return false;
            else return true;
        };
        bool exists_edge(Edge e) const { return exists_edge(e.u, e.v); };

        // get all edges in Graph
        Edges get_edges() const {
            Edges edges;
            // for (auto const& [column, rows] : data) {
            for (ID_t i_c = 0; i_c < data.size(); ++i_c) {
                /* column means col_id ; row means row_id
                // rows is not real rows, it is a [ ] in [ ]
                * c c c c
                r x x x X
                r x x x X
                r x x x X
                r x x x X
                c, r are id
                X are in same [ ]vector
                */
                Vertex pos = vid[i_c];
                // for (auto const& [row, weight] : rows) {
                for (ID_t i_r = 0; i_r < data.size(); ++i_r) {
                    Vertex npos = vid[i_r];
                    if (pos == npos) continue;      // self edge
                    Weight_t weight = data[i_c][i_r];
                    if (is_inf(weight)) continue;       // edge not exists
                    if constexpr (!Is_Directed::is_directed) if (pos > npos) continue;
                    edges.push_back(Edge{pos, npos, weight});
                }
            }
            return edges;
        };

        // return all neighbors of Vertex u in Graph
        std::vector<Vertex> get_NBs(Vertex u) const {
            if (!id.count(u)) return {};
            std::vector<Vertex> res;
            const ID_t ui = id.at(u);
            // res.reserve(data.size());
            for (ID_t i = 0; i < data.size(); ++i) {
                if ((!is_inf(data[ui][i])) && ui != i)
                    res.push_back(vid[i]);  // id->Vertex
            }
            return res;
            // return [x...] that all (u -> x)
        };
        
        /**
         * // forEach neighbors
         * @param u Vertex
         * @param callback function(Vertex, Weight_t);
         */
        void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const {
            const auto u_id_it = id.find(u);
            if (u_id_it == id.end()) return;
            const ID_t u_id = u_id_it->second;
            for (ID_t i = 0; i < data.size(); ++i) {
                if (is_inf(data[u_id][i]) || (u_id == i)) continue;
                callback(vid[i], data[u_id][i]);
            }
        };

        /**
         * // forEach Vertex
         * @param callback function(Vertex)
         */
        void forEach_vertex(std::function<void(Vertex)> callback) const {
            for (const Vertex v : vid) {
                callback(v);
            }
        };

        //
        // func | modify
        
        // insert vertex v into graph; v has no incident edges
        void insert_vertex(Vertex v) {
            if (id.count(v)) return;    // already exists
            const ID_t i = data.size();
            id.insert({v, i});  // start from index 0
            vid.push_back(v);
            const Weight_t inf_w = get_inf();
            for (auto& rows : data) {
                rows.push_back(inf_w);
            }
            data.emplace_back(i+1, inf_w);
            data[i][i] = 0; // self = 0
        };

        // insert edge (u, v) into graph
        void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) {
            if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
            insert_vertex(u);    // possible no exists
            insert_vertex(v);    // possible no exists

            const ID_t pos = id.at(u);
            const ID_t npos = id.at(v);
            if (is_inf(data[pos][npos])) ++e;
            data[pos][npos] = w;
if constexpr (!Is_Directed::is_directed)
            data[npos][pos] = w;
        }
        
        // delete v and all edges incident to it
        void delete_vertex(Vertex v) {
            // find it -> change as last -> pop &&-> set [i][i] = INF &&-> update id and vid
            auto it = id.find(v);   // it->second == be d id // old index
            if (it == id.end()) return;  // not exists
            const Vertex lv = vid.back();  // last v
            for (ID_t i = 0; i + 1 < data.size(); ++i) {  // bypass last one
                if (it->second != i) {  // do not count [i][i]
                    if (!is_inf(data[it->second][i])) --e;
if constexpr (Is_Directed::is_directed)
                    if (!is_inf(data[i][it->second])) --e;
                }
                data[it->second][i] = data[data.size() - 1][i];
                data[i][it->second] = data[i][data.size() - 1];
                data[i].pop_back();
            }
            data.pop_back();
            
            // self edge = 0
            if (data.size() > it->second)
                data[it->second][it->second] = 0;

            // id mapping
            // id vid
            vid[it->second] = lv; // id -> v
            id[lv] = it->second;   // v -> id
            vid.pop_back();
            id.erase(it);
            // #TODO confirm no bug
        };

        // delete edge (u, v) from the graph
        void delete_edge(Vertex u, Vertex v) {
            if (u == v) return;
            if ((!id.count(u)) || (!id.count(v))) return; // a Vertex not exists => edge impossible exists
            const ID_t pos = id.at(u);
            const ID_t npos = id.at(v);
            if (is_inf(data[pos][npos])) return;         // edge not exists
            data[pos][npos] = get_inf();
if constexpr (!Is_Directed::is_directed)
            data[npos][pos] = get_inf();
            --e;
        };

    };
    // solved // # TO DO fix matrix data[i][i] = INF problem in Floyd-Warshall case
};

/*
// special Bimap
template<typename VT, typename KT = ID_t>
class SBimap {
    public:
    VT v(KT id) const {};
    KT k(VT v) const {};
    private:
    std::unordered_map<VT, KT> v_id;
    std::vector<VT> id_v;   // must fix if KT != ID_t
};
*/

class IStorage {
public:
    virtual ~IStorage() {};
    //
    // const functions

    // how many of Vertices in storage
    virtual size_t count_vertices() const = 0;
    // how many of Edges in storage
    virtual size_t count_edges() const = 0;

    // is a Vertex exists?
    virtual bool exists_vertex(Vertex u) const = 0;
    // is an Edge exists?
    virtual bool exists_edge(Vertex u, Vertex v) const = 0;

    // get completed Edge (with Weight)
    virtual Edge get_edge(const Vertex pos, const Vertex npos) const = 0;
    // get all Edges in storage (with Weight)
    virtual Edges get_edges() const = 0;
    
    // get a Vertex from storage
    // <Vertex v, bool x> x meaning whether exists
    virtual std::pair<Vertex, bool> _get_a_vertex() const = 0;
    
    /**
     * // forEach neighbors // all (u -> v)
     * @param u Vertex
     * @param callback function(Vertex, Weight_t);
     */
    virtual void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const = 0;
    /**
     * // forEach Vertex // all Vertex in storage
     * @param callback function(Vertex)
     */
    virtual void forEach_vertex(std::function<void(Vertex)> callback) const = 0;

    //
    // modifiers functions

    // insert Vertex v into storage; without inserts any Edges
    virtual void insert_vertex(Vertex v) = 0;
    // insert Edge (u, v, (w)) into storage; also add Vertex if it is NOT exists
    virtual void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) = 0;
    // insert Edge (u, v, (w)) into storage; also add Vertex if it is NOT exists
    virtual void insert_edge(Edge e) { insert_edge(e.u, e.v, e.weight); };

    // delete Vertex and all Edges incident to it
    virtual void delete_vertex(Vertex v) = 0;
    // delete Edge (u, v) from the storage
    virtual void delete_edge(Vertex u, Vertex v) = 0;
    // delete Edge (u, v) from the storage
    virtual void delete_edge(Edge e) { delete_edge(e.u, e.v); };
private:
};

template<bool DIRECTED, bool WEIGHTED>
class Linked_STG : public IStorage {
private:
    //
    // type
using NB_t = std::conditional_t<            // V -> nb鄰居
        WEIGHTED,                               // ?:
        std::unordered_map<Vertex, Weight_t>,   // { V : W }
        std::unordered_set<Vertex>              // { V... }
    >;

    //
    // data

    // { Vertex : { Vertex : Weight_t... }... } OR
    // { Vertex : { Vertex... } }
    std::unordered_map<Vertex, NB_t> data;
    size_t e = 0;                               // numbers of edges

    //
    // func | helper

    // get npos/Vertex
    template<typename T>
    static Vertex _get_wv(const T& item) {
if constexpr (WEIGHTED)
        return item.first;
else
        return item;
    };

    // get weight by (u, v);
    // you have to promise that is exist or it will returns 0
    Weight_t _get_w(const Vertex u, const Vertex v) const {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");  // if make it possible, must fix the logic
if constexpr (WEIGHTED) {
        const auto it = data.find(u);
        if (it == data.end()) return 0;
        const auto& it_map = it->second;
        const auto v_it = it_map.find(v);
        if (v_it != it_map.end()) return v_it->second;
}
        return 0;
    };

    void _add_edge(Vertex from, Vertex to, Weight_t weight) {
if constexpr (WEIGHTED)
        data[from][to] = weight;
else
        data[from].insert(to);
    };

public:
    //
    // const functions
    size_t count_vertices() const override { return data.size(); };
    size_t count_edges() const override { return e; };

    bool exists_vertex(Vertex u) const override {
        return (data.find(u) != data.end());
    };
    bool exists_edge(Vertex u, Vertex v) const override {
        auto the = data.find(u);
        if (the == data.end()) return false;
        else return the->second.count(v);
    };

    Edge get_edge(const Vertex pos, const Vertex npos) const override {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");
        // ^ if make it possible, must fix the logic
        // (u, u) meaning NOT exists
        const auto it = data.find(pos);
        if (it == data.end() || it->second.find(npos) == it->second.end())
            return Edge{};
if constexpr (WEIGHTED) {         // weight
        return Edge{pos, npos, data.at(pos).at(npos)};
} else {                                                          // non-weight
        return Edge{pos, npos};
}
    };
    Edges get_edges() const override {
        Edges edges;
        for (auto const& [pos, nbs] : data) {
            for (auto const& item : nbs) {
                Vertex npos = _get_wv(item);
if constexpr (!DIRECTED)
{   if (pos > npos) continue;   }
                edges.push_back(get_edge(pos, npos));
            }
        }
        return edges;
    };
    
    std::pair<Vertex, bool> _get_a_vertex() const override {
        if (!count_vertices()) return {0, false};
        else return {data.begin()->first, true};
    };
    
    void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const override {
        if (data.count(u)) {
            for (const auto& item : data.at(u)) {
                const Vertex v = _get_wv(item);
                const Weight_t w = _get_w(u, v);
                callback(v, w);
            }
        }
    };
    void forEach_vertex(std::function<void(Vertex)> callback) const override {              
        for (const auto& [v, _] : data) {
            callback(v);
        }
    };

    //
    // modifiers functions
    void insert_vertex(Vertex v) override {
if constexpr (WEIGHTED) {
        data.emplace(v, std::unordered_map<Vertex, Weight_t>{});
} else {
        data.emplace(v, std::unordered_set<Vertex>{});
}
};
    void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) override {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");
        // if make it possible, must fix the logic
        insert_vertex(u);
        // could be disappeared, bc data[u] is able to auto-insert
        insert_vertex(v);
        if (!data[u].count(v)) ++e;

        _add_edge(u, v, w);
if constexpr (!DIRECTED) {
        _add_edge(v, u, w);
}
    };
    // void insert_edge(Edge e) override { insert_edge(e.u, e.v, e.weight); };

    void delete_vertex(Vertex v) override {
        auto the = data.find(v);
        if (the == data.end()) return;

        // edges part
if constexpr (DIRECTED) {
        for (auto& [_, sec] : data)
            if (sec.erase(v)) --e;
} else {
        for (auto& it : the->second) {
            data.at(_get_wv(it)).erase(v);
        }
}

        // vertices part
        e -= the->second.size();
        data.erase(v);
    };
    void delete_edge(Vertex u, Vertex v) override {
        auto the = data.find(u);
        if (the == data.end()) return;
        if (the->second.erase(v)) {
if constexpr (!DIRECTED) {      // IF
            data.at(v).erase(u); // data[v].erase(u);
}
            --e;
        }
    };
    // void delete_edge(Edge e) override { delete_edge(e.u, e.v); };
};


template<bool DIRECTED, bool WEIGHTED>
class Matrix_STG : public IStorage {
private:
    //
    // data

    // [ [ w ] ] | [u][v] = w;  // by id
    std::vector<std::vector<Weight_t>> data;
    // Bimap
    std::unordered_map<Vertex, ID_t> id;    // { Vertex : id }  | v  -> id
    std::vector<Vertex> vid;                // { id : Vertex }  | id -> v

    size_t e = 0;                           // numbers of edges

    //
    // helper
    Weight_t _get_w(Vertex u, Vertex v) const {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");
        // if make it possible, must fix the logic
if constexpr (WEIGHTED) {
        const auto u_it = id.find(u);
        const auto v_it = id.find(v);
        if ((u_it != id.end()) && (v_it != id.end()))
            return data[u_it->second][v_it->second];
}
        return 0;
    };

public:
    //
    // const functions

    // how many of Vertices in storage
    size_t count_vertices() const override { return data.size(); };
    // how many of Edges in storage
    size_t count_edges() const override { return e; };

    // is a Vertex exists?
    bool exists_vertex(Vertex u) const override {
        return (id.find(u) != id.end());
    };
    // is an Edge exists?
    bool exists_edge(Vertex u, Vertex v) const override {
        if (u == v) return false;
        if ((!id.count(u)) || (!id.count(v))) return false;
        const ID_t pos = id.at(u);
        const ID_t npos = id.at(v);
        if (is_inf(data[pos][npos])) return false;
        else return true;
    };

    // get completed Edge (with Weight)
    Edge get_edge(const Vertex pos, const Vertex npos) const override {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");
        // if make it possible, must fix the logic
        const auto u_it = id.find(u);
        const auto v_it = id.find(v);
        if (
            (u_it == id.end()) ||
            (v_it == id.end()) ||
            is_inf(data[u_it->second][v_it->second])
            // be marked as none by INF
        ) return Edge{};
if constexpr (WEIGHTED) {
        return Edge{u, v, data[u_it->second][v_it->second]};
} else {
        return Edge{u, v};
}
    };
    // get all Edges in storage (with Weight)
    Edges get_edges() const override {
        Edges edges;
        // for (auto const& [column, rows] : data) {
        for (ID_t i_c = 0; i_c < data.size(); ++i_c) {
            /* column means col_id ; row means row_id
            // rows is not real rows, it is a [ ] in [ ]
            * c c c c
            r x x x X
            r x x x X
            r x x x X
            r x x x X
            c, r are id
            X are in same [ ]vector
            */
            Vertex pos = vid[i_c];
            // for (auto const& [row, weight] : rows) {
            for (ID_t i_r = 0; i_r < data.size(); ++i_r) {
                Vertex npos = vid[i_r];
if constexpr (!DIRECTED)
{   if (pos > npos) continue;   }
                if (pos == npos) continue;      // self edge
                Weight_t weight = data[i_c][i_r];
                if (is_inf(weight)) continue;   // edge not exists
                edges.push_back(Edge{pos, npos, weight});
            }
        }
        return edges;
    };
    
    // get a Vertex from storage
    // <Vertex v, bool x> x meaning whether exists
    std::pair<Vertex, bool> _get_a_vertex() const override {
        if (!count_vertices()) return {0, false};
        else return {vid[0], true};
    };
    
    /**
     * // forEach neighbors // all (u -> v)
     * @param u Vertex
     * @param callback function(Vertex, Weight_t);
     */
    void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const override {
        const auto u_id_it = id.find(u);
        if (u_id_it == id.end()) return;
        const ID_t u_id = u_id_it->second;
        for (ID_t i = 0; i < data.size(); ++i) {
            if (is_inf(data[u_id][i]) || (u_id == i)) continue;
            callback(vid[i], data[u_id][i]);
        }
    };
    /**
     * // forEach Vertex // all Vertex in storage
     * @param callback function(Vertex)
     */
    void forEach_vertex(std::function<void(Vertex)> callback) const override {
        for (const Vertex v : vid) {
            callback(v);
        }
    };

    //
    // modifiers

    // insert Vertex v into storage; without inserts any Edges
    void insert_vertex(Vertex v) override {
        if (id.count(v)) return;    // already exists
        const ID_t i = data.size();
        id.insert({v, i});          // start from index 0
        vid.push_back(v);
        const Weight_t inf_w = get_inf();
        for (auto& rows : data) {
            rows.push_back(inf_w);
        }
        data.emplace_back(i+1, inf_w);
        data[i][i] = 0;             // self = 0
    };
    // insert Edge (u, v, (w)) into storage; also add Vertex if it is NOT exists
    void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) override {
        if (u == v) throw std::invalid_argument("(v, v) is illegal");
        // if make it possible, must fix the logic
        insert_vertex(u);    // possible no exists
        insert_vertex(v);    // possible no exists

        const ID_t pos = id.at(u);
        const ID_t npos = id.at(v);
        if (is_inf(data[pos][npos])) ++e;
        data[pos][npos] = w;
if constexpr (!DIRECTED)
{   data[npos][pos] = w;    }
    };
    // insert Edge (u, v, (w)) into storage; also add Vertex if it is NOT exists
    // void insert_edge(Edge e) override { insert_edge(e.u, e.v, e.weight); };

    // delete Vertex and all Edges incident to it
    void delete_vertex(Vertex v) override {
        // find it -> change as last -> pop &&->
        // set [i][i] = INF &&-> update id and vid
        auto it = id.find(v);           // it->second == be d id // old index
        if (it == id.end()) return;     // not exists
        const Vertex lv = vid.back();   // last v
        for (ID_t i = 0; i + 1 < data.size(); ++i) {  // bypass last one
            if (it->second != i) {      // do not count [i][i]
                if (!is_inf(data[it->second][i])) --e;
if constexpr (DIRECTED)
    {           if (!is_inf(data[i][it->second])) --e;  }
            }
            data[it->second][i] = data[data.size() - 1][i];
            data[i][it->second] = data[i][data.size() - 1];
            data[i].pop_back();
        }
        data.pop_back();
        
        // self edge = 0
        if (data.size() > it->second)
            data[it->second][it->second] = 0;

        // id mapping
        // id vid
        vid[it->second] = lv;       // id -> v
        id[lv] = it->second;        // v -> id
        vid.pop_back();
        id.erase(it);
        // #TODO confirm no bug
    };
    // delete Edge (u, v) from the storage
    void delete_edge(Vertex u, Vertex v) override {
        if (u == v) return;
        if ((!id.count(u)) || (!id.count(v))) return;
        // a Vertex not exists => edge impossible exists
        const ID_t pos = id.at(u);
        const ID_t npos = id.at(v);
        if (is_inf(data[pos][npos])) return;    // edge not exists
        data[pos][npos] = get_inf();
if constexpr (!DIRECTED)
    {   data[npos][pos] = get_inf();    }
        --e;
    };
    // delete Edge (u, v) from the storage
    // void delete_edge(Edge e) override { delete_edge(e.u, e.v); };
};




// virtual basic class | for pointer using only
class Graph {
    /**
     * @property a non-empty set of vertices and a set of undirected edges.
     * where each edge is a pair of vertices.
     */
protected:
    IStorage* data_ptr;                 // data
    // size_t n;                        // number of vertices
    // size_t e;                        // number of edges
public:
    virtual ~Graph() {};
    // destructor

    // func | special
    /**
     * @details force type cast (static)
     * @warning ensure type currect
     */
    /*
    template<typename TypeC_Graph>
    static TypeC_Graph& cast(Graph* ptr) {
        return static_cast<TypeC_Graph&>(*ptr);
    };
    */

    // func | helper
    virtual Vertex _get_a_vertex() const = 0;

    virtual bool _is_directed() const = 0;
    virtual bool _is_weight() const = 0;

    //
    // func | getter

    virtual bool is_empty() const = 0;
    // return true if graph has no vertices

    virtual size_t number_of_vertices() const = 0;
    // return number of vertices in the graph

    virtual size_t number_of_edges() const = 0;
    // return number of edges in the graph

    virtual size_t degree(Vertex u) const = 0;
    // return number of edges incident to vertex u

    // return true if graph has the vertex u
    virtual bool exists_vertex(Vertex u) const = 0;

    virtual bool exists_edge(Vertex u, Vertex v) const = 0;
    // return true if graph has the edge (u, v)

    virtual std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const = 0;
    // return { weight, is_exist } from graph by edge(u, v)

    virtual Edge get_edge(Vertex u, Vertex v) const = 0;
    // return Edge{u, v, (w)} from graph

    virtual Edges get_edges() const = 0;
    // return all edges

    virtual std::vector<Vertex> get_NBs(Vertex u) const = 0;
    // return all neighbors of Vertex u in Graph

    virtual void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const = 0;
    // forEach all neighbors

    virtual void forEach_vertex(std::function<void(Vertex)> callback) const = 0;
    // forEach all Vertex

    //
    // modify-type

    virtual void insert_vertex(Vertex v) = 0;
    // insert vertex v into graph; v has no incident edges

    // virtual void insert_edge(Vertex u, Vertex v) = 0;
    // insert edge (u, v) into graph

    virtual void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) = 0;
    // insert edge (u, v) (weight) into graph

    virtual void insert_edge(Edge e) = 0;

    virtual void delete_vertex(Vertex v) = 0;
    // delete v and all edges incident to it

    virtual void delete_edge(Vertex u, Vertex v) = 0;
    // delete edge (u, v) from the graph

/*
    //
    // algorithm

    // get Depth-First Search
    virtual DFS_Result getDFS(Vertex start) const = 0;
    virtual DFS_Result getDFS() const = 0;
    
    // get Breadth-First Search
    virtual BFS_Result getBFS(Vertex start) const = 0;
    virtual BFS_Result getBFS() const = 0;

    
    // you should call this function with the DFS result
    virtual std::vector<std::vector<Vertex>> const getCComponents() const {
        return getDFS().components;
    };
    // you should call this function with the DFS result
    virtual Edges const getSpanningTree() const {
        return getDFS().tree_edges;
    };
*/

    // get Connected Components
    virtual std::vector<std::vector<Vertex>> const getCComponents(const DFS_Result &dfs) const {
        return dfs.components;
    };

    // get Spanning Tree (return a Forest is possible)
    virtual Edges const getSpanningTree(const DFS_Result& dfs) const {
        return dfs.tree_edges;
    };

    // get Biconnected Components
    virtual std::vector<Edges> getBCComponents(const DFS_Result &dfs) const { return dfs.bcc_edges; };

    // get articulation points
    virtual std::unordered_set<Vertex> getArticulationPoints(const DFS_Result& dfs) { return dfs.articulation_points; };
};

template <typename Storage_P>
class BasicGraph : public Graph {
private:
    Storage_P storage;
public:
    // using Storage_t = Storage_P;
    static constexpr bool IS_DIRECTED = Storage_P::is_directed;
    static constexpr bool IS_WEIGHTED = Storage_P::is_weighted;
    //
    // func | helper
    Vertex _get_a_vertex() const { return storage._get_a_vertex(); };
    
    inline bool _is_directed() const { return IS_DIRECTED; };
    inline bool _is_weight() const { return IS_WEIGHTED; };

    //
    // func | getter
    bool is_empty() const override { return storage.is_empty(); };
    size_t number_of_vertices() const override { return storage.number_of_vertices(); };
    size_t number_of_edges() const override { return storage.number_of_edges(); };
    size_t degree(Vertex u) const override { return storage.degree(u); };
    bool exists_vertex(Vertex u) const override { return storage.exists_vertex(u); };
    bool exists_edge(Vertex u, Vertex v) const override { return storage.exists_edge(u, v); };

    
    // return { weight, is_exist } from graph by edge(u, v)
    std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const {
        static constexpr bool IS_W = Storage_P::is_weighted;
        return {storage.get_w(u, v), IS_W};
    };

    // return Edge{u, v, (w)} from graph
    Edge get_edge(Vertex u, Vertex v) const {
        return storage.get_edge(u, v);
    };

    Edges get_edges() const override { return storage.get_edges(); };

    
    // return all neighbors of Vertex u in Graph
    std::vector<Vertex> get_NBs(Vertex u) const { return storage.get_NBs(u); };
    
    // forEach all neighbors in Graph by (u -> v)
    void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const {
        storage.forEach_NBs(u, callback);
    };
    
    // forEach all Vertex in Graph
    void forEach_vertex(std::function<void(Vertex)> callback) const {
        storage.forEach_vertex(callback);
    };

    //
    // modify

    void insert_vertex(Vertex v) override { storage.insert_vertex(v); };
    void insert_edge(Vertex u, Vertex v) override {
        storage.insert_edge(u, v);
    };
    void insert_edge(Vertex u, Vertex v, Weight_t w) override {
        storage.insert_edge(u, v, w);
    };
    void insert_edge(Edge e) override {
        storage.insert_edge(e.u, e.v, e.weight);
    };
    void delete_vertex(Vertex v) override { storage.delete_vertex(v); };
    void delete_edge(Vertex u, Vertex v) override { storage.delete_edge(u, v); };

    //
    // algorithm
    // #TODO move algorithm from Graph class to namespace,
    // it should not be a member function
    // splice getDFS to DFS with a callback to collect data

    // get DFS
    /*
    DFS_Result getDFS(Vertex start) const override {
        return storage.getDFS(start);
    };
    DFS_Result getDFS() const override {
        if (is_empty()) return {};
        return getDFS(storage._get_a_vertex());
        // return getDFS(storage.data.begin()->first);
    };
    
    // get BFS
    BFS_Result getBFS(Vertex start) const override {
        return storage.getBFS(start);
    };
    BFS_Result getBFS() const override {
        if (is_empty()) return {};
        return getBFS(storage._get_a_vertex());
        // return getBFS(storage.data.begin()->first);
    };
    */
    
    inline std::vector<Edges> getBCComponents(const DFS_Result& dfs) const override { return dfs.bcc_edges; };
    inline std::unordered_set<Vertex> getArticulationPoints(const DFS_Result& dfs) {
        return dfs.articulation_points;
    };
};

//
// classes

using DiLinkedGraph = BasicGraph<Storage<Weight::None, Direction::Directed>::Linked>;
using UndiLinkedGraph = BasicGraph<Storage<Weight::None, Direction::Undirected>::Linked>;
using WDiLinkedGraph = BasicGraph<Storage<Weight::Type, Direction::Directed>::Linked>;
using WUndiLinkedGraph = BasicGraph<Storage<Weight::Type, Direction::Undirected>::Linked>;

using WDiMatrixGraph = BasicGraph<Storage<Weight::Type, Direction::Directed>::Matrix>;
using DiMatrixGraph = BasicGraph<Storage<Weight::Type, Direction::Directed>::Matrix>;
using WUndiMatrixGraph = BasicGraph<Storage<Weight::None, Direction::Undirected>::Matrix>;
using UndiMatrixGraph = BasicGraph<Storage<Weight::None, Direction::Undirected>::Matrix>;



//
// algorithm
// #TODO

/**
 * exeDFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(parent, current, next); // (par -> [cur -> next])
 */
template<typename TGraph,
                typename F = FS_callback>
                // ,typename std::enable_if<std::is_base_of_v<Graph, TGraph>, int>::type = 0>
// requires std::derived_from<TGraph, Graph>;
DFS_Result exeDFS(
    const TGraph& graph,
    Vertex start,
    const F& callback = FS_callback{}
    // const std::function<void(Vertex, Vertex, Vertex)>& callback
) {
    static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
    if (graph.is_empty()) return {};

    // if start from NOT exists
    if (!graph.exists_vertex(start))
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
        return {};                               // END
        // or throw Error
#else
        start = graph._get_a_vertex();    // get a RND one
#endif

    static constexpr bool IS_DIR = TGraph::IS_DIRECTED;    // is_directed ?

    DFS_Result res; // save result
    res.components.emplace_back();  // for save [[]]
    std::vector<Vertex>* components_ptr = &res.components.back();
    Order_t counter = 0;
    std::unordered_set<Vertex> on_stack;
    // determine that in a chain from parent, not from other chain
    // stack for SCC ? // std::stack<Vertex> stk;

    // bcc used for undirected Graph
    std::stack<Edge> bcc_stack;

    // res.dfn => visited ? { Vertex : Order_t }


    // rec function
    std::function<void(Vertex, std::optional<Vertex>)> rec  // REC FNC BEGIN ==== ==== ### |
    = [&](Vertex pos, std::optional<Vertex> par) {  // par = std::nullopt   // std::pair<Vertex, bool> par)
        const bool has_parent = par.has_value();
        const Vertex ppos = (has_parent ? par.value() : pos);
        on_stack.insert(pos);                // on stack BEGIN

        // components
        components_ptr->push_back(pos);

        // order
        res.order.push_back(pos);
        res.dfn[pos] = res.low_link[pos] = counter;
        ++counter;
        
        // (u -> v) childrens
        size_t children_counting = 0;
        graph.forEach_NBs(pos, [&](Vertex npos, Weight_t weight) {
if constexpr (!IS_DIR) {
            if (has_parent && (ppos == npos)) return; // not continue, bc it is a Lambda/std::function
}
            // iterate all childrens
            auto const& dfn_npos = res.dfn.find(npos);
            if (dfn_npos == res.dfn.end()) {      // never visited | #### #### | #### #### |

                res.parent[npos] = pos;
                res.children[pos].push_back(npos);
                ++children_counting;        //

                Edge e = graph.get_edge(pos, npos);
                res.tree_edges.push_back(e);            // spanning tree (forest)
if constexpr (!IS_DIR) {
                bcc_stack.push(e);
}
                //
                //                                                // CALL recursive | BEGIN
                callback(ppos, pos, npos);      // call callback with prepos, pos, nextpos
                rec(npos, pos);
                //                                                // END recursive
                //
                // #TODO HERE #TODO
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.low_link[npos]
                );                                      // update low-link

                //
                // # TO DO #HERE undi
                // if never visited, push Edge{u, v}
                // after rec(u,v) (u -> v)
                // if low(v) >= dfn(u) meaning exist bcc
                // start pushing from bcc_stack
                // until edge {u, v}

if constexpr (!IS_DIR) {  // undirected
                // if (u != start && low[v] >= dfn[u])
                // BCC
                if (res.low_link.at(npos) >= res.dfn.at(pos)) {
                    if (has_parent) res.articulation_points.insert(pos);
                    Edges bcc_tmp;
                    while (true) {  // popping
                        Edge edge = bcc_stack.top();
                        bcc_stack.pop();
                        bcc_tmp.push_back(edge);
                        if (
                            (edge.u == pos && edge.v == npos) ||
                            (edge.u == npos && edge.v == pos)
                        ) break;
                    }
                    res.bcc_edges.push_back(bcc_tmp);
                }
                // if (par.has_value() && res.low_link.at(npos) >= res.dfn.at(pos))
                    // res.articulation_points.insert(pos);
}

            } else {                 // been visited | #### #### | #### #### |

if constexpr (IS_DIR) {
            // directed
                if (on_stack.count(npos)) {
                    // AND is in current DFS stack
                    // (pos -> npos) is a back-edge
                    if (res.dfn[npos] < res.dfn[pos]) res.low_link[pos] = std::min(
                        res.low_link[pos],
                        res.dfn[npos]
                    );
                }
} else {
            // undirected
                res.low_link[pos] = std::min(
                    res.low_link[pos],
                    res.dfn[npos]
                );

                if (res.dfn[npos] < res.dfn[pos]) {
                    bcc_stack.push(graph.get_edge(pos, npos));
                }
}

            }   // "is visited" END
        }); // graph.forEach_NBs(pos, [&]() { // END forEach_NBs

if constexpr (!IS_DIR) {
    // root articulation points
        if (!has_parent && children_counting > 1) res.articulation_points.insert(pos);
}
        on_stack.erase(pos);                // END stack
    };                                                   // END REC FNC  ==== ==== ### |


    // exe
    rec(start, std::nullopt);
    graph.forEach_vertex([&](Vertex v) {
        // for isolated => forest
        if (!res.dfn.count(v)) {
            res.components.emplace_back();
            components_ptr = &res.components.back();
            rec(v, std::nullopt);
        }
    });
    return res;
};

// caution | param "parent" in callback hasn't be designed, and high-probability will not
/**
 * exeBFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(0, current, next); // (par -> [cur -> next])
 * parent did not design
 */
template<typename TGraph,
                typename F = FS_callback>
                // ,typename std::enable_if<std::is_base_of_v<Graph, TGraph>, int>::type = 0>
BFS_Result exeBFS(
    const TGraph& graph,
    Vertex start,
    const F& callback = FS_callback{}
) {
    static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
    if (graph.is_empty()) return {};

    // start from NOT exists
    if (!graph.exists_vertex(start))
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
        return {};                               // END
#else
        start = graph._get_a_vertex();    // get a RND one
#endif
    BFS_Result res;
    Order_t counter = 0;
    // res.bfn -> visited

    // in queueing
    std::queue<Vertex> queueing;
    const auto q_push = [&](const Vertex p) {
        queueing.push(p);
        res.order.push_back(p);
        res.bfn[p] = counter;   // is a visited mark same time
        ++counter;
    };

    // queueing.push(start);
    q_push(start);

    while (!queueing.empty()) {
        Vertex pos = queueing.front();
        queueing.pop();
        // res.order.push_back(pos);
        // res.bfn.insert(pos, counter);
        // ++counter;

        graph.forEach_NBs(pos, [&](Vertex npos, Weight_t weight) {
            if (res.bfn.count(npos)) return; // continue; bc of Lambda
            q_push(npos);
            callback(0, pos, npos);
        });
    }

    return res;
};

// disjoint set union
class DSU {
private:
    std::unordered_map<Vertex, Vertex> data;

public:
    // dsu get
    Vertex find(Vertex v) {
        if (!data.count(v))
            return data[v] = v;
        if (data[v] == v)
            return v;
        return data[v] = find(data[v]);
    }

    // a hack method
    bool unite(Vertex u, Vertex v) {
        u = find(u);
        v = find(v);

        if (u == v) return false;

        data[v] = u;
        return true;
    };
    bool unite(Edge e) {
        return unite(e.u, e.v);
    };
};


/**
 * Kruskal’s Algorithm
 * @param graph a Graph ref
 * @returns Edges (vector<Edge>) meaning MST
 */
template<typename TGraph>
Edges getMST_K(const TGraph& graph) {
    static_assert(!TGraph::is_directed, "MST alg do not supports undirected graphs.");
    static_assert(TGraph::is_weighted, "MST alg requires weighted graphs");
    Edges res;
    Edges edges = graph.get_edges();
    std::sort(edges.begin(), edges.end()); // <
    DSU dsu;

    for (const Edge edge : edges) {
        if (dsu.unite(edge))
            res.push_back(edge);
    }

    return res;
};


/**
 * Prim's Algorithm
 * @param graph a Graph ref
 * @param start start from
 * @returns Edges (vector<Edge>) meaning MST
 */
template<typename TGraph>
Edges getMST_P(const TGraph& graph, const Vertex start) {
    static_assert(!TGraph::is_directed, "MST alg do not supports undirected graphs.");
    static_assert(TGraph::is_weighted, "MST alg requires weighted graphs");

    if (!graph.exists_vertex(start)) return {};
    std::unordered_set<Vertex> visited;
    MinHeap<Edge> pHeap;
    Edges res;

    auto pHeap_import = [&](Vertex u) {
        visited.insert(u);
        graph.forEach_NBs(u, [&](Vertex v, Weight_t w){
            if (!visited.count(v)) pHeap.push(Edge{u, v, w});
        });
    };

    pHeap_import(start);

    while (!pHeap.empty()) {
        const Edge e = pHeap.top();
        pHeap.pop();
        if (visited.count(e.v)) continue;
        pHeap_import(e.v);
        res.push_back(e);
    };

    return res;
};


}; // namespace yGraph


#endif // // GRAPH_H // //
