//
// Created by cqupt1811 on 2022/5/8.
//

/*
 * 这个头文件包含了 tinySTL 的一系列算法
 */

#ifndef TINYSTL_ALGO_H
#define TINYSTL_ALGO_H

#include <cstddef>  /* 头文件cstddef与其C对应版本兼容，它是C头文件<stddef.h>较新版本，定义了常用的常量、宏、类型和函数 */
#include <ctime>  /* 时间处理相关 */

#include "algobase.h"  /* 这个头文件包含了库中的一些基本算法 */
#include "memory.h"  /* 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "heap_algo.h"  /* 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap */
#include "functional.h"  /* 这个头文件包含了 tinySTL 的函数对象与哈希函数 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // ==============================================================================

    /*
     * all_of 函数
     * 检查[first, last)内是否全部元素都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
     */
    template<typename InputIter, typename UnaryPred>
    bool all_of(InputIter first, InputIter last, UnaryPred unary_pred) {
        for (; first != last; ++first) {
            if (!unary_pred(*first)) {
                return false;
            }
        }
        return true;
    }

    // ====================================================================================

    /*
     * any_of 函数
     * 检查[first, last)内是否存在某个元素满足一元操作 unary_pred 为 true 的情况，满足则返回 true
     */
    template<typename InputIter, typename UnaryPred>
    bool any_of(InputIter first, InputIter last, UnaryPred unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                return true;
            }
        }
        return false;
    }

    // ================================================================================================

    /*
     * none_of 函数
     * 检查[first, last)内是否全部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
     */
    template<typename InputIter, typename UnaryPred>
    bool none_of(InputIter first, InputIter last, UnaryPred unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                return false;
            }
        }
        return true;
    }

    // =============================================================================================

    /*
     * count 函数
     * 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
     */
    template<typename InputIter, typename T>
    size_t count(InputIter first, InputIter last, const T &value) {
        size_t n = 0;
        for (; first != last; ++first) {
            if (*first == value) {
                ++n;
            }
        }
        return n;
    }

    // =========================================================================================

    /*
     * count_if 函数
     * 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
     */
    template<typename InputIter, typename UnaryPred>
    size_t count_if(InputIter first, InputIter last, UnaryPred unary_pred) {
        size_t n = 0;
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                ++n;
            }
        }
        return n;
    }

    // =====================================================================================

    /*
     * find 函数
     * 在[first, last)区间内找到等于 value 的元素，返回指向该元素的迭代器
     */
    template<typename InputIter, typename T>
    InputIter find(InputIter first, InputIter last, const T &value) {
        while (first != last && *first != value) {
            ++first;
        }
        return first;
    }

    // =========================================================================================

    /*
     * find_if 函数
     * 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
     */
    template<typename InputIter, typename UnaryPred>
    InputIter find_if(InputIter first, InputIter last, UnaryPred unary_pred) {
        while (first != last && !unary_pred(*first)) {
            ++first;
        }
        return first;
    }

    // =======================================================================================

    /*
     * find_if_not 函数
     * 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
     */
    template<typename InputIter, typename UnaryPred>
    InputIter find_if_not(InputIter first, InputIter last, UnaryPred unary_pred) {
        while (first != last && unary_pred(*first)) {
            ++first;
        }
        return first;
    }

    // ======================================================================================

    /*
     * search 函数
     * 在[first1, last1)中查找[first2, last2)的首次出现点
     */
    template<typename ForwardIter1, typename ForwardIter2>
    ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2) {
        /* 获取两个序列的长度 */
        auto d1 = tinySTL::distance(first1, last1);
        auto d2 = tinySTL::distance(first2, last2);
        if (d1 < d2) {
            return last1;
        }
        auto cur1 = first1;
        auto cur2 = first2;
        while (cur2 != last2) {
            if (*cur1 == *cur2) {
                ++cur1;
                ++cur2;
            } else {
                if (d1 == d2) {
                    return last1;
                } else {
                    cur1 = ++first1;
                    cur2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    /*
     * search 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter1, typename ForwardIter2, typename Compared>
    ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2, Compared comp) {
        /* 获取两个序列的长度 */
        auto d1 = tinySTL::distance(first1, last1);
        auto d2 = tinySTL::distance(first2, last2);
        if (d1 < d2) {
            return last1;
        }
        auto cur1 = first1;
        auto cur2 = first2;
        while (cur2 != last2) {
            if (comp(*cur1, *cur2)) {
                ++cur1;
                ++cur2;
            } else {
                if (d1 == d2) {
                    return last1;
                } else {
                    cur1 = ++first1;
                    cur2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    // =========================================================================================

    /*
     * search_n 函数
     * 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
     */
    template<typename ForwardIter, typename Size, typename T>
    ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T &value) {
        if (n < 0) {
            return first;
        } else {
            first = tinySTL::find(first, last, value);
            while (first != last) {
                Size m = n - 1;
                ForwardIter cur = first;
                ++cur;
                while (cur != last && m != 0 && *cur == value) {
                    ++cur, --m;
                }
                if (m == 0) {
                    return first;
                } else {
                    first = tinySTL::find(cur, last, value);
                }
            }
            return last;
        }
    }

    /*
     * search_n 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Size, typename T, typename Compared>
    ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T &value, Compared comp) {
        if (n < 0) {
            return first;
        } else {
            while (first != last) {
                if (comp(*first, value)) {
                    break;
                }
                ++first;
            }
            while (first != last) {
                Size m = n - 1;
                ForwardIter cur = first;
                ++cur;
                while (cur != last && m != 0 && comp(*cur, value)) {
                    ++cur, --m;
                }
                if (m == 0) {
                    return first;
                } else {
                    while (cur != last) {
                        if (comp(*cur, value)) {
                            break;
                        }
                        ++cur;
                    }
                    first = cur;
                }
            }
            return last;
        }
    }

    // =============================================================================================

    /*
     * find_end 函数
     * 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
     */

    /*
     * find_end_dispatch 函数
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter1, typename ForwardIter2>
    ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                   ForwardIter2 first2, ForwardIter2 last2,
                                   tinySTL::forward_iterator_tag, tinySTL::forward_iterator_tag) {
        if (first1 == last1) {
            return last1;
        } else {
            ForwardIter1 result = last1;
            while (true) {
                /* 利用 search 查找某个子序列的首次出现点，找不到则返回 last1 */
                ForwardIter1 new_result = tinySTL::search(first1, last1, first2, last2);
                if (new_result == last1) {
                    return result;
                } else {
                    result = new_result;
                    first1 = new_result;
                    ++first1;
                }
            }
        }
    }

    /*
     * find_end_dispatch 函数
     * bidirectional_iterator_tag 版本
     * 使用reverse_iterator反向查找到的第一个即是最后一个
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
                                         BidirectionalIter2 first2, BidirectionalIter2 last2,
                                         tinySTL::random_access_iterator_tag, tinySTL::random_access_iterator_tag) {
        typedef tinySTL::reverse_iterator<BidirectionalIter1> reverse_iter1;
        typedef tinySTL::reverse_iterator<BidirectionalIter2> reverse_iter2;

        reverse_iter1 r_last1(first1);
        reverse_iter2 r_last2(first2);
        reverse_iter1 r_result = tinySTL::search(reverse_iter1(last1), r_last1, reverse_iter2(last2), r_last2);

        if (r_result == r_last1) {
            return last1;
        } else {
            BidirectionalIter1 result = r_result.base();
            tinySTL::advance(result, -tinySTL::distance(first2, last2));
            return result;
        }
    }

    /*
     * find_end 函数
     * 负责根据迭代器的种类分发任务给指定函数
     */
    template<typename ForwardIter1, typename ForwardIter2>
    ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                          ForwardIter2 first2, ForwardIter2 last2) {
        return tinySTL::find_end_dispatch(first1, last1, first2, last2,
                                          tinySTL::iterator_category(first1),
                                          tinySTL::iterator_category(first2));
    }

    /*
     * find_end_dispatch 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter1, typename ForwardIter2, typename Compared>
    ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                                   ForwardIter2 first2, ForwardIter2 last2,
                                   tinySTL::forward_iterator_tag, tinySTL::forward_iterator_tag, Compared comp) {
        if (first1 == last1) {
            return last1;
        } else {
            ForwardIter1 result = last1;
            while (true) {
                /* 利用 search 查找某个子序列的首次出现点，找不到则返回 last1 */
                ForwardIter1 new_result = tinySTL::search(first1, last1, first2, last2, comp);
                if (new_result == last1) {
                    return result;
                } else {
                    result = new_result;
                    first1 = new_result;
                    ++first1;
                }
            }
        }
    }

    /*
     * find_end_dispatch 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     * bidirectional_iterator_tag 版本
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2, typename Compared>
    BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
                                         BidirectionalIter2 first2, BidirectionalIter2 last2,
                                         tinySTL::random_access_iterator_tag, tinySTL::random_access_iterator_tag,
                                         Compared comp) {
        typedef tinySTL::reverse_iterator<BidirectionalIter1> reverse_iter1;
        typedef tinySTL::reverse_iterator<BidirectionalIter2> reverse_iter2;

        reverse_iter1 r_last1(first1);
        reverse_iter2 r_last2(first2);
        reverse_iter1 r_result = tinySTL::search(reverse_iter1(last1), r_last1, reverse_iter2(last2), r_last2, comp);

        if (r_result == r_last1) {
            return last1;
        } else {
            BidirectionalIter1 result = r_result.base();
            tinySTL::advance(result, -tinySTL::distance(first2, last2));
            return result;
        }
    }

    /*
     * find_end 函数
     * 负责根据迭代器的种类分发任务给指定函数
     */
    template<typename ForwardIter1, typename ForwardIter2, typename Compared>
    ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
                          ForwardIter2 first2, ForwardIter2 last2, Compared comp) {
        return tinySTL::find_end_dispatch(first1, last1, first2, last2,
                                          tinySTL::iterator_category(first1),
                                          tinySTL::iterator_category(first2), comp);
    }

    // =============================================================================================

    /*
     * find_first_of 函数
     * 在[first1, last1)中查找[first2, last2)中的某些元素，返回指向第一次出现的元素的迭代器
     */
    template<typename InputIter, typename ForwardIter>
    InputIter find_first_of(InputIter first1, InputIter last1,
                            ForwardIter first2, ForwardIter last2) {
        for (; first1 != last1; ++first1) {
            for (ForwardIter iter = first2; iter != last2; ++iter) {
                if (*first1 == *iter) {
                    return first1;
                }
            }
        }
        return last1;
    }

    /*
     * find_first_of 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter, typename ForwardIter, typename Compared>
    InputIter find_first_of(InputIter first1, InputIter last1,
                            ForwardIter first2, ForwardIter last2, Compared comp) {
        for (; first1 != last1; ++first1) {
            for (ForwardIter iter = first2; iter != last2; ++iter) {
                if (comp(*first1, *iter)) {
                    return first1;
                }
            }
        }
        return last1;
    }

    // =========================================================================================

    /*
     * for_each 函数
     * 使用一个函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
     * f() 可返回一个值，但该值会被忽略
     */
    template<typename InputIter, typename Function>
    Function for_each(InputIter first, InputIter last, Function func) {
        for (; first != last; ++first) {
            func(*first);
        }
        return func;
    }

    // ============================================================================================

    /*
     * adjacent_find 函数
     * 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
     */
    template<typename ForwardIter>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return last;
        }
        ForwardIter next = first;
        while (++next != last) {
            if (*first == *next) {
                return first;
            }
            first = next;
        }
        return last;
    }

    /*
     * adjacent_find 函数重载版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Compared>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last) {
            return last;
        }
        ForwardIter next = first;
        while (++next != last) {
            if (comp(*first, *next)) {
                return first;
            }
            first = next;
        }
        return last;
    }

    // ===================================================================================

    /*
     * lower_bound 函数
     * 在[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
     */

    /*
     * lower_bound_dispatch 函数
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T>
    ForwardIter lower_bound_dispatch(ForwardIter first, ForwardIter last, const T &value,
                                     tinySTL::forward_iterator_tag) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (*middle < value) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    /*
     * lower_bound_dispatch 函数
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter, typename T>
    RandomIter lower_bound_dispatch(RandomIter first, RandomIter last, const T &value,
                                    tinySTL::random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    /*
     * lower_bound 函数
     * 根据迭代器的种类分配任务
     */
    template<typename ForwardIter, typename T>
    ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T &value) {
        return tinySTL::lower_bound_dispatch(first, last, value, tinySTL::iterator_category(first));
    }

    /*
     * lower_bound_dispatch 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T, typename Compared>
    ForwardIter lower_bound_dispatch(ForwardIter first, ForwardIter last, const T &value,
                                     tinySTL::forward_iterator_tag, Compared comp) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (comp(*middle, value)) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    /*
     * lower_bound_dispatch 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter, typename T, typename Compared>
    RandomIter lower_bound_dispatch(RandomIter first, RandomIter last, const T &value,
                                    tinySTL::random_access_iterator_tag, Compared comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    /*
     * lower_bound 函数
     * 根据迭代器的种类分配任务
     */
    template<typename ForwardIter, typename T, typename Compared>
    ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return tinySTL::lower_bound_dispatch(first, last, value, tinySTL::iterator_category(first), comp);
    }

    // ===============================================================================================

    /*
     * upper_bound 函数
     * 在[first, last)中查找第一个大于value 的元素，并返回指向它的迭代器，若没有则返回 last
     */

    /*
     * upper_bound_dispatch 函数
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T>
    ForwardIter upper_bound_dispatch(ForwardIter first, ForwardIter last, const T &value,
                                     tinySTL::forward_iterator_tag) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (value < *middle) {
                len = half;
            } else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    /*
     * upper_bound_dispatch 函数
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter, typename T>
    RandomIter upper_bound_dispatch(RandomIter first, RandomIter last, const T &value,
                                    tinySTL::random_access_iterator_tag) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (value < *middle) {
                len = half;
            } else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    /*
     * upper_bound 函数
     * 根据迭代器类型分发任务
     */
    template<typename ForwardIter, typename T>
    ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T &value) {
        return tinySTL::upper_bound_dispatch(first, last, value, tinySTL::iterator_category(first));
    }

    /*
     * upper_bound_dispatch 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T, typename Compared>
    ForwardIter upper_bound_dispatch(ForwardIter first, ForwardIter last, const T &value,
                                     tinySTL::forward_iterator_tag, Compared comp) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (comp(value, *middle)) {
                len = half;
            } else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    /*
     * upper_bound_dispatch 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter, typename T, typename Compared>
    RandomIter upper_bound_dispatch(RandomIter first, RandomIter last, const T &value,
                                    tinySTL::random_access_iterator_tag, Compared comp) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(value, *middle)) {
                len = half;
            } else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    /*
     * upper_bound 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * 根据迭代器类型分发任务
     */
    template<typename ForwardIter, typename T, typename Compared>
    ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return tinySTL::upper_bound_dispatch(first, last, value, tinySTL::iterator_category(first), comp);
    }

    // =======================================================================================

    /*
     * binary_search 函数
     * 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
     * 在其中调用lower_bound实现查找
     */
    template<typename ForwardIter, typename T>
    bool binary_search(ForwardIter first, ForwardIter last, const T &value) {
        ForwardIter pos = tinySTL::lower_bound(first, last, value);
        return pos != last && !(value < *pos);
    }

    /*
     * binary_search 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename T, typename Compared>
    bool binary_search(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        ForwardIter pos = tinySTL::lower_bound(first, last, value, comp);
        return pos != last && !comp(value, *pos);
    }

    // ============================================================================================

    /*
     * equal_range 函数
     * 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
     * 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
     */

    /*
     * equal_range_dispatch 函数
     * forward_iterator_tag 版本
     * 首先找到值所在的区间中的点，然后再通过lower_bound与upper_bound找到上下界
     */
    template<typename ForwardIter, typename T>
    tinySTL::pair<ForwardIter, ForwardIter>  // 返回值
    equal_range_dispatch(ForwardIter first, ForwardIter last,
                         const T &value, tinySTL::forward_iterator_tag) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (*middle < value) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else if (value < *middle) {
                len = half;
            } else {
                left = tinySTL::lower_bound(first, middle, value);
                tinySTL::advance(first, len);
                right = tinySTL::upper_bound(++middle, first, value);
                return tinySTL::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return tinySTL::pair<ForwardIter, ForwardIter>(last, last);
    }

    /*
     * equal_range_dispatch 函数
     * random_access_iterator_tag 版本
     * 首先找到值所在的区间中的点，然后再通过lower_bound与upper_bound找到上下界
     */
    template<typename RandomIter, typename T>
    tinySTL::pair<RandomIter, RandomIter>  // 返回值
    equal_range_dispatch(RandomIter first, RandomIter last,
                         const T &value, tinySTL::random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            } else if (value < *middle) {
                len = half;
            } else {
                left = tinySTL::lower_bound(first, middle, value);
                right = tinySTL::upper_bound(++middle, first + len, value);
                return tinySTL::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return tinySTL::pair<RandomIter, RandomIter>(last, last);
    }

    /*
     * equal_range 函数
     * 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
     * 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
     * 在此函数中根据迭代器的类别分发任务给对应函数
     */
    template<typename ForwardIter, typename T>
    tinySTL::pair<ForwardIter, ForwardIter>  // 返回值
    equal_range(ForwardIter first, ForwardIter last, const T &value) {
        return tinySTL::equal_range_dispatch(first, last, value, tinySTL::iterator_category(first));
    }

    /*
     * equal_range_dispatch 函数
     * 重载版本使用函数对象 comp 代替比较操作
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T, typename Compared>
    tinySTL::pair<ForwardIter, ForwardIter>  // 返回值
    equal_range_dispatch(ForwardIter first, ForwardIter last,
                         const T &value, tinySTL::forward_iterator_tag, Compared comp) {
        auto len = tinySTL::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            tinySTL::advance(middle, half);
            if (comp(*middle, value)) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else if (comp(value, *middle)) {
                len = half;
            } else {
                left = tinySTL::lower_bound(first, middle, value, comp);
                tinySTL::advance(first, len);
                right = tinySTL::upper_bound(++middle, first, value, comp);
                return tinySTL::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return tinySTL::pair<ForwardIter, ForwardIter>(last, last);
    }

    /*
     * equal_range_dispatch 函数
     * random_access_iterator_tag 版本
     * 首先找到值所在的区间中的点，然后再通过lower_bound与upper_bound找到上下界
     */
    template<typename RandomIter, typename T, typename Compared>
    tinySTL::pair<RandomIter, RandomIter>  // 返回值
    equal_range_dispatch(RandomIter first, RandomIter last,
                         const T &value, tinySTL::random_access_iterator_tag, Compared comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            } else if (comp(value, *middle)) {
                len = half;
            } else {
                left = tinySTL::lower_bound(first, middle, value, comp);
                right = tinySTL::upper_bound(++middle, first + len, value, comp);
                return tinySTL::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return tinySTL::pair<RandomIter, RandomIter>(last, last);
    }

    /*
     * equal_range 函数
     * 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
     * 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
     * 在此函数中根据迭代器的类别分发任务给对应函数
     */
    template<typename ForwardIter, typename T, typename Compared>
    tinySTL::pair<ForwardIter, ForwardIter>  // 返回值
    equal_range(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return tinySTL::equal_range_dispatch(first, last, value, tinySTL::iterator_category(first), comp);
    }

    // ==========================================================================================

    /*
     * generate 函数
     * 将函数对象 gen 的运算结果对[first, last)内的每个元素赋值
     */
    template<typename ForwardIter, typename Generator>
    void generate(ForwardIter first, ForwardIter last, Generator gen) {
        for (; first != last; ++first) {
            *first = gen();
        }
    }

    // =======================================================================================

    /*
     * generate_n 函数
     * 用函数对象 gen 连续对 n 个元素赋值
     */
    template<typename ForwardIter, typename Size, typename Generator>
    void generate_n(ForwardIter first, Size n, Generator gen) {
        for (; n > 0; --n, ++first) {
            *first = gen();
        }
    }

    // ===========================================================================================

    /*
     * includes 函数
     * 判断序列一S1 是否包含序列二S2, S1与S2为增序序列
     */
    template<typename InputIter1, typename InputIter2>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2) {
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                return false;
            } else if (*first1 < *first2) {
                ++first1;
            } else {
                ++first1, ++first2;
            }
        }
        return first2 == last2;
    }

    /*
     * includes 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter1, typename InputIter2, typename Compared>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                return false;
            } else if (comp(*first1, *first2)) {
                ++first1;
            } else {
                ++first1, ++first2;
            }
        }
        return first2 == last2;
    }

    // =======================================================================

    /*
     * is_heap 函数
     * 检查[first, last)内的元素是否为一个堆(大根)，如果是，则返回 true
     * 从头检查是否满足父节点大于子节点
     */
    template<typename RandomIter>
    bool is_heap(RandomIter first, RandomIter last) {
        auto n = tinySTL::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (first[parent] < first[child]) {
                return false;
            }
            /* 每两个数切换一下父节点 */
            if ((child & 1) == 0) {
                ++parent;
            }
        }
        return true;
    }

    /*
     * is_heap 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    bool is_heap(RandomIter first, RandomIter last, Compared comp) {
        auto n = tinySTL::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (comp(first[parent], first[child])) {
                return false;
            }
            /* 每两个数切换一下父节点 */
            if ((child & 1) == 0) {
                ++parent;
            }
        }
        return true;
    }

    // ========================================================================================

    /*
     * is_sorted 函数
     * 检查[first, last)内的元素是否升序，如果是升序，则返回 true
     */
    template<typename ForwardIter>
    bool is_sorted(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return true;
        }
        ForwardIter next = first;
        ++next;
        while (next != last) {
            if (*next < *first) {
                return false;
            }
            first = next;
            ++next;
        }
        return true;
    }

    /*
     * is_sorted 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Compared>
    bool is_sorted(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last) {
            return true;
        }
        ForwardIter next = first;
        ++next;
        while (next != last) {
            if (comp(*next, *first)) {
                return false;
            }
            first = next;
            ++next;
        }
        return true;
    }

    // ===========================================================================================

    /*
     * median 函数
     * 找出三个值的中间值
     */
    template<typename T>
    const T &median(const T &left, const T &mid, const T &right) {
        if (left < mid) {
            if (mid < right) {
                return mid;
            } else if (left < right) {
                return right;
            } else {
                return left;
            }
        } else if (left < right) {
            return left;
        } else if (mid < right) {
            return right;
        } else {
            return mid;
        }
    }

    /*
     * median 函数
     * 找出三个值的中间值
     */
    template<typename T, typename Compared>
    const T &median(const T &left, const T &mid, const T &right, Compared comp) {
        if (comp(left, mid)) {
            if (comp(mid, right)) {
                return mid;
            } else if (comp(left, right)) {
                return right;
            } else {
                return left;
            }
        } else if (comp(left, right)) {
            return left;
        } else if (comp(mid, right)) {
            return right;
        } else {
            return mid;
        }
    }

    // ==============================================================================

    /*
     * max_element 函数
     * 返回一个迭代器，指向序列中最大的元素
     */
    template<typename ForwardIter>
    ForwardIter max_element(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return last;
        }
        ForwardIter max_ite = first;
        while (++first != last) {
            if (*max_ite < *first) {
                max_ite = first;
            }
        }
        return max_ite;
    }

    /*
     * max_element 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Compared>
    ForwardIter max_element(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last) {
            return last;
        }
        ForwardIter max_ite = first;
        while (++first != last) {
            if (comp(*max_ite, *first)) {
                max_ite = first;
            }
        }
        return max_ite;
    }

    // =========================================================================================

    /*
     * min_element 函数
     * 返回一个迭代器，指向序列中最大的元素
     */
    template<typename ForwardIter>
    ForwardIter min_element(ForwardIter first, ForwardIter last) {
        if (first == last) {
            return last;
        }
        ForwardIter max_ite = first;
        while (++first != last) {
            if (*first < *max_ite) {
                max_ite = first;
            }
        }
        return max_ite;
    }

    /*
     * min_element 函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Compared>
    ForwardIter min_element(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last) {
            return last;
        }
        ForwardIter max_ite = first;
        while (++first != last) {
            if (comp(*first, *max_ite)) {
                max_ite = first;
            }
        }
        return max_ite;
    }


    // =================================================================================

    /*
     * swap_ranges 函数
     * 将[first1, last1)从 first2 开始，交换相同个数元素
     * 交换的区间长度必须相同，两个序列不能互相重叠，返回一个迭代器指向序列二最后一个被交换元素的下一位置
     */
    template<typename ForwardIter1, typename ForwardIter2>
    ForwardIter2 swap_ranges(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            tinySTL::iter_swap(first1, first2);
        }
        return first2;
    }

    // ======================================================================================

    /*
     * transform 函数
     * 以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
     */
    template<typename InputIter, typename OutputIter, typename UnaryOperation>
    OutputIter transform(InputIter first, InputIter last, OutputIter result, UnaryOperation unary_op) {
        for (; first != last; ++first, ++result) {
            *result = unary_op(*first);
        }
        return result;
    }

    /*
     * transform 函数
     * 以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename BinaryOperation>
    OutputIter transform(InputIter1 first1, InputIter1 last1, InputIter2 first2,
                         OutputIter result, BinaryOperation binary_op) {
        for (; first1 != last1; ++first1, ++first2, ++result) {
            *result = binary_op(*first1, *first2);
        }
        return result;
    }

    // ===================================================================================

    /*
     * remove_copy 函数
     * 移除区间内与指定 value 相等的元素，并将结果复制到以 result 标示起始位置的容器上
     */
    template<typename InputIter, typename OutputIter, typename T>
    OutputIter remove_copy(InputIter first, InputIter last, OutputIter result, const T &value) {
        for (; first != last; ++first) {
            if (*first != value) {
                *result++ = *first;
            }
        }
        return result;
    }

    /*
     * remove 函数
     * 移除所有与指定 value 相等的元素
     * 并不从容器中删除这些元素，所以 remove 和 remove_if 不适用于 array
     * 可以调用remove_copy完成
     */
    template<typename ForwardIter, typename T>
    ForwardIter remove(ForwardIter first, ForwardIter last, const T &value) {
        /* 利用 find 找出第一个匹配的地方 */
        first = tinySTL::find(first, last, value);
        ForwardIter next = first;
        return first == last ? last : tinySTL::remove_copy(++next, last, first, value);
    }

    /*
     * remove_copy_if 函数
     * 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上
     */
    template<typename InputIter, typename OutputIter, typename UnaryPred>
    OutputIter remove_copy_if(InputIter first, InputIter last, OutputIter result, UnaryPred unary_pred) {
        for (; first != last; ++first) {
            if (!unary_pred(*first)) {
                *result++ = *first;
            }
        }
        return result;
    }

    /*
     * remove_if 函数
     * 移除区间内所有令一元操作 unary_pred 为 true 的元素
     */
    template<typename ForwardIter, typename UnaryPred>
    ForwardIter remove_if(ForwardIter first, ForwardIter last, UnaryPred unary_pred) {
        /* 利用 find_if 找出第一个匹配的地方 */
        first = tinySTL::find_if(first, last, unary_pred);
        ForwardIter next = first;
        return first == last ? last : tinySTL::remove_copy_if(++next, last, first, unary_pred);
    }

    // ======================================================================================

    /*
     * replace 函数
     * 将区间内所有的 old_value 都以 new_value 替代
     */
    template<typename ForwardIter, typename T>
    void replace(ForwardIter first, ForwardIter last, const T &old_value, const T &new_value) {
        for (; first != last; ++first) {
            if (*first == old_value) {
                *first = new_value;
            }
        }
    }

    /*
     * replace_copy 函数
     * 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
     */
    template<typename InputIter, typename OutputIter, typename T>
    OutputIter replace_copy(InputIter first, InputIter last, OutputIter result,
                            const T &old_value, const T &new_value) {
        for (; first != last; ++first, ++result) {
            if (*first == old_value) {
                *result = new_value;
            } else {
                *result = *first;
            }
        }
        return result;
    }

    /*
     * replace_copy_if
     * 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
     */
    template<typename InputIter, typename OutputIter, typename T, typename UnaryPred>
    OutputIter replace_copy_if(InputIter first, InputIter last, OutputIter result,
                               UnaryPred unary_pred, const T &new_value) {
        for (; first != last; ++first, ++result) {
            if (unary_pred(*first)) {
                *result = new_value;
            } else {
                *result = *first;
            }
        }
        return result;
    }

    /*
     * replace_if 函数
     * 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
     */
    template<typename ForwardIter, typename T, typename UnaryPred>
    void replace_if(ForwardIter first, ForwardIter last,
                    UnaryPred unary_pred, const T &new_value) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                *first = new_value;
            }
        }
    }

    // ==================================================================================

    /*
     * reverse 函数
     * 将[first, last)区间内的元素反转
     */

    /*
     * reverse_dispatch 函数
     * bidirectional_iterator_tag 版本
     */
    template<typename BidirectionalIter>
    void reverse_dispatch(BidirectionalIter first, BidirectionalIter last,
                          tinySTL::bidirectional_iterator_tag) {
        while (true) {
            if (first == last || first == --last) {
                return;
            }
            tinySTL::iter_swap(first++, last);
        }
    }

    /*
     * reverse_dispatch 函数
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter>
    void reverse_dispatch(RandomIter first, RandomIter last,
                          tinySTL::random_access_iterator_tag) {
        while (first < last) {
            tinySTL::iter_swap(first++, --last);
        }
    }

    /*
     * reverse 函数
     * 根据迭代器的种类调用不同的函数
     */
    template<typename BidirectionalIter>
    void reverse(BidirectionalIter first, BidirectionalIter last) {
        tinySTL::reverse_dispatch(first, last, tinySTL::iterator_category(first));
    }

    /*
     * reverse_copy 函数
     * 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
     */
    template<typename BidirectionalIter, typename OutputIter>
    OutputIter reverse_copy(BidirectionalIter first, BidirectionalIter last,
                            OutputIter result) {
        while (first != last) {
            *result++ = *--last;
        }
        return result;
    }

    // ===============================================================================

    /*
     * random_shuffle 函数
     * 将[first, last)内的元素次序随机重排
     */
    template<typename RandomIter>
    void random_shuffle(RandomIter first, RandomIter last) {
        if (first == last) {
            return;
        }
        /* 设置随机数种子 */
        srand((unsigned) time(0));
        for (auto i = first + 1; i != last; ++i) {
            tinySTL::iter_swap(i, first + (rand() % (i - first + 1)));
        }
    }

    /*
     * random_shuffle 函数重载
     * 重载版本使用一个产生随机数的函数对象 rand
     */
    template<typename RandomIter, typename RandomNumberGenerator>
    void random_shuffle(RandomIter first, RandomIter last, RandomNumberGenerator &rand) {
        if (first == last) {
            return;
        }
        auto len = tinySTL::distance(first, last);
        for (auto i = first + 1; i != last; ++i) {
            tinySTL::iter_swap(i, first + (rand(i - first + 1) % len));
        }
    }

    // ===============================================================================

    /*
     * rotate 函数
     * 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
     * 返回交换后 middle 的位置
     * 例如: A B C D E F G H (middle指向D)
     *      D E F G H A B C
     */

    /*
     * rotate_dispatch 函数
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter>
    ForwardIter rotate_dispatch(ForwardIter first, ForwardIter middle,
                                ForwardIter last, tinySTL::forward_iterator_tag) {
        ForwardIter first2 = middle;
        /* 后半段移到前面 */
        do {
            tinySTL::swap(*first++, *first2++);
            if (first == middle) {
                middle = first2;
            }
        } while (first2 != last);

        /* 迭代器返回的位置 */
        ForwardIter new_middle = first;
        first2 = middle;
        /* 调整剩余元素 */
        while (first2 != last) {
            tinySTL::swap(*first++, *first2++);
            if (first == middle) {
                middle = first2;
            } else if (first2 == last) {
                first2 = middle;
            }
        }
        return new_middle;
    }

    /*
     * rotate_dispatch 函数
     * bidirectional_iterator_tag 版本
     */
    template<typename BidirectionalIter>
    BidirectionalIter rotate_dispatch(BidirectionalIter first, BidirectionalIter middle,
                                      BidirectionalIter last, tinySTL::bidirectional_iterator_tag) {
        tinySTL::reverse_dispatch(first, middle, tinySTL::bidirectional_iterator_tag());
        tinySTL::reverse_dispatch(middle, last, tinySTL::bidirectional_iterator_tag());
        while (first != middle && middle != last) {
            tinySTL::swap(*first++, *--last);
        }
        if (first == middle) {
            tinySTL::reverse_dispatch(middle, last, tinySTL::bidirectional_iterator_tag());
        } else {
            tinySTL::reverse_dispatch(first, middle, tinySTL::bidirectional_iterator_tag());
            return first;
        }
    }

    /*
     * r_gcd函数
     * 求最大公因子
     */
    template<typename EuclideanRingElement>
    EuclideanRingElement r_gcd(EuclideanRingElement m, EuclideanRingElement n) {
        while (n != 0) {
            auto t = m % n;
            m = n;
            n = t;
        }
        return m;
    }

    /*
     * rotate_dispatch 函数
     * bidirectional_iterator_tag 版本
     */
    template<typename RandomIter>
    RandomIter rotate_dispatch(RandomIter first, RandomIter middle,
                               RandomIter last, tinySTL::random_access_iterator_tag) {
        /* 因为是 random access iterator，我们可以确定每个元素的位置 */
        auto n = last - first;
        auto l = middle - first;
        auto r = n - l;
        auto result = first + (last - middle);
        if (l == r) {
            tinySTL::swap_ranges(first, middle, middle);
            return result;
        }
        auto cycle_times = r_gcd(n, l);
        for (auto i = 0; i < cycle_times; ++i) {
            auto tmp = *first;
            auto p = first;
            if (l < r) {
                for (auto j = 0; j < r / cycle_times; ++j) {
                    if (p > first + r) {
                        *p = *(p - r);
                        p -= r;
                    }
                    *p = *(p + l);
                    p += l;
                }
            } else {
                for (auto j = 0; j < l / cycle_times - 1; ++j) {
                    if (p < last - l) {
                        *p = *(p + l);
                        p += l;
                    }
                    *p = *(p - r);
                    p -= r;
                }
            }
            *p = tmp;
            ++first;
        }
        return result;
    }

    /*
     * rotate 函数
     * 根据迭代器种类调用函数
     */
    template<typename ForwardIter>
    ForwardIter rotate(ForwardIter first, ForwardIter middle, ForwardIter last) {
        if (first == middle) {
            return last;
        }
        if (middle == last) {
            return first;
        }
        return tinySTL::rotate_dispatch(first, middle, last, tinySTL::iterator_category(first));
    }

    /*
     * rotate_copy 函数
     * 行为与 rotate 类似，不同的是将结果复制到 result 所指的容器中
     */
    template<typename ForwardIter, typename OutputIter>
    ForwardIter rotate_copy(ForwardIter first, ForwardIter middle,
                            ForwardIter last, OutputIter result) {
        return tinySTL::copy(first, middle, tinySTL::copy(middle, last, result));
    }

    // ==========================================================================================

    /*
     * is_permutation_aux 函数
     * 接受一个判断谓词
     */
    template<typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
    bool is_permutation_aux(ForwardIter1 first1, ForwardIter1 last1,
                            ForwardIter2 first2, ForwardIter2 last2,
                            BinaryPred pred) {
        constexpr bool is_ra_it = tinySTL::is_random_access_iterator<ForwardIter1>::value
                                  && tinySTL::is_random_access_iterator<ForwardIter2>::value;
        if (is_ra_it) {
            auto len1 = last1 - first1;
            auto len2 = last2 - first2;
            if (len1 != len2) {
                return false;
            }
        }

        /* 先找出相同的前缀段 */
        for (; first1 != last1 && first2 != last2; ++first1, (void) ++first2) {
            if (!pred(*first1, *first2)) {
                break;
            }
        }
        if (is_ra_it) {
            if (first1 == last1) {
                return true;
            }
        } else {
            auto len1 = tinySTL::distance(first1, last1);
            auto len2 = tinySTL::distance(first2, last2);
            if (len1 == 0 && len2 == 0) {
                return true;
            }
            if (len1 != len2) {
                return false;
            }
        }

        /* 判断剩余部分 */
        for (auto i = first1; i != last1; ++i) {
            bool is_repeated = false;
            for (auto j = first1; j != i; ++j) {
                if (pred(*j, *i)) {
                    is_repeated = true;
                    break;
                }
            }

            if (!is_repeated) {
                /* 计算 *i 在 [first2, last2) 的数目 */
                auto c2 = 0;
                for (auto j = first2; j != last2; ++j) {
                    if (pred(*i, *j)) {
                        ++c2;
                    }
                }
                if (c2 == 0) {
                    return false;
                }

                /*计算 *i 在 [first1, last1) 的数目 */
                auto c1 = 1;
                auto j = i;
                for (++j; j != last1; ++j) {
                    if (pred(*i, *j)) {
                        ++c1;
                    }
                }
                if (c1 != c2) {
                    return false;
                }
            }
        }
        return true;
    }

    /*
     * is_permutation 函数
     * 判断[first1,last1)是否为[first2, last2)的排列组合
     * 使用equal_to函数对象传入is_permutation_aux函数
     */
    template<typename ForwardIter1, typename ForwardIter2>
    bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2) {
        typedef typename iterator_traits<ForwardIter1>::value_type v1;
        typedef typename iterator_traits<ForwardIter2>::value_type v2;
        static_assert(std::is_same<v1, v2>::value,
                      "the type should be same in mystl::is_permutation");
        return is_permutation_aux(first1, last1, first2, last2, tinySTL::equal_to<v1>());
    }

    /*
     * is_permutation 函数重载
     * 接受一个比较对象
     */
    template<typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
    bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2,
                        BinaryPred pred) {
        return is_permutation_aux(first1, last1, first2, last2, pred);
    }

    // ====================================================================================

    /*
     * next_permutation 函数
     * 可见侯捷老师的 STL源码分析
     * 取得[first, last)所标示序列的下一个排列组合，如果没有下一个排序组合，返回 false，否则返回 true
     */
    template<typename BidirectionalIter>
    bool next_permutation(BidirectionalIter first, BidirectionalIter last) {
        auto i = last;
        if (first == last || first == --i) {
            return false;
        }
        for (;;) {
            auto ii = i;
            /* 找到第一对小于关系的元素 */
            if (*--i < *ii) {
                auto j = last;
                while (!(*i < *--j)) {

                }
                /* 交换 i，j 所指元素 */
                tinySTL::iter_swap(i, j);
                /* 将 ii 之后的所有元素反转 */
                tinySTL::reverse(ii, last);
                return true;
            }
            if (i == first) {
                tinySTL::reverse(first, last);
                return false;
            }
        }
    }

    /*
     * next_permutation 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename Compared>
    bool next_permutation(BidirectionalIter first, BidirectionalIter last, Compared comp) {
        auto i = last;
        if (first == last || first == --i) {
            return false;
        }
        for (;;) {
            auto ii = i;
            /* 找到第一对小于关系的元素 */
            if (comp(*--i, *ii)) {
                auto j = last;
                while (!comp(*i, *--j)) {

                }
                /* 交换 i，j 所指元素 */
                tinySTL::iter_swap(i, j);
                /* 将 ii 之后的所有元素反转 */
                tinySTL::reverse(ii, last);
                return true;
            }
            if (i == first) {
                tinySTL::reverse(first, last);
                return false;
            }
        }
    }

    // ============================================================================================

    /*
     * prev_permutation 函数
     * 可见侯捷老师的 STL源码分析
     * 取得[first, last)所标示序列的上一个排列组合，如果没有上一个排序组合，返回 false，否则返回 true
     */
    template<typename BidirectionalIter>
    bool prev_permutation(BidirectionalIter first, BidirectionalIter last) {
        auto i = last;
        if (first == last || first == --i) {
            return false;
        }
        for (;;) {
            auto ii = i;
            /* 找到第一对大于关系的元素 */
            if (*ii < *--i) {
                auto j = last;
                while (!(*--j < *i)) {

                }
                /* 交换i，j */
                tinySTL::iter_swap(i, j);
                /* 将 ii 之后的所有元素反转 */
                tinySTL::reverse(ii, last);
                return true;
            }
            if (i == first) {
                tinySTL::reverse(first, last);
                return false;
            }
        }
    }

    /*
     * prev_permutation 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename Compared>
    bool prev_permutation(BidirectionalIter first, BidirectionalIter last, Compared comp) {
        auto i = last;
        if (first == last || first == --i) {
            return false;
        }
        for (;;) {
            auto ii = i;
            /* 找到第一对大于关系的元素 */
            if (comp(*ii, *--i)) {
                auto j = last;
                while (!comp(*--j, *i)) {

                }
                /* 交换i，j */
                tinySTL::iter_swap(i, j);
                /* 将 ii 之后的所有元素反转 */
                tinySTL::reverse(ii, last);
                return true;
            }
            if (i == first) {
                tinySTL::reverse(first, last);
                return false;
            }
        }
    }

    // =====================================================================================

    /*
     * merge 函数
     * 将两个经过排序的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter>
    OutputIter merge(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2,
                     OutputIter result) {
        while (first1 != last1 || first2 != last2) {
            if (first1 == last1) {
                *result = *first2++;
            } else if (first2 == last2) {
                *result = *first1++;
            } else {
                if (*first1 < *first2) {
                    *result = *first1++;
                } else {
                    *result = *first2++;
                }
            }
            ++result;
        }
        return result;
    }

    /*
     * merge 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename Compared>
    OutputIter merge(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2,
                     OutputIter result, Compared comp) {
        while (first1 != last1 || first2 != last2) {
            if (first1 == last1) {
                *result = *first2++;
            } else if (first2 == last2) {
                *result = *first1++;
            } else {
                if (comp(*first1, *first2)) {
                    *result = *first1++;
                } else {
                    *result = *first2++;
                }
            }
            ++result;
        }
        return result;
    }

    // ============================================================================================

    /*
     * inplace_merge 函数
     * 把连接在一起的两个有序序列结合成单一序列并保持有序
     */

    /*
     * merge_without_buffer 函数
     * 在没有缓冲区的情况下合并
     */
    template<typename BidirectionalIter, typename Distance>
    void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
                              BidirectionalIter last, Distance len1, Distance len2) {
        if (len1 == 0 || len2 == 0) {
            return;
        }
        if (len1 + len2 == 2) {
            if (*middle < *first) {
                tinySTL::iter_swap(first, middle);
            }
            return;
        }
        BidirectionalIter first_cut = first;
        BidirectionalIter second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        /* 序列一较长，找到序列一的中点 */
        if (len1 > len2) {
            len11 = len1 >> 1;
            tinySTL::advance(first_cut, len11);
            second_cut = tinySTL::lower_bound(middle, last, *first_cut);
            len22 = tinySTL::distance(middle, second_cut);
        } else {
            len22 = len2 >> 1;
            tinySTL::advance(second_cut, len22);
            first_cut = tinySTL::upper_bound(first, middle, *second_cut);
            len11 = tinySTL::distance(first, first_cut);
        }
        BidirectionalIter new_middle = tinySTL::rotate(first_cut, middle, second_cut);
        tinySTL::merge_without_buffer(first, first_cut, new_middle, len11, len22);
        tinySTL::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22);
    }

    /*
     * merge_backward 函数
     * 反方向merge
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter1 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
                                      BidirectionalIter2 first2, BidirectionalIter2 last2,
                                      BidirectionalIter1 result) {
        if (first1 == last1) {
            return tinySTL::copy_backward(first2, last2, result);
        }
        if (first2 == last2) {
            return tinySTL::copy_backward(first1, last1, result);
        }
        --last1, --last2;
        while (true) {
            if (*last2 < *last1) {
                *--result = *last1;
                if (first1 == last1) {
                    return tinySTL::copy_backward(first2, ++last2, result);
                }
                --last1;
            } else {
                *--result = *last2;
                if (first2 == last2) {
                    return tinySTL::copy_backward(first1, ++last1, result);
                }
                --last2;
            }
        }
    }

    /*
     * rotate_adaptive 函数
     * 使用缓冲空间旋转迭代器间的元素
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2, typename Distance>
    BidirectionalIter1 rotate_adaptive(BidirectionalIter1 first, BidirectionalIter1 middle,
                                       BidirectionalIter1 last, Distance len1, Distance len2,
                                       BidirectionalIter2 buffer, Distance buffer_size) {
        BidirectionalIter2 buffer_end;
        if (len1 > len2 && len2 <= buffer_size) {
            buffer_end = tinySTL::copy(middle, last, buffer);
            tinySTL::copy_backward(first, middle, last);
            return tinySTL::copy(buffer, buffer_end, first);
        } else if (len1 <= buffer_size) {
            buffer_end = tinySTL::copy(first, middle, buffer);
            tinySTL::copy(middle, last, first);
            return tinySTL::copy_backward(buffer, buffer_end, last);
        } else {
            return tinySTL::rotate(first, middle, last);
        }
    }

    /*
     * merge_adaptive 函数
     * 有缓冲区的情况下合并
     */
    template<typename BidirectionalIter, typename Distance, typename Pointer>
    void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
                        BidirectionalIter last, Distance len1, Distance len2,
                        Pointer buffer, Distance buffer_size) {
        /* 区间长度足够放进缓冲区 */
        if (len1 <= len2 && len1 <= buffer_size) {
            Pointer buffer_end = tinySTL::copy(first, middle, buffer);
            tinySTL::merge(buffer, buffer_end, middle, last, first);
        } else if (len2 <= buffer_size) {
            Pointer buffer_end = tinySTL::copy(middle, last, buffer);
            tinySTL::merge_backward(first, middle, buffer, buffer_end, last);
        } else {
            /* 区间长度太长，分割递归处理 */
            BidirectionalIter first_cut = first;
            BidirectionalIter second_cut = middle;
            Distance len11 = 0;
            Distance len22 = 0;
            if (len1 > len2) {
                len11 = len1 >> 1;
                tinySTL::advance(first_cut, len11);
                second_cut = tinySTL::lower_bound(middle, last, *first_cut);
                len22 = tinySTL::distance(middle, second_cut);
            } else {
                len22 = len2 >> 1;
                tinySTL::advance(second_cut, len22);
                first_cut = tinySTL::upper_bound(first, middle, *second_cut);
                len11 = tinySTL::distance(first, first_cut);
            }
            BidirectionalIter new_middle = tinySTL::rotate_adaptive(
                    first_cut, middle, second_cut, len1 - len11, len22, buffer, buffer_size);
            tinySTL::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size);
            tinySTL::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size);
        }
    }

    /*
     * inplace_merge_aux 函数
     * 申请缓冲空间，根据申请结果调用对应的函数
     */
    template<typename BidirectionalIter, typename T>
    void inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last, T *) {
        auto len1 = tinySTL::distance(first, middle);
        auto len2 = tinySTL::distance(middle, last);
        /* 申请缓冲空间 */
        tinySTL::temporary_buffer<BidirectionalIter, T> buf(first, last);
        if (!buf.begin()) {
            /* 未成功申请就调用不使用缓冲取的版本 */
            tinySTL::merge_without_buffer(first, middle, last, len1, len2);
        } else {
            /* 申请成功则调用缓冲区版本 */
            tinySTL::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size());
        }
    }

    /*
     * inplace_merge 函数
     * 在此函数中调用inplace_merge_aux函数完成功能
     */
    template<typename BidirectionalIter>
    void inplace_merge(BidirectionalIter first, BidirectionalIter middle, BidirectionalIter last) {
        if (first == middle || middle == last) {
            return;
        }
        tinySTL::inplace_merge_aux(first, middle, last, tinySTL::value_type(first));
    }

    /*
     * merge_without_buffer 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename Distance, typename Compared>
    void merge_without_buffer(BidirectionalIter first, BidirectionalIter middle,
                              BidirectionalIter last, Distance len1, Distance len2,
                              Compared comp) {
        if (len1 == 0 || len2 == 0) {
            return;
        }
        if (len1 + len2 == 2) {
            if (comp(*middle, *first)) {
                tinySTL::iter_swap(first, middle);
            }
            return;
        }
        auto first_cut = first;
        auto second_cut = middle;
        Distance len11 = 0;
        Distance len22 = 0;
        if (len1 > len2) {
            len11 = len1 >> 1;
            tinySTL::advance(first_cut, len11);
            second_cut = tinySTL::lower_bound(middle, last, *first_cut, comp);
            len22 = tinySTL::distance(middle, second_cut);
        } else {
            len22 = len2 >> 1;
            tinySTL::advance(second_cut, len22);
            first_cut = tinySTL::upper_bound(first, middle, *second_cut, comp);
            len11 = tinySTL::distance(first, first_cut);
        }
        auto new_middle = tinySTL::rotate(first_cut, middle, second_cut);
        tinySTL::merge_without_buffer(first, first_cut, new_middle, len11, len22, comp);
        tinySTL::merge_without_buffer(new_middle, second_cut, last, len1 - len11, len2 - len22, comp);
    }

    /*
     * merge_backward 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2, typename Compared>
    BidirectionalIter1 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
                                      BidirectionalIter2 first2, BidirectionalIter2 last2,
                                      BidirectionalIter1 result, Compared comp) {
        if (first1 == last1) {
            return tinySTL::copy_backward(first2, last2, result);
        }
        if (first2 == last2) {
            return tinySTL::copy_backward(first1, last1, result);
        }
        --last1;
        --last2;
        while (true) {
            if (comp(*last2, *last1)) {
                *--result = *last1;
                if (first1 == last1) {
                    return tinySTL::copy_backward(first2, ++last2, result);
                }
                --last1;
            } else {
                *--result = *last2;
                if (first2 == last2) {
                    return tinySTL::copy_backward(first1, ++last1, result);
                }
                --last2;
            }
        }
    }

    /*
     * merge_adaptive 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename Distance, typename Pointer, typename Compared>
    void merge_adaptive(BidirectionalIter first, BidirectionalIter middle,
                        BidirectionalIter last, Distance len1, Distance len2,
                        Pointer buffer, Distance buffer_size, Compared comp) {
        /* 区间长度足够放进缓冲区 */
        if (len1 <= len2 && len1 <= buffer_size) {
            Pointer buffer_end = tinySTL::copy(first, middle, buffer);
            tinySTL::merge(buffer, buffer_end, middle, last, first, comp);
        } else if (len2 <= buffer_size) {
            Pointer buffer_end = tinySTL::copy(middle, last, buffer);
            tinySTL::merge_backward(first, middle, buffer, buffer_end, last, comp);
        } else {
            /* 区间长度太长，分割递归处理 */
            auto first_cut = first;
            auto second_cut = middle;
            Distance len11 = 0;
            Distance len22 = 0;
            if (len1 > len2) {
                len11 = len1 >> 1;
                tinySTL::advance(first_cut, len11);
                second_cut = tinySTL::lower_bound(middle, last, *first_cut, comp);
                len22 = tinySTL::distance(middle, second_cut);
            } else {
                len22 = len2 >> 1;
                tinySTL::advance(second_cut, len22);
                first_cut = tinySTL::upper_bound(first, middle, *second_cut, comp);
                len11 = tinySTL::distance(first, first_cut);
            }
            auto new_middle = tinySTL::rotate_adaptive(first_cut, middle, second_cut, len1 - len11,
                                                       len22, buffer, buffer_size);
            tinySTL::merge_adaptive(first, first_cut, new_middle, len11, len22, buffer, buffer_size, comp);
            tinySTL::merge_adaptive(new_middle, second_cut, last, len1 - len11, len2 - len22, buffer, buffer_size,
                                    comp);
        }
    }

    /*
     * inplace_merge_aux 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename T, typename Compared>
    void inplace_merge_aux(BidirectionalIter first, BidirectionalIter middle,
                           BidirectionalIter last, T *, Compared comp) {
        auto len1 = tinySTL::distance(first, middle);
        auto len2 = tinySTL::distance(middle, last);
        temporary_buffer<BidirectionalIter, T> buf(first, last);
        if (!buf.begin()) {
            tinySTL::merge_without_buffer(first, middle, last, len1, len2, comp);
        } else {
            tinySTL::merge_adaptive(first, middle, last, len1, len2, buf.begin(), buf.size(), comp);
        }
    }

    /*
     * inplace_merge 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename BidirectionalIter, typename Compared>
    void inplace_merge(BidirectionalIter first, BidirectionalIter middle,
                       BidirectionalIter last, Compared comp) {
        if (first == middle || middle == last) {
            return;
        }
        tinySTL::inplace_merge_aux(first, middle, last, value_type(first), comp);
    }

    // ==================================================================================

    /*
     * partial_sort 函数
     * 对整个序列做部分排序，保证较小的 N 个元素以递增顺序置于[first, first + N)中
     * 使用大根堆实现
     */
    template<typename RandomIter>
    void partial_sort(RandomIter first, RandomIter middle, RandomIter last) {
        /* 首先建立一个大根堆，大小为middle-first，因为只需要对这一部分机型排序 */
        tinySTL::make_heap(first, middle);
        for (RandomIter i = middle; i < last; ++i) {
            /* 每一次将后面区间的一个值与根比较，如果比根小，那么将原来的根出堆，将*i加入堆中 */
            if (*i < *first) {
                tinySTL::pop_heap_aux(first, middle, i, *i, tinySTL::distance_type(first));
            }
        }
        /* 将大根堆转化为排序状态 */
        tinySTL::sort_heap(first, middle);
    }

    /*
     * partial_sort 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void partial_sort(RandomIter first, RandomIter middle, RandomIter last, Compared comp) {
        /* 首先建立一个大根堆，大小为middle-first，因为只需要对这一部分机型排序 */
        tinySTL::make_heap(first, middle, comp);
        for (RandomIter i = middle; i < last; ++i) {
            /* 每一次将后面区间的一个值与根比较，如果比根小，那么将原来的根出堆，将*i加入堆中 */
            if (comp(*i, *first)) {
                tinySTL::pop_heap_aux(first, middle, i, *i, tinySTL::distance_type(first), comp);
            }
        }
        /* 将大根堆转化为排序状态 */
        tinySTL::sort_heap(first, middle, comp);
    }

    // =======================================================================================

    /*
     * partial_sort_copy 函数
     * 行为与 partial_sort 类似，不同的是把排序结果复制到 result 容器中
     */

    /*
     * partial_sort_copy_aux 函数
     * 此函数使用大根堆进行partial排序
     */
    template<typename InputIter, typename RandomIter, typename Distance>
    RandomIter partial_sort_aux(InputIter first, InputIter last,
                                RandomIter result_first, RandomIter result_last, Distance *) {
        if (result_first == result_last) {
            return result_last;
        }
        RandomIter result_iter = result_first;
        /* 复制原空间中前目标空间大小个数据到目标空间中 */
        while (first != last && result_iter != result_last) {
            *result_iter = *first;
            ++result_iter;
            ++first;
        }
        /* 在目标空间制作一个大根堆 */
        tinySTL::make_heap(result_first, result_last);
        while (first != last) {
            /* 每一次将后面区间的一个值与根比较，如果比根小，那么将原来的根出堆，将*i加入堆中 */
            if (*first < *result_first) {
                tinySTL::adjust_heap(result_first, static_cast<Distance>(0), result_iter - result_first, *first);
            }
            ++first;
        }
        /* 将大根堆转化为排序状态 */
        tinySTL::sort_heap(result_first, result_iter);
        return result_iter;
    }

    /*
     * partial_sort_copy 函数
     * 调用partial_sort_aux函数完成擦欧总
     */
    template<typename InputIter, typename RandomIter>
    RandomIter partial_sort_copy(InputIter first, InputIter last,
                                 RandomIter result_first, RandomIter result_last) {
        return tinySTL::partial_sort_aux(first, last, result_first, result_last, tinySTL::distance_type(result_first));
    }

    /*
     * partial_sort_copy_aux 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter, typename RandomIter, typename Distance, typename Compared>
    RandomIter partial_sort_aux(InputIter first, InputIter last,
                                RandomIter result_first, RandomIter result_last,
                                Distance *, Compared comp) {
        if (result_first == result_last) {
            return result_last;
        }
        RandomIter result_iter = result_first;
        /* 复制原空间中前目标空间大小个数据到目标空间中 */
        while (first != last && result_iter != result_last) {
            *result_iter = *first;
            ++result_iter;
            ++first;
        }
        /* 在目标空间制作一个大根堆 */
        tinySTL::make_heap(result_first, result_last, comp);
        while (first != last) {
            /* 每一次将后面区间的一个值与根比较，如果比根小，那么将原来的根出堆，将*i加入堆中 */
            if (comp(*first, *result_first)) {
                tinySTL::adjust_heap(result_first, static_cast<Distance>(0),
                                     result_iter - result_first, *first, comp);
            }
            ++first;
        }
        /* 将大根堆转化为排序状态 */
        tinySTL::sort_heap(result_first, result_iter, comp);
        return result_iter;
    }

    /*
     * partial_sort_copy 函数
     * 调用partial_sort_aux函数完成擦欧总
     */
    template<typename InputIter, typename RandomIter, typename Compared>
    RandomIter partial_sort_copy(InputIter first, InputIter last, RandomIter result_first,
                                 RandomIter result_last, Compared comp) {
        return tinySTL::partial_sort_aux(first, last, result_first, result_last,
                                         tinySTL::distance_type(result_first), comp);
    }

    // ==============================================================================================

    /*
     * partition 函数
     * 对区间内的元素重排，被一元条件运算判定为 true 的元素会放到区间的前段
     * 该函数不保证元素的原始相对位置
     * 在每一个while循环中从前查找第一个不满足unary_pred条件的，和从后往前第一个满足条件的，并将其互换位置
     * 直到前后指针相遇
     */
    template<typename BidirectionalIter, typename UnaryPred>
    BidirectionalIter partition(BidirectionalIter first, BidirectionalIter last, UnaryPred unary_pred) {
        while (true) {
            while (first != last && unary_pred(*first)) {
                ++first;
            }
            if (first == last) {
                break;
            }
            --last;
            while (first != last && !unary_pred(*last)) {
                --last;
            }
            if (first == last) {
                break;
            }
            tinySTL::iter_swap(first, last);
            ++first;
        }
        return first;
    }

    // ===================================================================================

    /*
     * partition_copy 函数
     * 行为与 partition 类似，不同的是，将被一元操作符判定为 true 的放到 result_true 的输出区间
     * 其余放到 result_false 的输出区间，并返回一个 mystl::pair 指向这两个区间的尾部
     */
    template<typename InputIter, typename OutputIter1, typename OutputIter2, typename UnaryPred>
    tinySTL::pair<OutputIter1, OutputIter2>  // 返回值
    partition_copy(InputIter first, InputIter last, OutputIter1 result_true,
                   OutputIter2 result_false, UnaryPred unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                *result_true++ = *first;
            } else {
                *result_false++ = *first;
            }
        }
        return tinySTL::pair<OutputIter1, OutputIter2>(result_true, result_false);
    }

    // ==========================================================================================

    /*
     * sort 系列函数
     * 将[first, last)内的元素以递增的方式排序
     * kSmallSectionSize表示小型区间的大小，在这个大小内采用插入排序
     */
    constexpr static size_t kSmallSectionSize = 128;

    /*
     * slg2 函数
     * 辅助函数
     * 用于控制分割恶化的情况,找到log_2^k<=n的最大K值
     */
    template<typename Size>
    Size slg2(Size n) {
        /* 找出 lgk <= n 的 k 的最大值 */
        Size k = 0;
        for (; n > 1; n >>= 1) {
            ++k;
        }
        return k;
    }

    /*
     * unchecked_partition 函数
     * 快排中的分割函数，按照pivot将区间划分为凉拌
     */
    template<typename RandomIter, typename T>
    RandomIter unchecked_partition(RandomIter first, RandomIter last, const T &pivot) {
        while (true) {
            while (*first < pivot) {
                ++first;
            }
            --last;
            while (pivot < *last) {
                --last;
            }
            if (!(first < last)) {
                return first;
            }
            tinySTL::iter_swap(first, last);
            ++first;
        }
    }

    /*
     * intro_sort 函数
     * 可见 侯捷老师的 STL源码分析
     * 内省式排序，先进行 quick sort，当分割行为有恶化倾向时，改用 heap sort
     */
    template<typename RandomIter, typename Size>
    void intro_sort(RandomIter first, RandomIter last, Size depth_limit) {
        while (static_cast<size_t>(last - first) > kSmallSectionSize) {
            /* 到达最大分割深度限制 */
            if (depth_limit == 0) {
                /* 改用 heap_sort */
                tinySTL::partial_sort(first, last, last);
                return;
            }
            --depth_limit;
            auto mid = tinySTL::median(*first, *(first + (last - first) / 2), *(last - 1));
            auto cut = tinySTL::unchecked_partition(first, last, mid);
            tinySTL::intro_sort(cut, last, depth_limit);
            last = cut;
        }
    }

    /*
     * unchecked_linear_insert 函数
     * 插入排序辅助函数
     */
    template<typename RandomIter, typename T>
    void unchecked_linear_insert(RandomIter last, const T &value) {
        RandomIter next = last;
        --next;
        while (value < *next) {
            *last = *next;
            last = next;
            --next;
        }
        *last = value;
    }

    /*
     * unchecked_insertion_sort 函数
     * 插入排序辅助函数
     */
    template<typename RandomIter>
    void unchecked_insertion_sort(RandomIter first, RandomIter last) {
        for (RandomIter i = first; i != last; ++i) {
            tinySTL::unchecked_linear_insert(i, *i);
        }
    }

    /*
     * insertion_sort 函数
     * 插入排序函数
     */
    template<typename RandomIter>
    void insertion_sort(RandomIter first, RandomIter last) {
        if (first == last) {
            return;
        }
        for (RandomIter i = first + 1; i != last; ++i) {
            auto value = *i;
            /* 如果比第一个值都小，那么调用copy_backward函数将所有元素后移一个位置 */
            if (value < *first) {
                tinySTL::copy_backward(first, i, i + 1);
                *first = value;
            } else {
                /* 否则使用常规的插入排序 */
                tinySTL::unchecked_linear_insert(i, value);
            }
        }
    }

    /*
     * final_insertion_sort 函数
     * 最终插入排序函数
     */
    template<typename RandomIter>
    void final_insertion_sort(RandomIter first, RandomIter last) {
        if (static_cast<size_t>(last - first) > kSmallSectionSize) {
            /* 若高于设定的值，前 kSmallSectionSize 个值使用 insertion_sort，后面使用 unchecked_insertion_sort */
            tinySTL::insertion_sort(first, first + kSmallSectionSize);
            tinySTL::unchecked_insertion_sort(first + kSmallSectionSize, last);
        } else {
            /* 若低于设定的值,采用insertion_sort */
            tinySTL::insertion_sort(first, last);
        }
    }

    /*
     * sort 函数
     */
    template<typename RandomIter>
    void sort(RandomIter first, RandomIter last) {
        if (first != last) {
            /* 内省式排序，将区间分为一个个小区间，然后对整体进行插入排序 */
            tinySTL::intro_sort(first, last, slg2(last - first) * 2);
            tinySTL::final_insertion_sort(first, last);
        }
    }

    /*
     * unchecked_partition 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename T, typename Compared>
    RandomIter unchecked_partition(RandomIter first, RandomIter last,
                                   const T &pivot, Compared comp) {
        while (true) {
            while (comp(*first, pivot)) {
                ++first;
            }
            --last;
            while (comp(pivot, *last)) {
                --last;
            }
            if (!(first < last)) {
                return first;
            }
            tinySTL::iter_swap(first, last);
            ++first;
        }
    }

    /*
     * intro_sort 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Size, typename Compared>
    void intro_sort(RandomIter first, RandomIter last,
                    Size depth_limit, Compared comp) {
        while (static_cast<size_t>(last - first) > kSmallSectionSize) {
            /* 到达最大分割深度限制 */
            if (depth_limit == 0) {
                /* 改用 heap_sort */
                tinySTL::partial_sort(first, last, last, comp);
                return;
            }
            --depth_limit;
            auto mid = tinySTL::median(*(first), *(first + (last - first) / 2), *(last - 1));
            auto cut = tinySTL::unchecked_partition(first, last, mid, comp);
            tinySTL::intro_sort(cut, last, depth_limit, comp);
            last = cut;
        }
    }

    /*
     * unchecked_linear_insert 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename T, typename Compared>
    void unchecked_linear_insert(RandomIter last, const T &value, Compared comp) {
        auto next = last;
        --next;
        while (comp(value, *next)) {  // 从尾部开始寻找第一个可插入位置
            *last = *next;
            last = next;
            --next;
        }
        *last = value;
    }

    /*
     * unchecked_insertion_sort 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void unchecked_insertion_sort(RandomIter first, RandomIter last,
                                  Compared comp) {
        for (auto i = first; i != last; ++i) {
            tinySTL::unchecked_linear_insert(i, *i, comp);
        }
    }

    /*
     * insertion_sort 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void insertion_sort(RandomIter first, RandomIter last, Compared comp) {
        if (first == last) {
            return;
        }
        for (auto i = first + 1; i != last; ++i) {
            auto value = *i;
            if (comp(value, *first)) {
                tinySTL::copy_backward(first, i, i + 1);
                *first = value;
            } else {
                tinySTL::unchecked_linear_insert(i, value, comp);
            }
        }
    }

    /*
     * final_insertion_sort 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void final_insertion_sort(RandomIter first, RandomIter last, Compared comp) {
        if (static_cast<size_t>(last - first) > kSmallSectionSize) {
            tinySTL::insertion_sort(first, first + kSmallSectionSize, comp);
            tinySTL::unchecked_insertion_sort(first + kSmallSectionSize, last, comp);
        } else {
            tinySTL::insertion_sort(first, last, comp);
        }
    }

    /*
     * sort 重载函数
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void sort(RandomIter first, RandomIter last, Compared comp) {
        if (first != last) {
            // 内省式排序，将区间分为一个个小区间，然后对整体进行插入排序
            tinySTL::intro_sort(first, last, slg2(last - first) * 2, comp);
            tinySTL::final_insertion_sort(first, last, comp);
        }
    }

    // =========================================================================================

    /*
     * nth_element 函数
     * 对序列重排，使得所有小于第 n 个元素的元素出现在它的前面，大于它的出现在它的后面
     */
    template<typename RandomIter>
    void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
        if (nth == last) {
            return;
        }
        while (last - first > 3) {
            RandomIter cut = tinySTL::unchecked_partition(first, last, tinySTL::median(*first,
                                                                                       *(first + (last - first) / 2),
                                                                                       *(last - 1)));
            /* 如果 nth 位于右段,对右段进行分割 */
            if (cut <= nth) {
                first = cut;
            } else {
                /* 对左段进行分割 */
                last = cut;
            }
        }
        tinySTL::insertion_sort(first, last);
    }

    /*
     * nth_element 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename RandomIter, typename Compared>
    void nth_element(RandomIter first, RandomIter nth, RandomIter last, Compared comp) {
        if (nth == last) {
            return;
        }
        while (last - first > 3) {
            RandomIter cut = tinySTL::unchecked_partition(first, last, tinySTL::median(*first,
                                                                                       *(first + (last - first) / 2),
                                                                                       *(last - 1)), comp);
            /* 如果 nth 位于右段,对右段进行分割 */
            if (cut <= nth) {
                first = cut;
            } else {
                /* 对左段进行分割 */
                last = cut;
            }
        }
        tinySTL::insertion_sort(first, last, comp);
    }

    // ================================================================================

    /*
     * unique_copy 函数
     * 从[first, last)中将元素复制到 result 上，序列必须有序，如果有重复的元素，只会复制一次
     */

    /*
     * unique_copy_dispatch 函数
     * forward_iterator_tag 版本
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter
    unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result, tinySTL::forward_iterator_tag) {
        *result = *first;
        while (++first != last) {
            if (*result != *first) {
                *++result = *first;
            }
        }
        return ++result;
    }

    /*
     * unique_copy_dispatch 函数
     * output_iterator_tag 版本
     * 由于 output iterator 只能进行只写操作，所以不能有 *result != *first 这样的判断
     */
    template<typename InputIter, typename OutputIter>
    OutputIter unique_copy_dispatch(InputIter first, InputIter last, OutputIter result, tinySTL::output_iterator_tag) {
        auto value = *first;
        *result = value;
        while (++first != last) {
            if (value != *first) {
                value = *first;
                *++result = value;
            }
        }
        return ++result;
    }

    /*
     * unique_copy 函数
     * 此函数中根据迭代器的类别分发任务
     */
    template<typename InputIter, typename OutputIter>
    OutputIter unique_copy(InputIter first, InputIter last, OutputIter result) {
        if (first == last) {
            return result;
        }
        return tinySTL::unique_copy_dispatch(first, last, result, tinySTL::iterator_category(result));
    }

    /*
     * unique_copy_dispatch 函数重载
     * forward_iterator_tag 版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter, typename ForwardIter, typename Compared>
    ForwardIter unique_copy_dispatch(InputIter first, InputIter last, ForwardIter result,
                                     tinySTL::forward_iterator_tag, Compared comp) {
        *result = *first;
        while (++first != last) {
            if (!comp(*result, *first)) {
                *++result = *first;
            }
        }
        return ++result;
    }

    /*
     * unique_copy_dispatch 函数重载
     * output_iterator_tag 版本
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter, typename OutputIter, typename Compared>
    OutputIter unique_copy_dispatch(InputIter first, InputIter last, OutputIter result,
                                    tinySTL::output_iterator_tag, Compared comp) {
        auto value = *first;
        *result = value;
        while (++first != last) {
            if (!comp(value, *first)) {
                value = *first;
                *++result = value;
            }
        }
        return ++result;
    }

    /*
     * unique_copy 函数
     * 此函数中根据迭代器的类别分发任务
     */
    template<typename InputIter, typename OutputIter, typename Compared>
    OutputIter unique_copy(InputIter first, InputIter last, OutputIter result, Compared comp) {
        if (first == last) {
            return result;
        }
        return tinySTL::unique_copy_dispatch(first, last, result, tinySTL::iterator_category(result), comp);
    }

    // =========================================================================================

    /*
     * unique 函数
     * 移除[first, last)内重复的元素，序列必须有序，和 remove 类似，它也不能真正的删除重复元素
     * 调用unique_copy函数完成实际操作
     */
    template<typename ForwardIter>
    ForwardIter unique(ForwardIter first, ForwardIter last) {
        first = tinySTL::adjacent_find(first, last);
        return tinySTL::unique_copy(first, last, first);
    }

    /*
     * unique 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename ForwardIter, typename Compared>
    ForwardIter unique(ForwardIter first, ForwardIter last, Compared comp) {
        first = tinySTL::adjacent_find(first, last, comp);
        return tinySTL::unique_copy(first, last, first, comp);
    }

} // namepsace tinySTL

#endif //TINYSTL_ALGO_H
