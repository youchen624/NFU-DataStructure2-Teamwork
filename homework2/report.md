# 41343131

# 41343129

作業二

## 解題說明

"作業說明"中展現了很多的結構、圖、程式片段、以及一些sample...
在這份作業中，存在多次修改，大致上程式的設計方式有三個階段(大改):

1. 一開始我使用單純的class繼承方式，單純的繼承方式就如說明中的圖示相同。
2. 但在途中發現了很多地方會重複寫到，我意識到這些重複的地方與儲存結構有關。因此我將Graph結構改成使用struct以及template定義了不同的"是否有權重"的類變數。
3. 再後來我發現我越寫越亂，這樣的結構甚至讓我感到混亂，我重新思考後，我將**儲存**的部分做成單一抽象類IStorage(透過指標儲存於Graph基類，並且被Linked_STG,Matrix_STG繼承)，並透過模板template在BasicGraph(繼承基類Graph)中使用，最後搭配using定義這些不同模板的條件組合而成的所有類(題目出現的)。

## 程式實作

### Edge Class

在Edge邊中，我們設計固定都儲存兩個頂點與一個權重:

```cpp
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
```

- `operator bool() const` - 用於if判斷邊是否存在，只要是self-edge就是不存在
  - 其中 `explicit` 語法比較特殊，它用於禁止編譯器擅自作主，將它用在其他地方，這邊僅允許用於布林判斷
- `operator<(const Edge& that) const`/`operator>(const Edge& that) const` - 用於比較邊，單純比較權重，用於排序

### Storage

在Matrix中，前面提到透過 `std::vector<std::vector<Weight_t>>` 儲存，理由是節省成本(時間與空間上較均衡的選項)，因應根據作業說明所示，加入的頂點很可能不連續，所以這邊還需要一個Bimap交換index，透過 `std::vector` (index->Vertex)以及 `std::unordered_map` (Vertex->index)，有點像是ID的運作方式，詳細的過程請詳見程式或示意圖。
這是Matrix_STG的運作方式示意圖(大致)：

![圖片](../.img/DS2-HW2-Matrix_storage.png)

在Matrix中，刪除一個頂點，同時顧及vector特性，我們先將最後一項複製到待刪除的位置，隨後將最後一項刪除(`vector::pop_back()`)，同時修改自身邊及id/value map。

![圖片](../.img/DS2-HW2-Matrix_deleting_Vertex.png)

### Graphs Classes

我們使用

```cpp
template<template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED>
class BasicGraph : public Graph;
```

其中

- `Graph` - 基類有 `IStorage* data_ptr` protected成員，並在BasicGraph中維護(new, delete)。
- `STG` - 是儲存結構，使用繼承IStorage的類即可，同時支援其他類型(透過撰寫繼承IStorage的類，支援不僅侷限於Linked/Matrix)
- `DIRECTED`/`WEIGHTED` - 是否有向/是否有權，字面上，不多做說明。

並透過

```cpp
using UndiLinkedGraph   = BasicGraph<Linked_STG, false, false>;
using WUndiLinkedGraph  = BasicGraph<Linked_STG, false, true>;
using WDiLinkedGraph    = BasicGraph<Linked_STG, true,  true>;
using DiLinkedGraph     = BasicGraph<Linked_STG, true,  false>;

using UndiMatrixGraph   = BasicGraph<Matrix_STG, false, false>;
using WUndiMatrixGraph  = BasicGraph<Matrix_STG, false, true>;
using WDiMatrixGraph    = BasicGraph<Matrix_STG, true,  true>;
using DiMatrixGraph     = BasicGraph<Matrix_STG, true,  false>;
```

進行繼承與定義

其中使用到template搭配 `if constexpr` ，這個部分會在編譯時期自動完成，也就是實際運作時它不需要重新判斷(即不需要在每次執行階段判斷)。

### Graphs Algorithms Functions

原先我將演算法的方法都寫於Graphs內，但我總覺得哪裡怪怪的，後來才發現應該將這些算法置於Graph類外部較為合理(因為他不是"圖"類的一部份)。在這過程中也是一次中等程度的全部修改(從直接存取data到透過public functions去做存取)，同時也新增了一些 `forEach_` 方法：

