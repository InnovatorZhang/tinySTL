//
// Created by cqupt1811 on 2022/5/7.
//

/*
 * 这个头文件用于对未初始化空间构造元素
 */

#ifndef TINYSTL_UNINITIALIZED_H
#define TINYSTL_UNINITIALIZED_H

#include "algobase.h"  /* 这个头文件包含了库中的一些基本算法 */
#include "construct.h"  /* 这个头文件包含两个函数 construct 负责对象的构造，destroy 负责对象的析构*/
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "type_traits.h"  /* type_traits.h文件钟引入了标准库钟的type_traits文件，同时定义了一些自己的模板方法 */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =============================================================================================

    /*
     * uninitialized_copy函数
     * 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        /* 调用copy函数完成操作即可 */
        return tinySTL::copy(first, last, result);
    }

    /*
     * uninitialized_copy函数
     * 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     * 在此函数中调用构造函数在指定空间上构建对象
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                tinySTL::construct(&*cur, *first);
            }
        }
        catch (...) {
            /* 若操作失败恢复原样 */
            tinySTL::destroy(result, cur);
            return result;
        }
        return cur;
    }

    /*
     * uninitialized_copy函数
     * 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
        return unchecked_uninitialized_copy(first, last, result,
                                            std::is_trivially_copy_assignable<
                                                    typename tinySTL::iterator_traits<ForwardIter>::
                                                    value_type>{});
    }

    // ======================================================================================

    /*
     * uninitialized_copy_n函数
     * 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_copy_n(InputIter first, Size n, ForwardIter result, std::true_type) {
        /* 调用copy_n函数完成操作即可 */
        return tinySTL::copy_n(first, n, result).second;
    }

    /*
     * uninitialized_copy_n函数
     * 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     * 在此函数中调用构造函数在指定空间上构建对象
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_copy_n(InputIter first, Size n, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++cur, ++first) {
                tinySTL::construct(&*cur, *first);
            }
        }
        catch (...) {
            tinySTL::destroy(result, cur);
            return result;
        }
        return cur;
    }

    /*
     * uninitialized_copy_n函数
     * 把把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
        return unchecked_uninitialized_copy_n(first, n, result,
                                              std::is_trivially_copy_assignable<
                                                      typename tinySTL::iterator_traits<InputIter>::
                                                      value_type>{});
    }

    // =============================================================================================

    /*
     * uninitialized_fill
     * 在 [first, last) 区间内填充元素值
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename ForwardIter, typename T>
    void unchecked_uninitialized_fill(ForwardIter first, ForwardIter last, const T &value, std::true_type) {
        /* 调用fill函数完成操作即可 */
        tinySTL::fill(first, last, value);
    }

    /*
     * uninitialized_fill
     * 在 [first, last) 区间内填充元素值
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     * 在此函数中调用构造函数在指定空间上构建对象
     */
    template<typename ForwardIter, typename T>
    void unchecked_uninitialized_fill(ForwardIter first, ForwardIter last, const T &value, std::false_type) {
        auto cur = first;
        try {
            for (; cur != last; ++cur) {
                tinySTL::construct(&*cur, value);
            }
        }
        catch (...) {
            tinySTL::destroy(first, cur);
            return first;
        }
    }

    /*
     * uninitialized_fill函数
     * 在 [first, last) 区间内填充元素值
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename ForwardIter, typename T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T &value) {
        unchecked_uninitialized_fill(first, last, value,
                                     std::is_trivially_copy_assignable<
                                             typename tinySTL::iterator_traits<ForwardIter>::
                                             value_type>{});
    }

    // ===============================================================================================

    /*
     * uninitialized_fill_n函数
     * 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename ForwardIter, typename Size, typename T>
    ForwardIter  // 返回值
    unchecked_uninitialized_fill_n(ForwardIter first, Size n, const T &value, std::true_type) {
        /* 直接调用fill_n函数 */
        return tinySTL::fill_n(first, n, value);
    }

    /*
     * uninitialized_fill_n函数
     * 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename ForwardIter, typename Size, typename T>
    ForwardIter  // 返回值
    unchecked_uninitialized_fill_n(ForwardIter first, Size n, const T &value, std::false_type) {
        auto cur = first;
        try {
            for (; n > 0; --n, ++cur) {
                tinySTL::construct(&*cur, value);
            }
        }
        catch (...) {
            tinySTL::destroy(first, cur);
            return first;
        }
        return cur;
    }

    /*
     * uninitialized_fill_n函数
     * 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename ForwardIter, typename Size, typename T>
    ForwardIter  // 返回值
    uninitialized_fill_n(ForwardIter first, Size n, const T &value) {
        return unchecked_uninitialized_fill_n(first, n, value,
                                              std::is_trivially_copy_assignable<
                                                      typename tinySTL::iterator_traits<ForwardIter>::
                                                      value_type>{});
    }

    // ================================================================================================

    /*
     * uninitialized_move函数
     * 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        /* 调用move函数即可 */
        return tinySTL::move(first, last, result);
    }

    /*
     * uninitialized_move函数
     * 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                tinySTL::construct(&*cur, tinySTL::move(*first));
            }
        }
        catch (...) {
            tinySTL::destroy(result, cur);
            return result;
        }
        return cur;
    }

    /*
     * uninitialized_move函数
     * 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename InputIter, typename ForwardIter>
    ForwardIter  // 返回值
    uninitialized_move(InputIter first, InputIter last, ForwardIter result) {
        return unchecked_uninitialized_move(first, last, result,
                                            std::is_trivially_copy_assignable<
                                                    typename tinySTL::iterator_traits<InputIter>::
                                                    value_type>{});
    }

    // =============================================================================================

    /*
     * uninitialized_move_n函数
     * 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 若copy操作的值满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::true_type) {
        /* 调用move函数即可 */
        return tinySTL::move(first, first + n, result);
    }

    /*
     * uninitialized_move_n函数
     * 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 若copy操作的值不满足is_trivially_copy_assignable条件，跳转到此函数
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++first, ++cur) {
                tinySTL::construct(&*cur, tinySTL::move(*first));
            }
        }
        catch (...) {
            tinySTL::destroy(result, cur);
            return result;
        }
        return cur;
    }

    /*
     * uninitialized_move_n函数
     * 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
     * 根据值类型是否满足is_trivially_copy_assignable条件
     * 调用不同的函数
     */
    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter  // 返回值
    uninitialized_move_n(InputIter first, Size n, ForwardIter result) {
        return unchecked_uninitialized_move_n(first, n, result,
                                              std::is_trivially_copy_assignable<
                                                      typename tinySTL::iterator_traits<InputIter>::
                                                      value_type>{});
    }

}  // namespace tinySTL

#endif //TINYSTL_UNINITIALIZED_H
