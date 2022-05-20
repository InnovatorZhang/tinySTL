//
// Created by cqupt1811 on 2022/5/10.
//

/*
 * 这个头文件包含 set 的四种算法: union, intersection, difference, symmetric_difference
 * 所有函数都要求序列有序
 */

#ifndef TINYSTL_SET_ALGO_H
#define TINYSTL_SET_ALGO_H

#include "algobase.h"  /* 这个头文件包含了库中的一些基本算法 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =========================================================================================

    /*
     * set_union 函数
     * 计算 S1 ∪ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter>
    OutputIter set_union(InputIter1 first1, InputIter1 last1,
                         InputIter2 first2, InputIter2 last2,
                         OutputIter result) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                *result = *first1;
                ++first1;
            } else if (*first2 < *first1) {
                *result = *first2;
                ++first2;
            } else {
                *result = *first1;
                ++first1, ++first2;
            }
            ++result;
        }
        /* 将剩余元素拷贝到 result */
        return tinySTL::copy(first2, last2, tinySTL::copy(first1, last1, result));
    }

    /*
     * set_union 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename Compared>
    OutputIter set_union(InputIter1 first1, InputIter1 last1,
                         InputIter2 first2, InputIter2 last2,
                         OutputIter result, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                *result = *first1;
                ++first1;
            } else if (comp(*first2, *first1)) {
                *result = *first2;
                ++first2;
            } else {
                *result = *first1;
                ++first1, ++first2;
            }
            ++result;
        }
        /* 将剩余元素拷贝到 result */
        return tinySTL::copy(first2, last2, tinySTL::copy(first1, last1, result));
    }

    // ============================================================================================

    /*
     * set_intersection 函数
     * 计算 S1 ∩ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter>
    OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                InputIter2 first2, InputIter2 last2,
                                OutputIter result) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                ++first1;
            } else if (*first2 < *first1) {
                ++first2;
            } else {
                *result = *first1;
                ++first1, ++first2;
                ++result;
            }
        }
        return result;
    }

    /*
     * set_intersection 函数
     * 计算 S1 ∩ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename Compared>
    OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
                                InputIter2 first2, InputIter2 last2,
                                OutputIter result, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                ++first1;
            } else if (comp(*first2, *first1)) {
                ++first2;
            } else {
                *result = *first1;
                ++first1, ++first2;
                ++result;
            }
        }
        return result;
    }

    // ===========================================================================================

    /*
     * set_difference 函数
     * 计算 S1 - S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter>
    OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                              InputIter2 first2, InputIter2 last2,
                              OutputIter result) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                *result = *first1;
                ++first1;
                ++result;
            } else if (*first2 < *first1) {
                ++first2;
            } else {
                ++first1, ++first2;
            }
        }
        return tinySTL::copy(first1, last1, result);
    }

    /*
     * set_difference 函数重载
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename Compared>
    OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                              InputIter2 first2, InputIter2 last2,
                              OutputIter result, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                *result = *first1;
                ++first1;
                ++result;
            } else if (comp(*first2, *first1)) {
                ++first2;
            } else {
                ++first1, ++first2;
            }
        }
        return tinySTL::copy(first1, last1, result);
    }

    // =======================================================================================

    /*
     * set_symmetric_difference 函数
     * 计算 (S1-S2) ∪ (S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter>
    OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
                                        InputIter2 first2, InputIter2 last2,
                                        OutputIter result) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                *result = *first1;
                ++first1;
                ++result;
            } else if (*first2 < *first1) {
                *result = *first2;
                ++first2;
                ++result;
            } else {
                ++first1, ++first2;
            }
        }
        return tinySTL::copy(first2, last2, tinySTL::copy(first1, last1, result));
    }

    /*
     * set_symmetric_difference 函数
     * 计算 (S1-S2) ∪ (S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
     */
    template<typename InputIter1, typename InputIter2, typename OutputIter, typename Compared>
    OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
                                        InputIter2 first2, InputIter2 last2,
                                        OutputIter result, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                *result = *first1;
                ++first1;
                ++result;
            } else if (comp(*first2, *first1)) {
                *result = *first2;
                ++first2;
                ++result;
            } else {
                ++first1, ++first2;
            }
        }
        return tinySTL::copy(first2, last2, tinySTL::copy(first1, last1, result));
    }

}  // namespace tinySTL

#endif //TINYSTL_SET_ALGO_H
