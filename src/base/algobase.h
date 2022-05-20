//
// Created by cqupt1811 on 2022/5/6.
//

/*
 * 这个头文件包含了库中的一些基本算法
 */

#ifndef TINYSTL_ALGOBASE_H
#define TINYSTL_ALGOBASE_H

#include <cstring>  /* std::memmove 函数 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

/*
 * 取消编译器中定义好的min与max，因为下面自己要定义
 */
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif

    // =============================================================================================

    /*
     * 使用模板实现一些基本的算法
     */

    /*
     * max函数，返回两者中大值
     */
    template<typename T>
    const T &max(const T &lhs, const T &rhs) {
        return lhs < rhs ? rhs : lhs;
    }

    /*
     * max函数的重载版本，使用Comp代替比较操作
     */
    template<typename T, typename Compare>
    const T &max(const T &lhs, const T &rhs, Compare comp) {
        return comp(lhs, rhs) ? rhs : lhs;
    }

    /*
     * min函数，返回两者中小值
     */
    template<typename T>
    const T &min(const T &lhs, const T &rhs) {
        return lhs < rhs ? lhs : rhs;
    }

    /*
     * min函数的重载版本，使用Comp代替比较操作
     */
    template<typename T, typename Compare>
    const T &min(const T &lhs, const T &rhs, Compare comp) {
        return comp(lhs, rhs) ? lhs : rhs;
    }

    /*
     * 将两个迭代器指向的对象对调
     */
    template<typename FIter1, typename FIter2>
    void iter_swap(FIter1 lhs, FIter2 rhs) {
        tinySTL::swap(*lhs, *rhs);
    }

    // =============================================================================================

    /*
     * copy函数
     * 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
     * input_iterator_tag 版本,迭代器只能单向移动
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
                       tinySTL::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            /* 赋值 */
            *result = *first;
        }
        return result;
    }

    /*
     * copy函数
     * 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
     * input_iterator_tag 版本,迭代器只能单向移动
     */
    template<typename RandomIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
                       tinySTL::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = *first;
        }
        return result;
    }

    /*
     * copy函数
     * 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
     * 此函数负责根据迭代器的不同类别将任务分发给不同的函数
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_copy(InputIter first, InputIter last, OutputIter result) {
        return unchecked_copy_cat(first, last, result, tinySTL::iterator_category(first));
    }

    /*
     * copy函数
     * 为 trivially_copy_assignable 类型提供特化版本
     * 如果Up是trivial拷贝的，那么调用 std::memmove 函数会快很多
     */
    template<class Tp, class Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&  // Up与Tp去除const修饰后的类型是否相同
            std::is_trivially_copy_assignable<Up>::value,  // 是否是trivial拷贝的
            Up *>::type  // 返回值，如果enable_if第一个参数为true的话，::type为第二个参数
    unchecked_copy(Tp *first, Tp *last, Up *result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return result + n;
    }

    /*
     * copy函数
     * 此函数中调用 unchecked_copy 函数执行真正的操作
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    copy(InputIter first, InputIter last, OutputIter result) {
        return unchecked_copy(first, last, result);
    }

    // =============================================================================================

    /*
     * 反向copy函数
     * 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
     * 因为是反向的复制，所以没有forward_iterator的版本
     * bidirectional_iterator_tag 版本
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, tinySTL::bidirectional_iterator_tag) {
        while (first != last) {
            *--result = *--last;
        }
        return result;
    }

    /*
     * 反向copy函数
     * 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
     * 因为是反向的复制，所以没有forward_iterator的版本
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter1, typename RandomIter2>
    RandomIter2  // 返回值
    unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last,
                                RandomIter2 result, tinySTL::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *--result = *--last;
        }
        return result;
    }

    /*
     * 反向copy函数
     * 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
     * 此函数负责根据迭代器的不同类别将任务分发给不同的函数
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result) {
        /* 此函数负责根据迭代器的不同类别将任务分发给不同的函数 */
        return unchecked_copy_backward_cat(first, last, result, tinySTL::iterator_category(first));
    }

    /*
     * 反向copy函数
     * 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
     * trivially_copy_assignable 类型提供特化版本
     * 如果Up是trivial拷贝的，那么调用 std::memmove 函数会快很多
     */
    template<typename Tp, typename Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&  // Up与Tp去除const修饰后的类型是否相同
            std::is_trivially_copy_assignable<Up>::value,  // 是否是trivial拷贝的
            Up *>::type  // 返回值，如果enable_if第一个参数为true的话，::type为第二个参数
    unchecked_copy_backward(Tp *first, Tp *last, Up *result) {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    /*
     * 反向copy函数
     * 此函数中调用 unchecked_copy_backward 函数执行真正的操作
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return unchecked_copy_backward(first, last, result);
    }

    // ==========================================================================================

    /*
     * copy_if函数
     * 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
     */
    template<typename InputIter, typename OutputIter, typename UnaryPredicate>
    OutputIter  // 返回值
    copy_if(InputIter first, InputIter last, OutputIter result,
            UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) {
                *result++ = *first;
            }
        }
        return result;
    }

    /*
     * copy_n
     * 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
     * 返回一个 pair 分别指向拷贝结束后的尾部
     * input_iterator_tag 版本
     */
    template<typename InputIter, typename Size, typename OutputIter>
    tinySTL::pair<InputIter, OutputIter>  // 返回值
    unchecked_copy_n(InputIter first, Size n, OutputIter result, tinySTL::input_iterator_tag) {
        for (; n > 0; --n, ++first, ++result) {
            *result = *first;
        }
        /* 返回一个pair对象 */
        return tinySTL::pair<InputIter, OutputIter>(first, result);
    }

    /*
     * copy_n
     * 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
     * 返回一个 pair 分别指向拷贝结束后的尾部
     * random_access_iterator_tag 版本
     */
    template<typename RandomIter, typename Size, typename OutputIter>
    tinySTL::pair<RandomIter, OutputIter>  // 返回值
    unchecked_copy_n(RandomIter first, Size n, OutputIter result,
                     tinySTL::random_access_iterator_tag) {
        /* 移动指针找到结束位置，再调用copy函数执行操作 */
        auto last = first + n;
        return tinySTL::pair<RandomIter, OutputIter>(last, tinySTL::copy(first, last, result));
    }

    /*
     * copy_n
     * 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
     * 返回一个 pair 分别指向拷贝结束后的尾部
     * 此函数中调用 unchecked_copy_n 函数执行真正的操作,根据迭代器的不同类别将任务分发给不同的函数
     */
    template<typename InputIter, typename Size, typename OutputIter>
    tinySTL::pair<InputIter, OutputIter>  // 返回值
    copy_n(InputIter first, Size n, OutputIter result) {
        return unchecked_copy_n(first, n, result, tinySTL::iterator_category(first));
    }

    // ==========================================================================================

    /*
     * move函数
     * 把 [first, last)区间内的元素 移动(右值) 到 [result, result + (last - first))内
     * input_iterator_tag 版本
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                       tinySTL::input_iterator_tag) {
        for (; first != last; ++first, ++result) {
            *result = tinySTL::move(*first);
        }
        return result;
    }

    /*
     * move函数
     * 把 [first, last)区间内的元素 移动(右值) 到 [result, result + (last - first))内
     * ramdom_access_iterator_tag 版本
     */
    template<typename RandomIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
                       tinySTL::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n, ++first, ++result) {
            *result = tinySTL::move(*first);
        }
        return result;
    }

    /*
     * move函数
     * 把 [first, last)区间内的元素 移动(右值) 到 [result, result + (last - first))内
     * 在此函数中根据迭代器种类分发任务给对应函数
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    unchecked_move(InputIter first, InputIter last, OutputIter result) {
        /* 此函数负责根据迭代器的不同类别将任务分发给不同的函数 */
        return unchecked_move_cat(first, last, result, tinySTL::iterator_category(first));
    }

    /*
     * move函数
     * 把 [first, last)区间内的元素 移动(右值) 到 [result, result + (last - first))内
     * trivially_copy_assignable 类型提供特化版本
     * 如果Up是trivial移动赋值的，那么调用 std::memmove 函数会快很多
     */
    template<typename Tp, typename Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&  // Up与Tp去除const修饰后的类型是否相同
            std::is_trivially_move_assignable<Up>::value,  // Up是否是trivial移动赋值的
            Up *>::type  // 返回值，如果enable_if第一个参数为true的话，::type为第二个参数
    unchecked_move(Tp *first, Tp *last, Up *result) {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0) {
            std::memmove(result, first, n * sizeof(Up));
        }
        return result + n;
    }

    /*
     * move函数
     * 此函数中调用 unchecked_move 函数执行真正的操作
     */
    template<typename InputIter, typename OutputIter>
    OutputIter  // 返回值
    move(InputIter first, InputIter last, OutputIter result) {
        return unchecked_move(first, last, result);
    }

    // ==========================================================================================

    /*
     * move_backward函数
     * 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
     * bidirectional_iterator_tag 版本
     * 因为需要反向移动迭代器，所以没有 input_iterator_tag 的版本
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, tinySTL::bidirectional_iterator_tag) {
        while (first != last) {
            *--result = tinySTL::move(*--last);
        }
        return result;
    }

    /*
     * move_backward函数
     * 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
     * bidirectional_iterator_tag 版本
     * 因为需要反向移动迭代器，所以没有 input_iterator_tag 的版本
     */
    template<typename RandomIter1, typename RandomIter2>
    RandomIter2  // 返回值
    unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
                                RandomIter2 result, tinySTL::random_access_iterator_tag) {
        for (auto n = last - first; n > 0; --n) {
            *--result = tinySTL::move(*--last);
        }
        return result;
    }

    /*
     * move_backward函数
     * 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
     * 在此函数中根据迭代器种类分发任务给对应函数
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return unchecked_move_backward_cat(first, last, result, tinySTL::iterator_category(first));
    }

    /*
     * move_backward函数
     * 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
     * trivially_copy_assignable 类型提供特化版本
     * 如果Up是trivial移动赋值的，那么调用 std::memmove 函数会快很多
     */
    template<typename Tp, typename Up>
    typename std::enable_if<
            std::is_same<typename std::remove_const<Tp>::type, Up>::value &&  // Up与Tp去除const修饰后的类型是否相同
            std::is_trivially_move_assignable<Up>::value,  // Up是否是trivial移动赋值的
            Up *>::type  // 返回值，如果enable_if第一个参数为true的话，::type为第二个参数
    unchecked_move_backward(Tp *first, Tp *last, Up *result) {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0) {
            /* 移动result指针到指定位置后再带哦用库函数 */
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    /*
     * move_backward函数
     * 此函数中调用 unchecked_move_backward 函数执行真正的操作
     */
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2  // 返回值
    move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
        return unchecked_move_backward(first, last, result);
    }

    // =============================================================================================

    /*
     * equal函数
     * 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
     */
    template<typename InputIter1, typename InputIter2>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            if (*first1 != *first2) {
                return false;
            }
        }
        return true;
    }

    /*
     * equal函数
     * 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
     * 重载版本使用函数对象 comp 代替比较操作
     */
    template<typename InputIter1, typename InputIter2, typename Compared>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2,
               Compared comp) {
        for (; first1 != last1; ++first1, ++first2) {
            if (!comp(*first1, *first2)) {
                return false;
            }
        }
        return true;
    }

    // ============================================================================================

    /*
     * fill_n函数
     * 从 first 位置开始填充 n 个指定值
     */
    template<typename OutputIter, typename Size, typename T>
    OutputIter  // 返回值
    unchecked_fill_n(OutputIter first, Size n, const T &value) {
        for (; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }

    /*
     * fill_n函数
     * 从 first 位置开始填充 n 个指定值
     * 为 one-byte 类型提供特化版本
     * 若Tp与Up大小为一个字节，且不为布尔类型，则调用此函数，
     * 使用库函数 memset 执行操作效率更高
     */
    template<typename Tp, typename Size, typename Up>
    typename std::enable_if<
            std::is_integral<Tp>::value && sizeof(Tp) == 1 &&  // 判断Tp是否是整形，且大小是否为1个字节
            !std::is_same<Tp, bool>::value &&  // Tp是否为布尔类型
            std::is_integral<Up>::value && sizeof(Up) == 1,  // 判断Up是否是整形，且大小是否为1个字节
            Tp *>::type  // 返回值，如果enable_if第一个参数为true的话，::type为第二个参数
    unchecked_fill_n(Tp *first, Size n, Up value) {
        if (n > 0) {
            std::memset(first, (unsigned char) value, (size_t) (n));
        }
        return first + n;
    }

    /*
     * fill_n函数
     * 从 first 位置开始填充 n 个指定值
     * 此函数中调用 unchecked_fill_n 函数执行真正的操作
     */
    template<typename OutputIter, typename Size, typename T>
    OutputIter  // 返回值
    fill_n(OutputIter first, Size n, const T &value) {
        return unchecked_fill_n(first, n, value);
    }

    // ============================================================================================

    /*
     * fill函数
     * 为 [first, last)区间内的所有元素填充新值
     * forward_iterator_tag 版本
     */
    template<typename ForwardIter, typename T>
    void fill_cat(ForwardIter first, ForwardIter last, const T &value,
                  tinySTL::forward_iterator_tag) {
        for (; first != last; ++first) {
            *first = value;
        }
    }

    /*
     * fill函数
     * 为 [first, last)区间内的所有元素填充新值
     * random_access_iterator_tag 版本
     * 调用fill_n完成任务
     */
    template<typename RandomIter, typename T>
    void fill_cat(RandomIter first, RandomIter last, const T &value,
                  tinySTL::random_access_iterator_tag) {
        fill_n(first, last - first, value);
    }

    /*
     * fill函数
     * 为 [first, last)区间内的所有元素填充新值
     * 在此函数中调用对应的函数完成任务
     */
    template<typename ForwardIter, typename T>
    void fill(ForwardIter first, ForwardIter last, const T &value) {
        fill_cat(first, last, value, tinySTL::iterator_category(first));
    }

    // ===========================================================================================

    /*
     * lexicographical_compare函数
     * 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
     * (1)如果第一序列的元素较小，返回 true ，否则返回 false
     * (2)如果到达 last1 而尚未到达 last2 返回 true
     * (3)如果到达 last2 而尚未到达 last1 返回 false
     * (4)如果同时到达 last1 和 last2 返回 false
     */
    template<typename InputIter1, typename InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (*first1 < *first2) {
                return true;
            }
            if (*first2 < *first1) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }

    /*
     * lexicographical_compare函数
     * 重载版本使用函数对象 comp 代替比较操作
     * 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
     * (1)如果第一序列的元素较小，返回 true ，否则返回 false
     * (2)如果到达 last1 而尚未到达 last2 返回 true
     * (3)如果到达 last2 而尚未到达 last1 返回 false
     * (4)如果同时到达 last1 和 last2 返回 false
     */
    template<typename InputIter1, typename InputIter2, typename Compared>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2, Compared comp) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (comp(*first1, *first2)) {
                return true;
            }
            if (comp(*first2, *first1)) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }

    /*
     * lexicographical_compare函数
     * 针对 const unsigned char* 的特化版本
     * 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
     * (1)如果第一序列的元素较小，返回 true ，否则返回 false
     * (2)如果到达 last1 而尚未到达 last2 返回 true
     * (3)如果到达 last2 而尚未到达 last1 返回 false
     * (4)如果同时到达 last1 和 last2 返回 false
     */
    bool lexicographical_compare(const unsigned char *first1, const unsigned char *last1,
                                 const unsigned char *first2, const unsigned char *last2) {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;

        /* 调用 std::memcmp 比较相同部分 */
        const auto result = std::memcmp(first1, first2, tinySTL::min(len1, len2));
        /* 若长度相同部分相等，则长度较大的比较大 */
        return result != 0 ? result < 0 : len1 < len2;
    }

    // =========================================================================================

    /*
     * mismatch函数
     * 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
     */
    template<typename InputIter1, typename InputIter2>
    tinySTL::pair<InputIter1, InputIter2>  // 返回值
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
        while (first1 != last1 && *first1 == *first2) {
            ++first1;
            ++first2;
        }
        return tinySTL::pair<InputIter1, InputIter2>(first1, first2);
    }

    /*
     * mismatch函数
     * 重载版本使用函数对象 comp 代替比较操作
     * 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
     */
    template<typename InputIter1, typename InputIter2, typename Compared>
    tinySTL::pair<InputIter1, InputIter2>  // 返回值
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp) {
        while (first1 != last1 && comp(*first1, *first2)) {
            ++first1;
            ++first2;
        }
        return tinySTL::pair<InputIter1, InputIter2>(first1, first2);
    }

}  // namespace tinySTL


#endif //TINYSTL_ALGOBASE_H
