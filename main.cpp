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

    RUN_ALL_TESTS();
    vector_test::vector_test();
    list_test::list_test();
    deque_test::deque_test();
    stack_test::stack_test();
    queue_test::queue_test();
    queue_test::priority_test();
    string_test::string_test();
    set_test::set_test();
    set_test::multiset_test();
    map_test::map_test();
    map_test::multimap_test();
    unordered_set_test::unordered_set_test();
    unordered_set_test::unordered_multiset_test();
    unordered_map_test::unordered_map_test();
    unordered_map_test::unordered_multimap_test();

    return 0;
}