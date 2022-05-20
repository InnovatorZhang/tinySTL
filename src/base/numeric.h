//
// Created by cqupt1811 on 2022/5/9.
//

/*
 * 这个头文件包含了 tinySTL 的数值算法
 */

#ifndef TINYSTL_NUMERIC_H
#define TINYSTL_NUMERIC_H

#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */


namespace tinySTL {

    // ======================================================================================

    /*
     * accumulate 累加函数
     */

    /*
     * accumulate 函数
     * 以初值 init 对每个元素进行累加
     */
    template<typename InputIter, typename T>
    T accumulate(InputIter first, InputIter last, T init) {
        for (; first != last; ++first) {
            init += *first;
        }
        return init;
    }

    /*
     * accumulate 函数 重载版本
     * 以初值 init 对每个元素进行二元操作
     */
    template<typename InputIter, typename T, typename BinaryOp>
    T accumulate(InputIter first, InputIter last, T init, BinaryOp binary_op) {
        for (; first != last; ++first) {
            init = binary_op(init, *first);
        }
        return init;
    }

    // ======================================================================================

    /*
     * adjacent_difference 计算相邻元素的差值
     */

    /*
     * adjacent_difference 函数
     * 计算相邻元素的差值，结果保存到以 result 为起始的区间上
     */
    template<typename InputIter, typename OutputIter>
    OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result) {
        if (first == last) {
            return result;
        }
        *result++ = *first;
        auto temp = *first;
        while (++first != last) {
            *result++ = *first - temp;
            temp = *first;
        }
        return result;
    }

    /*
     * adjacent_difference 函数 重载版本
     * 自定义相邻元素的二元操作
     */
    template<typename InputIter, typename OutputIter, typename BinaryOp>
    OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
        if (first == last) {
            return result;
        }
        *result++ = *first;
        auto temp = *first;
        while (++first != last) {
            *result++ = binary_op(*first, temp);
            temp = *first;
        }
        return result;
    }

    // ===========================================================================================

    /*
     * inner_product 内积函数
     */

    /*
     * inner_product 内积
     * 以 init 为初值，计算两个区间的内积
     */
    template<typename InputIter1, typename InputIter2, typename T>
    T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init) {
        for (; first1 != last1; ++first1, ++first2) {
            init += *first1 * *first2;
        }
        return init;
    }

    /*
     * inner_product 内积 重载版本
     * 自定义 operator+ 和 operator*
     */
    template<typename InputIter1, typename InputIter2, typename T, typename BinaryOp1, typename BinaryOp2>
    T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init,
                    BinaryOp1 binary_op1, BinaryOp2 binary_op2) {
        for (; first1 != last1; ++first1, ++first2) {
            init = binary_op1(init, binary_op2(*first1, *first2));
        }
        return init;
    }

    // ===========================================================================================

    /*
     * iota 函数
     * 填充[first, last)，以 value 为初值开始递增
     */
    template<typename ForwardIter, typename T>
    void iota(ForwardIter first, ForwardIter last, T value) {
        while (first != last) {
            *first++ = value;
            ++value;
        }
    }

    // ==============================================================================================

    /*
     * partial_sum 函数
     */

    /*
     * partial_sum 函数
     * 计算局部累计求和，结果保存到以 result 为起始的区间上
     */
    template<typename InputIter, typename OutputIter>
    OutputIter partial_sum(InputIter first, InputIter last, OutputIter result) {
        if (first == last) {
            return result;
        }
        *result = *first;
        auto temp = *first;
        while (++first != last) {
            temp = temp + *first;
            *++result = temp;
        }
        return ++result;
    }

    /*
     * partial_sum 函数
     * 计算局部累计求和，结果保存到以 result 为起始的区间上
     */
    template<typename InputIter, typename OutputIter, typename BinaryOp>
    OutputIter partial_sum(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
        if (first == last) {
            return result;
        }
        *result = *first;
        auto temp = *first;
        while (++first != last) {
            temp = binary_op(temp, *first);
            *++result = temp;
        }
        return ++result;
    }

}  // namepsace tinySTL

#endif //TINYSTL_NUMERIC_H