- `forEach_NBs(Vertex u, callback)` - 遍歷鄰居(u->v)，其中NB是neighbors的縮寫，會針對圖中**所有**從頂點u出發的鄰居邊跑過一次，過程無保證順序。
  - `Vertex` - 出發的頂點
  - `callback(Vertex, Weight_t, bool&)` - callback(std::function或Lambda)，如果是無權圖則Weight_t會是預設0。bool&用於提前終止遍歷。
- `forEach_vertex(callback)` - 便利頂點(v)，會針對圖中**所有**的頂點跑過一次，無保證順序。
  - `callback(Vertex)` - callback(std::function或Lambda)。
- `forEach_edge(callback)` - 遍歷邊(e)，針對途中**所有**邊都會跑過一次，不保證順序。
  - `callback(Edge, bool&)` - callback(std::function或Lambda)，Edge是邊類(結構)。bool&用於提前終止遍歷。

在設計 `DFS`/`BFS` 時，我設計了一個很有意思的define定義 - `ALLOW_FS_START_FROM_NOT_EXISTS`，字面上的意思，只要定義就允許這兩個演算法的初始頂點不存在圖，當然如果不存在會隨機找一個做為起點。

#### DFS/BFS Algorithms

- `exeDFS(Graph&, Vertex, callback)` - Depth First Search，顧名思義，它會盡可能地深入，這邊透過dfn(`std::unordered_map<Vertex, Order_t>`)判斷是否已經造訪過(同時作為紀錄第幾個造訪的)，這是多次更改後的最終版，簡單來說，這邊使用Lambda遞迴的方式自動製造stack，不斷探索直到完畢。
  - `Graph&` - 圖，這邊引用參考，不會複製整個圖;
  - `Vertex` - 起點。若`ALLOW_FS_START_FROM_NOT_EXISTS`有被定義，則允許在起點不存在時自動找一個起點
  - `callback(Vertex, Vertex, Vertex)` - 三個頂點分別為(parent, current, neighbors)

其中 `std::optional` 是可選的一種std類，因為開始的時候沒有parent，所以我使用這個類來判斷。

```cpp
/**
 * exeDFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(parent, current, next); // (par -> [cur -> next])
 */
template<
    template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED,
    typename F = FS_callback
>
DFS_Result exeDFS(
    const BasicGraph<STG, DIRECTED, WEIGHTED>& graph,
    Vertex start,
    const F& callback = FS_callback{}
    // const std::function<void(Vertex, Vertex, Vertex)>& callback
) {
    // static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
    if (graph.is_empty()) return {};

    // if start from NOT exists
    if (!graph.exists_vertex(start))
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
        return {};                               // END
        // or throw Error
#else
        start = graph._get_a_vertex();    // get a RND one
#endif

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
if constexpr (!DIRECTED) {
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
if constexpr (!DIRECTED) {
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

if constexpr (!DIRECTED) {  // undirected
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

if constexpr (DIRECTED) {
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

if constexpr (!DIRECTED) {
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
```

- `exeBFS`
  - `Graph&` - 圖，這邊引用參考，不會複製整個圖;
  - `Vertex` - 起點。若`ALLOW_FS_START_FROM_NOT_EXISTS`有被定義，則允許在起點不存在時自動找一個起點
  - `callback(Vertex, Vertex, Vertex)` - 相較於DFS，BFS我沒有設計neighbors，三個頂點分別為(parent, current, 0)

```cpp
/**
 * exeBFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(0, current, next); // (par -> [cur -> next])
 * parent did NOT design
 */
template<
    template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED,
    typename F = FS_callback
>
BFS_Result exeBFS(
    const BasicGraph<STG, DIRECTED, WEIGHTED>& graph,
    Vertex start,
    const F& callback = FS_callback{}
) {
    // static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
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

```

為了方便存儲資料(畢竟處理過程就能產生)，因此設計了這兩個演算法專用的result類:

