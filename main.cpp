#include<iostream>
#include "algorithm_performance_test.h"
#include "algorithm_test.h"
#include "vector_test.h"
#include "list_test.h"
#include "deque_test.h"
#include "stack_test.h"
#include "queue_test.h"
#include "string_test.h"
#include "set_test.h"
#include "map_test.h"
#include "unordered_set_test.h"
#include "unordered_map_test.h"

int main() {

    using namespace tinySTL::test;

    std::cout.sync_with_stdio(false);

//    RUN_ALL_TESTS();
//    vector_test::vector_test();
//    list_test::list_test();
//    deque_test::deque_test();
//    stack_test::stack_test();
//    queue_test::queue_test();
//    queue_test::priority_test();
//    string_test::string_test();
//    set_test::set_test();
//    set_test::multiset_test();
//    map_test::map_test();
//    map_test::multimap_test();
//    unordered_set_test::unordered_set_test();
//    unordered_set_test::unordered_multiset_test();
//    unordered_map_test::unordered_map_test();
//    unordered_map_test::unordered_multimap_test();
    tinySTL::deque<int> l7{1, 2, 3, 4, 5, 6, 7, 8, 9};
    l7.begin();
    l7.end();
    l7.rbegin();
    l7.rend();
    l7.cbegin();
    l7.cend();
    l7.crbegin();
    l7.crend();

    tinySTL::list<int> l8{1, 2, 3, 4, 5, 6, 7, 8, 9};
    l8.begin();
    l8.end();
    l8.rbegin();
    l8.rend();
    l8.cbegin();
    l8.cend();
    l8.crbegin();
    l8.crend();

    tinySTL::vector<int> l9{1, 2, 3, 4, 5, 6, 7, 8, 9};
    l9.begin();
    l9.end();
    l9.rbegin();
    l9.rend();
    l9.cbegin();
    l9.cend();
    l9.crbegin();
    l9.crend();

    tinySTL::set<int> s9{1, 2, 3, 4, 5};
    s9.begin();
    s9.end();
    s9.rbegin();
    s9.rend();
    s9.cbegin();
    s9.cend();
    s9.crbegin();
    s9.crend();


    tinySTL::map<int, int> m9{PAIR(1, 1), PAIR(3, 2), PAIR(2, 3)};
    m9.begin();
    m9.end();
    m9.rbegin();
    m9.rend();
    m9.cbegin();
    m9.cend();
    m9.crbegin();
    m9.crend();

    tinySTL::unordered_map<int, int> um13{PAIR(1, 1), PAIR(2, 3), PAIR(3, 3)};
    um13.begin();
    um13.end();
    um13.cbegin();
    um13.cend();

    tinySTL::unordered_set<int> us13{ 1,2,3,4,5 };
    us13.begin();
    us13.end();
    us13.cbegin();
    us13.cend();

    tinySTL::string str4("abc");
    str4.begin();
    str4.end();
    str4.rbegin();
    str4.rend();
    str4.cbegin();
    str4.cend();
    str4.crbegin();
    str4.crend();

    return 0;
}