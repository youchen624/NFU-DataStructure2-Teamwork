#include <iostream>
#include <string>
#include "Graph.h"
using namespace yGraph;

// use for std::cout
void show_dfn_res(const DFS_Result& dfs, std::string str ="DFS-result") {
    
    std::cout << ">" << str << std::endl;
    std::cout << "[order]: [";// << std::endl;
    for (auto const& v : dfs.order) {
        std::cout << v << ", ";
    } std::cout << "]" << std::endl; //std::cout << std::endl;

    // std::cout << "[CC]:" << std::endl;
    // for (auto const& vv : dfs.components) {
    //     std::cout << "[ ";
    //     for (auto const& v : vv) {
    //         std::cout << v << ", ";
    //     } std::cout << "]" << std::endl;
    // } std::cout << std::endl << std::endl;


    std::cout << "[articulation_points]: " << "[ ";//<< std::endl 
    for (auto const& v : dfs.articulation_points) {
        std::cout << v << ", ";
    } std::cout << " ]" << std::endl;
    
    std::cout << "[BCC]: [" << std::endl;
    for (auto const& ve : dfs.bcc_edges) {
        std::cout << "  [ ";
        for (auto const& e : ve) {
            std::cout << "(" << e.u << ", " << e.v << "), ";
        } std::cout << "]" << std::endl;
    } std::cout << "]" << std::endl;// << std::endl;
    
    
    std::cout << "[Spanning Trees]: [" << std::endl;
    for (auto const e : dfs.tree_edges) {
        std::cout << "(" << e.u << ", " << e.v << ")" << std::endl;
    } std::cout << "]" << std::endl;

    std::cout << "<" << "== END == <<<" << std::endl << std::endl;
};

// some LinkedGraph testing
void test_linked() {
    UndiLinkedGraph test = UndiLinkedGraph();
    test.insert_edge(1,2);
    test.insert_edge(2,3);
    test.insert_edge(3,4);
    test.insert_edge(4,1);
    test.insert_edge(4,5);
    /*

        1 - 2
        |     |
        4 - 3
        |
        5

    */
    DFS_Result dfs = exeDFS(test, 1);
    //test.getDFS(1);
    show_dfn_res(dfs);


    //
    // //
    //


    DiLinkedGraph test_1 = DiLinkedGraph();
    test_1.insert_edge(1,2);
    test_1.insert_edge(2,3);
    // test_1.insert_edge(2,6);
    test_1.insert_edge(3,4);
    test_1.insert_edge(4,1);
    test_1.insert_edge(4,5);
    /*

        1 . 2
        ↑      ↓
        4 <- 3
        ↓
        5

    */
    DFS_Result dfs_1 = exeDFS(test_1, 1);
    //test_1.getDFS(1);
    show_dfn_res(dfs_1);
    DFS_Result dfs_1p = exeDFS(test_1, 4);
    //test_1.getDFS(1);
    show_dfn_res(dfs_1p, "from 4");


    //
    // //
    //

    
    UndiLinkedGraph test_2 = UndiLinkedGraph();
    const Edge edge_ar_2[] = {
        {1,  2}, {2,  3}, {2,  6}, {3,  4},
        {4,  1}, {6,  7}, {7,  8}, {8,  9},
        {8, 13}, {9, 10}, {10,11}, {11,12},
        {12, 5}, {13,14}, {14,15}, {15,16},
        {16,13},
    };
    for (auto const& e : edge_ar_2) {
        test_2.insert_edge(e);
    }
    /*


        1 - 2 - 6 - 7 - 8 - 13 - 14
        |   |           |    |    |
        4 - 3           9   16 - 15
                        |
        5  -  12 - 11 - 10


    */
    DFS_Result dfs_2 = exeDFS(test_2, 1);
    //test_2.getDFS(1);
    show_dfn_res(dfs_2);

    //
    // //
    //

    ;

    // return 0;
};

// some MatrixGraph testing
void test_matrix() {
    UndiMatrixGraph test = UndiMatrixGraph();
    test.insert_edge(1,2);
    test.insert_edge(2,3);
    test.insert_edge(3,4);
    test.insert_edge(4,1);
    test.insert_edge(4,5);
    /*

        1 - 2
        |     |
        4 - 3
        |
        5

    */
    DFS_Result dfs = exeDFS(test, 1);
    //test.getDFS(1);
    show_dfn_res(dfs);


    //
    // //
    //


    DiMatrixGraph test_1 = DiMatrixGraph();
    test_1.insert_edge(1,2);
    test_1.insert_edge(2,3);
    // test_1.insert_edge(2,6);
    test_1.insert_edge(3,4);
    test_1.insert_edge(4,1);
    test_1.insert_edge(4,5);
    /*

        1 . 2
        ↑      ↓
        4 <- 3
        ↓
        5

    */
    DFS_Result dfs_1 = exeDFS(test_1, 1);
    //test_1.getDFS(1);
    show_dfn_res(dfs_1);

    DFS_Result dfs_1p = exeDFS(test_1, 4);
    //test_1.getDFS(1);
    show_dfn_res(dfs_1p, "from 4");


    //
    // //
    //

    
    UndiMatrixGraph test_2 = UndiMatrixGraph();
    const Edge edge_ar_2[] = {
        {1,2},   {2,3},    {2, 6},     {3, 4},
        {4,1},   {6,7},    {7,8},      {8,9},
        {8,13}, {9,10},  {10,11},  {11,12},
        {12,5}, {13,14}, {14,15}, {15,16},
        {16,13},
    };
    for (auto const& e : edge_ar_2) {
        test_2.insert_edge(e);
    }
    /*

        1 - 2 - 6 - 7 - 8 - 13 - 14
        |   |           |    |    |
        4 - 3           9   16 - 15
                        |
        5  -  12 - 11 - 10

    */
    DFS_Result dfs_2 = exeDFS(test_2, 1);
    //test_2.getDFS(1);
    show_dfn_res(dfs_2);

    //
    // //
    //

    ;

    // return 0;
};





int main() {
    std::cout << "//// //// BEGIN //// Linked //// >>>>" << std::endl;
    test_linked();
    std::cout << "//// //// NEXT //// Matrix //// >>>>" << std::endl;
    test_matrix();
    return 0;
};