```cpp
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
} BFS_Result;
```

#### MST 最小生成樹

這邊有兩種演算法，分別為 `Kruskal’s Algorithm` 和 `Prim's Algorithm`

為了方便設計Kruskal’s Algorithm，所以我們先設計了併查集DSU類([參考hackmd.io](<https://hackmd.io/@fdhscpp110/DSU_MST>))，簡單來說，這個演算法會不斷將編的鄰居頂點加入至同個集合或合併集合(只要不是同一個集合即可(沒有環出現)):
"#TODO HERE"

```cpp
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
```

- Kruskal’s Algorithm

```cpp
/**
 * Kruskal’s Algorithm
 * @param graph a Graph ref
 * @returns Edges (vector<Edge>) meaning MST
 */
template<template<bool, bool> class STG>
Edges getMST_K(const BasicGraph<STG, false, true>& graph) {
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

```

- Prim's Algorithm

```cpp
/**
 * Prim's Algorithm
 * @param graph a Graph ref
 * @param start start from
 * @returns Edges (vector<Edge>) meaning MST
 */
template<template<bool, bool> class STG>
Edges getMST_P(const BasicGraph<STG, false, true>& graph, const Vertex start) {

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
```

### Graph.h

-# [跳過](#效能分析)

```h
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
using VPath = std::vector<Vertex>;
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
    // how many of edges incident to vertex u
    virtual size_t count_degree(Vertex u) const = 0;

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

    // get Weight by Edge(u, v)
    // <Weight_t w, bool x> x meaning whether exists
    virtual std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const = 0;
    
    /**
     * // forEach neighbors // all (u -> v)
     * @param u Vertex
     * @param callback function(Vertex, Weight_t, bool&);
     * - set bool& to true to stop the iteration early
     */
    virtual void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const = 0;
    /**
     * // forEach Vertex // all Vertex in storage
     * @param callback function(Vertex, bool&)
     * - set bool& to true to stop the iteration early
     */
    virtual void forEach_vertex(std::function<void(Vertex)> callback) const = 0;
    /**
     * // forEach Edge // all Edge in storage
     * @param callback function(Edge, bool&);
     * - set bool& to true to stop the iteration early
     */
    virtual void forEach_edge(std::function<void(Edge)> callback) const = 0;

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
    size_t count_degree(Vertex u) const {
        auto the = data.find(u);
        if (the == data.end()) return 0;
if constexpr (DIRECTED) {       // IF
        size_t t = the->second.size();
        for (const auto& pair : data) {
            t += pair.second.count(u); // set::contains // C20
        }
        return t;
} else {                              // ELSE
        return the->second.size();
}
    };

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
    std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const override {
        return {_get_w(u, v), WEIGHTED};
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
    void forEach_edge(std::function<void(Edge)> callback) const override {
        for (const auto& [u, um] : data) {
if constexpr (WEIGHTED)
            // data{ u<Vertex>: um<{ v<Vertex>, w<Weight_t>... }>... }
            for (const auto& [v, w] : um) {
                callback(Edge{u, v, w});
            };
else
            // data{ u<Vertex>: um<{ v<Vertex>... }>... }
            for (const Vertex v : um) {
                callback(Edge{u, v});
            };
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
    // how many of edges incident to vertex u
    size_t count_degree(Vertex u) const override {
        auto const it = id.find(u);     // [v : id]
        if (it == id.end()) return 0; // Vertex not exist => 0 degree
        size_t count = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            if (i == it->second) ++i;   // bypass self
            if (!is_inf(data[i][it->second])) ++count;
if constexpr (DIRECTED)
    {   if (!is_inf(data[it->second][i])) ++count;  }
        }
        return count;
    };

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
    std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const override {
        return {_get_w(u, v), WEIGHTED};
    };
    
    /**
     * // forEach neighbors // all (u -> v)
     * @param u Vertex
     * @param callback function(Vertex, Weight_t, bool&);
     * - set bool& to true to stop the interation early
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
     * @param callback function(Vertex, bool&)
     * - set bool& to true to stop the interation early
     */
    void forEach_vertex(std::function<void(Vertex)> callback) const override {
        for (const Vertex v : vid) {
            callback(v);
        }
    };
    void forEach_edge(std::function<void(Edge)> callback) const override {
        for(ID_t ui = 0; ui < count_vertices(); ++ui) {
            for(ID_t vi = 0; vi < count_vertices(); ++vi) {
                if (is_inf(data[ui][vi]) || (ui == vi)) continue;
                callback(Edge{
                    vid[ui],
                    vid[vi],
                    data[ui][vi]
                });
            }
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
    virtual std::pair<Vertex, bool> _get_a_vertex() const = 0;

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

    virtual void forEach_edge(std::function<void(Edge)> callback) const = 0;

    //
    // modify-type

    virtual void insert_vertex(Vertex v) = 0;
    // insert vertex v into graph; v has no incident edges

    // virtual void insert_edge(Vertex u, Vertex v) = 0;
    // insert edge (u, v) into graph

    virtual void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) = 0;
    // insert edge (u, v) (weight) into graph

    virtual void insert_edge(Edge e) { insert_edge(e.u, e.v, e.weight);};

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


template<template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED>
class BasicGraph : public Graph {
protected:
    // IStorage* data_ptr;                 // data
public:
    // constructor
    BasicGraph() {
        data_ptr = new STG<DIRECTED, WEIGHTED>();
    };
    // destructor
    ~BasicGraph() {
        delete data_ptr;
    };

    // func | helper
    std::pair<Vertex, bool> _get_a_vertex() const override { return data_ptr->_get_a_vertex(); };

    bool _is_directed() const override { return DIRECTED; };
    bool _is_weight() const override { return WEIGHTED; };

    //
    // func | getter

    bool is_empty() const override { return !data_ptr->count_vertices(); };
    // return true if graph has no vertices

    size_t number_of_vertices() const override { return data_ptr->count_vertices(); };
    // return number of vertices in the graph

    size_t number_of_edges() const override { return data_ptr->count_edges(); };
    // return number of edges in the graph

    size_t degree(Vertex u) const override { return data_ptr->count_degree(); };
    // return number of edges incident to vertex u

    // return true if graph has the vertex u
    bool exists_vertex(Vertex u) const override { return data_ptr->exists_vertex(u); };

    bool exists_edge(Vertex u, Vertex v) const override { return data_ptr->exists_edge(u, v); };
    // return true if graph has the edge (u, v)

    std::pair<Weight_t, bool> get_weight(Vertex u, Vertex v) const override { return data_ptr->get_weight(u, v); };
    // return { weight, is_exist } from graph by edge(u, v)

    Edge get_edge(Vertex u, Vertex v) const override { return data_ptr->get_edge(u, v); };
    // return Edge{u, v, (w)} from graph

    Edges get_edges() const override { return data_ptr->get_edges(); };
    // return all edges

    std::vector<Vertex> get_NBs(Vertex u) const override {
        if (!data_ptr->exists_vertex(u)) return {};
        std::vector<Vertex> res;
        res.reserve(data_ptr->count_vertices());
        data_ptr->forEach_NBs(u, [&](Vertex v, Weight_t w){
            res.push_back(get_edge(u, v, w));
        });
        return res;
    };
    // return all neighbors of Vertex u in Graph

    void forEach_NBs(Vertex u, std::function<void(Vertex, Weight_t)> callback) const override { data_ptr->forEach_NBs(u, callback); };
    // forEach all neighbors

    void forEach_vertex(std::function<void(Vertex)> callback) const override { data_ptr->forEach_vertex(callback); };
    // forEach all Vertex

    void forEach_edge(std::function<void(Edge)> callback) const override { data_ptr->forEach_edge(callback); };

    //
    // modify-type

    void insert_vertex(Vertex v) override { data_ptr->insert_vertex(v); };
    // insert vertex v into graph; v has no incident edges

    // void insert_edge(Vertex u, Vertex v) override {};
    // insert edge (u, v) into graph

    // to fix C/C++ bug which not follows STD
    using Graph::insert_edge;

    void insert_edge(Vertex u, Vertex v, Weight_t w = Weight_t{}) override { data_ptr->insert_edge(u, v, w); };
    // insert edge (u, v) (weight) into graph

    // void insert_edge(Edge e) override {};

    void delete_vertex(Vertex v) override { data_ptr->delete_vertex(v); };
    // delete v and all edges incident to it

    void delete_edge(Vertex u, Vertex v) override { data_ptr->delete_edge(u, v); };
    // delete edge (u, v) from the graph
};


//
// classes definations

using UndiLinkedGraph   = BasicGraph<Linked_STG, false, false>;
using WUndiLinkedGraph  = BasicGraph<Linked_STG, false, true>;
using WDiLinkedGraph    = BasicGraph<Linked_STG, true,  true>;
using DiLinkedGraph     = BasicGraph<Linked_STG, true,  false>;

using UndiMatrixGraph   = BasicGraph<Matrix_STG, false, false>;
using WUndiMatrixGraph  = BasicGraph<Matrix_STG, false, true>;
using WDiMatrixGraph    = BasicGraph<Matrix_STG, true,  true>;
using DiMatrixGraph     = BasicGraph<Matrix_STG, true,  false>;



//
// algorithm
/**
 * exeDFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(parent, current, next); // (par -> [cur -> next])
 */
template<
    template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED,
    typename F = FS_callback
>
DFS_Result exeDFS(
    const BasicGraph<STG, DIRECTED, WEIGHTED>& graph,
    Vertex start,
    const F& callback = FS_callback{}
    // const std::function<void(Vertex, Vertex, Vertex)>& callback
) {
    // static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
    if (graph.is_empty()) return {};

    // if start from NOT exists
    if (!graph.exists_vertex(start))
#ifndef ALLOW_FS_START_FROM_NOT_EXISTS
        return {};                               // END
        // or throw Error
#else
        start = graph._get_a_vertex();    // get a RND one
#endif

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
if constexpr (!DIRECTED) {
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
if constexpr (!DIRECTED) {
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

if constexpr (!DIRECTED) {  // undirected
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

if constexpr (DIRECTED) {
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

if constexpr (!DIRECTED) {
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


// caution | param "parent" in callback has NOT be designed,
// and high-probability will not
/**
 * exeBFS
 * @param graph a Graph ref
 * @param start start from
 * @param callback a function
 * void callback(0, current, next); // (par -> [cur -> next])
 * parent did NOT design
 */
template<
    template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED,
    typename F = FS_callback
>
BFS_Result exeBFS(
    const BasicGraph<STG, DIRECTED, WEIGHTED>& graph,
    Vertex start,
    const F& callback = FS_callback{}
) {
    // static_assert(std::is_base_of_v<Graph, TGraph>, "TGraph must inherit from Graph");
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
template<template<bool, bool> class STG>
Edges getMST_K(const BasicGraph<STG, false, true>& graph) {
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
template<template<bool, bool> class STG>
Edges getMST_P(const BasicGraph<STG, false, true>& graph, const Vertex start) {

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



//
// algorithm | includes Paths

struct PathNode {
    Vertex u;
    Weight_t dis;
    bool operator>(const PathNode& that) const { return dis > that.dis; };
    bool operator<(const PathNode& that) const { return dis < that.dis; };
};


struct PathResult {
    VPath path;
    Weight_t total_dis = 0;
};

/**
 * @name ShortestPath_DataHolder = SP_DHolder
 * @details to store SP data, and provide a SP-getting method by (u, v)
 */
typedef class ShortestPath_DataHolder {
template<template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED>
friend SP_DHolder getSSSP_D(const BasicGraph<STG, DIRECTED, WEIGHTED>& graph, const Vertex start);
template<template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED>
friend SP_DHolder getSSSP_BF(const BasicGraph<STG, DIRECTED, WEIGHTED>& graph, const Vertex start);
private:
    // {start: { target: < parent, distance > }}
    std::unordered_map<Vertex, std::unordered_map<Vertex, PathNode>> data;
    bool has_NegativeCycle = false;

protected:
    // (org->to) CMP Edge(from->to)(w)
    // returns true if did a relaxation
    bool relax(const Vertex start, const Edge e) {
        auto& dataref = data[start];
        if ((dataref.count(e.u)) && ( // "from"(e.u) must exists
            // "to" never visited meaning INF-dis
            (!dataref.count(e.v)) ||
            // (from->to).dis < (org->to).dis // (from->to).dis == (from.dis + w)
            dataref[dataref[e.u].u].dis + e.weight < dataref[e.v].dis
        )) {
            dataref[e.v] = {e.u, dataref[dataref[e.u].u].dis + e.weight};
            return true;
        } else return false;
    };

public:
    PathResult getSP(Vertex start, Vertex target) const {
        if (!data.count(start) || !data.at(start).count(target)) return {};  // NOT found.
        if (has_NegativeCycle) throw "NegCycle exists"; // #TODO
        PathResult res;

        res.total_dis = data.at(start).at(target).dis;
        while (start != target) {
            res.path.push_back(target);
            target = data.at(start).at(target).u;
        };

        std::reverse(res.path.begin(), res.path.end());
        // ^ O(n) | can be optimization,
        // by using data{start: { target: < parent, distance, **path_count** > }}
        // + array::reserve + for(--) reversing pushing
        // to count how many nodes have been visited before the current path-node
        // anyway, O(n) is acceptable, so... LAZY~ YA!!

        return res;
    };
} SP_DHolder;


/**
 * Dijkstra's algorithm
 * @warning negative-Weight NOT supports !!!
 * @param graph a Graph ref
 * @param start start from
 */
template<template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED>
// SSSP - Single-Source Shortest Path // 單源最短路徑
SP_DHolder getSSSP_D(const BasicGraph<STG, DIRECTED, WEIGHTED>& graph, const Vertex start) {
    if (!graph.exists_vertex(start)) return {};

    SP_DHolder res;
    auto& data = res.data[start];   // { target: < parent, distance > }
    MinHeap<PathNode> pHeap;

    data[start] = {start, 0};
    pHeap.push({start, 0});

    auto relaxation = [&](PathNode p) {
        if (p.dis != data[p.u].dis) return;
        graph.forEach_NBs(p.u, [&](Vertex v, Weight_t w){
            // EXCEPTION #TODO should throw a clearly class
            if (w < 0) throw ("negative-Weight not support");
            if (res.relax(start, {p.u, v, w}))
                pHeap.push({v, data[v].dis});
            /*
            if ((!data.count(v)) || p.dis + w < data[v].dis) {
                data[v] = {p.u, p.dis + w};
                pHeap.push({v, data[v].dis});
            }
            */
        });
    };

    while(!pHeap.empty()) {
        PathNode pn = pHeap.top();
        pHeap.pop();

        relaxation(pn);
    };

    return res;
};



/**
 * Bellman-Ford's algorithm
 * @details allowed Neg-Weight
 * @param graph a Graph ref
 * @param start start from
 */
template<
    template<bool, bool> class STG, bool DIRECTED, bool WEIGHTED
    //, typename F = FS_callback
>   // SSSP - Single-Source Shortest Path (allowed Neg-Weight) // 單源最短路徑(允許負權)
SP_DHolder getSSSP_BF(const BasicGraph<STG, DIRECTED, WEIGHTED>& graph, const Vertex start) {
    if (!graph.exists_vertex(start)) return {};

    SP_DHolder res;
    auto& data = res.data[start];
    data[start] = {start, 0};

    for (size_t i = 0; i < graph.number_of_vertices() - 1; ++i) {
        graph.forEach_edge([&](const Edge e){   // u->v (w)
            res.relax(start, e);
        });
    };

    // check neg-cycle
    graph.forEach_edge([&](const Edge e){
        if (res.relax(start, e)) {
            res.has_NegativeCycle = true;
            return res;
        };
    });

    return res;
};




}; // namespace yGraph


#endif
// // GRAPH_H // //

```

-# [回去](#graphh)

## 效能分析

在LinkedGraph中，我們使用 `std::unordered_map` 與 `std::unordered_set` 作為底層資料結構的儲存類型。根據數學定義，圖的邊與頂點沒有順序可言(無順序問題)。
在有無權中，分別使用:

- 帶有權重 `std::unordered_map<Vertex, std::unordered_map<Vertex, Weight_t>>`
- 沒有權重 `std::unordered_map<Vertex, std::unordered_set<Vertex>>`

在Matrix中，我們使用 `std::vector<std::vector>` 作為Matrix儲存，並在(u-u)自身邊設為0，對於不存在的邊使用無限Infinite(INF)定義。

理想狀態下（雜湊函數均勻分佈且無劇烈碰撞），底層透過 Key-Value 進行查找與存取的時間複雜度皆為 $O(1)$。各項核心操作的時間複雜度如下表所示：

- Linked Graph

| 操作 (Operations) | 平均時間複雜度 | 說明 |
| :--- | :--- | :--- |
| `is_empty()` | $O(1)$ | 檢查Graph是否為空(頂點數為零) |
| `number_of_vertices()` | $O(1)$ | 取得Graph的頂點數量 |
| `number_of_edges()` | $O(1)$ | 取得Graph的邊數量 |
| `degree(u)` | $O(v)$ | 取得頂點 u 的所有出入度數量，其中 v 為出入度總量 |
| `exists_vertex(u)` | $O(1)$ | 查詢頂點 u 是否存在 |
| `exists_edge(u, v)` | $O(1)$ | 查詢邊 (u, v) 是否存在 |
| `get_edges()` | $O(n*v)$ | 取得Graph所有邊 |
| `get_NBs(u)` | $O(v)$ | 取得頂點 u 的所有鄰居 |
| `forEach_NBs(u, callback(v, w))` | $O(v)$ | 遍歷所有鄰居(u->v) |
| `forEach_vertex(callback(v))` | $O(n)$ | 遍歷所有頂點 v |
| | | |
| `insert_vertex(u)` | $O(1)$ | 於雜湊表中建立頂點與其鄰接容器。 |
| `insert_edge(u, v)` | $O(1)$ | 定位頂點後，直接寫入鄰接結構(如儲存權重或記錄連通)。 |
| `delete_vertex(u)` | $O(v)$ | 刪除頂點及所有與其相關的邊，其中 v 為有關聯的對 |
| `delete_edge(u, v)` | $O(1)$ | 藉由雜湊直接定位特定邊並將其抹除。 |
| | | |
| `_get_a_vertex()` | $O(1)$ | 取得一個頂點 |
| `_is_directed()` | $O(1)$ | 是否為有向圖 |
| `_is_weighted()` | $O(1)$ | 是否為有權圖 |

---

- Matrix Graph

| 操作 (Operations) | 平均時間複雜度 | 說明 |
| :--- | :--- | :--- |
| `is_empty()` | $O(1)$ | 檢查Graph是否為空(頂點數為零) |
| `number_of_vertices()` | $O(1)$ | 取得Graph的頂點數量 |
| `number_of_edges()` | $O(1)$ | 取得Graph的邊數量 |
| `degree(u)` | $O(n)$ | 取得頂點 u 的所有出入度數量 |
| `exists_vertex(u)` | $O(1)$ | 查詢頂點 u 是否存在 |
| `exists_edge(u, v)` | $O(1)$ | 查詢邊 (u, v) 是否存在 |
| `get_edges()` | $O(n^2)$ | 取得Graph所有邊 |
| `get_NBs(u)` | $O(n)$ | 取得頂點 u 的所有鄰居 |
| `forEach_NBs(u, callback(v, w))` | $O(n)$ | 遍歷所有鄰居(u->v) |
| `forEach_vertex(callback(v))` | $O(n)$ | 遍歷所有頂點 v |
| | | |
| `insert_vertex(u)` | $O(n)$ | 插入頂點 |
| `insert_edge(u, v)` | $O(1)$ | 插入邊(自動插入頂點，此自動操作會使複雜度提高為$O(n)$) |
| `delete_vertex(u)` | $O(n)$ | 刪除頂點及所有與其相關的邊 |
| `delete_edge(u, v)` | $O(1)$ | 刪除邊(u, v) |
| | | |
| `_get_a_vertex()` | $O(1)$ | 取得一個頂點 |
| `_is_directed()` | $O(1)$ | 是否為有向圖 |
| `_is_weighted()` | $O(1)$ | 是否為有權圖 |

---

- Algorithms

| 操作 (Operations) | 平均時間複雜度 | 說明 |
| :--- | :--- | :--- |
| `exeDFS(graph, start, callback(p, u, v))` | $O()$ | |
| `exeBFS(graph, start, callback(0, u, v))` | $O()$ | |
| `getMST_K(graph)` | $O(n^2)$ (Matrix) $O(n log(n))$ (Linked) | 透過Kruskal’s演算法取得最小生成樹 |
| `getMST_P(graph, start)` | $O()$ | 透過Prim’s演算法取得最小生成樹 |
| `` | $O()$ | |

## 測試與驗證

為了驗證圖形 ADT 運算以及走訪演算法（特別是 Tarjan 雙連通分量演算法與關節點計算）的正確性，我們設計了三種不同結構的圖形進行測試。

### 1. 測試案例一：無向圖形架構 (Undirected Graph)

這個圖形主要用來測試無向圖中的橋（Bridge）與關節點（Articulation Point）。頂點 4 連接了上方的環狀結構與下方的孤立點 5，因此在拓撲結構上屬於關鍵節點。

* **頂點集合 (V)：** $\{1, 2, 3, 4, 5\}$
* **邊集合 (E)：** $\{(1, 2), (2, 3), (3, 4), (4, 1), (4, 5)\}$
* **結構特性：** 頂點 4 為關節點；移除邊 $(4, 5)$ 會使圖形不連通，故 $(4, 5)$ 為橋。

### 2. 測試案例二：有向圖形架構 (Directed Graph)

這個圖形主要用來驗證有向圖下的 DFS 走訪順序與強連通分量（SCC）。其中的 1、2、3、4 四個頂點構成了一個順時針的有向環（Directed Cycle），而頂點 5 則是只能單向到達的終端節點。

* **頂點集合 (V)：** $\{1, 2, 3, 4, 5\}$
* **邊集合 (E)：** $\{(1, 2), (2, 3), (3, 4), (4, 1), (4, 5)\}$
* **結構特性：** 子圖 $\{1, 2, 3, 4\}$ 互為可達，構成一個強連通分量；頂點 5 出度（Out-degree）為 0。

---

## 效能量測

請參見 [效能分析](#效能分析)

---

## 心得討論

在設計過程中，我更換過最一開始的設計理念，將資料部分分離製作成 `storage`。但後面越做越偏，最後才注意到似乎將演算法等都塞進了 `storage` 之中，這是個未來待修正的問題。

在透過大量 `template` 的實作方法中，其實遇到了不少問題，途中也修改了多次。為此還上網找尋了很多資料，其中最有印象的是非常特別的 `std::conditional_t`，它竟然允許透過一個 `bool` 值來在編譯期決定資料類型，這其實蠻有趣的。

未來的重構方向上，我應該將 `Storage` 的工作徹底分離，而不再模糊於「資料存取」與「演算法」之間的界線。

---

## 申論及開發報告

為了解決「能夠使用基於父類別的 `Graph*` 指標來統一存取與操作子類別成員」這項物件導向需求，同時又想兼顧 Template 帶來的編譯期優化，本系統採用了如下的設計架構：

```text
+---------------------------------------+
|            Graph (抽象介面)            |
+---------------------------------------+
                    ▲
                    | (虛擬繼承 / 運行期多型)
+---------------------------------------+
|    BasicGraph<StoragePolicy, ...>     |
+---------------------------------------+
                    │
                    ▼ (內部組合 / 編譯期裝配)
+---------------------------------------+
|  Storage Component (Matrix / List)    |
+---------------------------------------+




