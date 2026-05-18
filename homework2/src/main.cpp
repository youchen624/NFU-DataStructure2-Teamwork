#include <iostream>
#include "Graph.h"

void show_dfn_res(const DFS_Result& dfs) {
    
    std::cout << "[order]:" << std::endl;
    for (auto const& v : dfs.order) {
        std::cout << v << ", ";
    } std::cout << std::endl << std::endl;

    // std::cout << "[CC]:" << std::endl;
    // for (auto const& vv : dfs.components) {
    //     std::cout << "[ ";
    //     for (auto const& v : vv) {
    //         std::cout << v << ", ";
    //     } std::cout << "]" << std::endl;
    // } std::cout << std::endl << std::endl;


    std::cout << "[articulation_points]:" << std::endl << "[ ";
    for (auto const& v : dfs.articulation_points) {
        std::cout << v << ", ";
    } std::cout << " ]" << std::endl;
    
    std::cout << "[BCC]:" << std::endl;
    for (auto const& ve : dfs.bcc_edges) {
        std::cout << "[ ";
        for (auto const& e : ve) {
            std::cout << "(" << e.u << ", " << e.v << "), ";
        } std::cout << "]" << std::endl;
    } std::cout << std::endl << std::endl;
    
    std::cout << "==== ====" << std::endl;
};

int main() {
    Graph* test = new UndiLinkedGraph;
    test->insert_edge(1,2);
    test->insert_edge(2,3);
    test->insert_edge(3,4);
    test->insert_edge(4,1);
    test->insert_edge(4,5);
    /*

        1 - 2
        |     |
        4 - 3
        |
        5

    */
    DFS_Result dfs = test->getDFS(1);
    show_dfn_res(dfs);


    //
    // //
    //


    Graph* test_1 = new DiLinkedGraph;
    test_1->insert_edge(1,2);
    test_1->insert_edge(2,3);
    // test_1->insert_edge(2,6);
    test_1->insert_edge(3,4);
    test_1->insert_edge(4,1);
    test_1->insert_edge(4,5);
    /*

        1 -> 2
        ↑      ↓
        4 <- 3
        ↓
        5

    */
    DFS_Result dfs_1 = test_1->getDFS(1);
    show_dfn_res(dfs_1);


    //
    // //
    //

    
    Graph* test_2 = new UndiLinkedGraph;
    const Edge edge_ar_2[] = {
        {1,2},   {2,3},    {2, 6},     {3, 4},
        {4,1},   {6,7},    {7,8},      {8,9},
        {8,13}, {9,10},  {10,11},  {11,12},
        {12,5}, {13,14}, {14,15}, {15,16},
        {16,13},
    };
    for (auto const& e : edge_ar_2) {
        test_2->insert_edge(e);
    }
    /*

        1 - 2 - 6 - 7 - 8 - 13 - 14
        |     |              |     |       |
        4 - 3             9     16 - 15
                             |
        5 - 12 - 11 - 10

    */
    DFS_Result dfs_2 = test_2->getDFS(1);
    show_dfn_res(dfs_2);

    //
    // //
    //

    ;

    return 0;
};