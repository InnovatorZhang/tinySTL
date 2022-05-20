//
// Created by cqupt1811 on 2022/5/8.
//

/*
 * 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap
 */

#ifndef TINYSTL_HEAP_ALGO_H
#define TINYSTL_HEAP_ALGO_H

#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =================================================================================================

    /*
     * push_heap
     * 将元素加入堆中
     * 使用 < 操作符作为比较方式，所以为大根堆
     * 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
     */

    /*
     * push_heap_aux 函数
     * 使用 < 操作符作为比较方式，所以为大根堆
     * 对尾新加入的节点执行上溯操作，即和父节点比大小，决定是否交换
     * 必须要random访问的迭代器
     * first堆空间的的开始位置
     * holeIndex元素插入位置
     * topIndex堆顶位置
     * value插入元素的值
     */
    template<typename RandomIter, typename Distance, typename T>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
        /* 找到父节点 */
        auto parent = (holeIndex - 1) / 2;
        /* 当到达根节点或者父节点不小于value的时候退出循环 */
        while (holeIndex > topIndex && *(first + parent) < value) {
            /* 节点上移至父节点 */
            *(first + holeIndex) = *(first + parent);
            /* 将父节点设置为当前节点 */
            holeIndex = parent;
            /* 找到当前节点的父节点 */
            parent = (holeIndex - 1) / 2;
        }
        /* 最终位置赋值为value */
        *(first + holeIndex) = value;
    }

    /*
     * push_head_d 函数
     * 在此函数中调用push_heap_aux执行真正的上溯操作
     */
    template<typename RandomIter, typename Distance>
    void push_head_d(RandomIter first, RandomIter last, Distance *) {
        tinySTL::push_heap_aux(first, (last - first - 1), static_cast<Distance>(0), *(last - 1));
    }

    /*
     * push_heap 函数
     * 暴露给外部的接口
     * 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
     */
    template<typename RandomIter>
    void push_heap(RandomIter first, RandomIter last) {
        tinySTL::push_head_d(first, last, tinySTL::distance_type(first));
    }

    /*
     * push_heap_aux 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Distance, typename T, typename Compared>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value, Compared comp) {
        /* 找到父节点 */
        auto parent = (holeIndex - 1) / 2;
        /* 当到达根节点或者父节点不小于value的时候退出循环 */
        while (holeIndex > topIndex && comp(*(first + parent), value)) {
            /* 节点上移至父节点 */
            *(first + holeIndex) = *(first + parent);
            /* 将父节点设置为当前节点 */
            holeIndex = parent;
            /* 找到当前节点的父节点 */
            parent = (holeIndex - 1) / 2;
        }
        /* 最终位置赋值为value */
        *(first + holeIndex) = value;
    }

    /*
     * push_head_d 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Distance, typename Compared>
    void push_head_d(RandomIter first, RandomIter last, Distance *, Compared comp) {
        tinySTL::push_heap_aux(first, (last - first - 1), static_cast<Distance>(0), *(last - 1), comp);
    }

    /*
     * push_heap 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Compared>
    void push_heap(RandomIter first, RandomIter last, Compared comp) {
        tinySTL::push_head_d(first, last, tinySTL::distance_type(first), comp);
    }

    // =============================================================================================

    /*
     * pop_heap
     * 使用 < 操作符作为比较方式，所以为大根堆
     * 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
     */

    /*
     * adjust_heap函数
     * 堆指定的节点先进行下溯，在进行上溯操作
     * first堆空间的的开始位置
     * holeIndex进行调整的位置
     * len堆的长度
     * value为holeIndex位置对应元素的值
     */
    template<typename RandomIter, typename Distance, typename T>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
        /* 从指定节点进行下溯操作 */
        auto topIndex = holeIndex;
        /* 获取当前节点右子节点的坐标 */
        auto rchild = 2 * holeIndex + 2;
        /* 若当前节点没有右子节点，退出循环 */
        while (rchild < len) {
            /* 比较左右子节点的大小，选择较大值作为下一个holeIndex */
            if (*(first + rchild) < *(first + rchild - 1)) {
                --rchild;
            }
            /* 将较大子节点的置赋给当前节点 */
            *(first + holeIndex) = *(first + rchild);
            /* 将较大子节点设置为新的holeIndex */
            holeIndex = rchild;
            /* 找到新的holeIndex的右子节点 */
            rchild = 2 * rchild + 2;
        }
        /* 若只有左子节点 */
        if (rchild == len) {
            /* 将holeIndex节点与左子节点交换 */
            *(first + holeIndex) = *(first + rchild - 1);
            /* 将左子节点设置为新的holeIndex */
            holeIndex = rchild - 1;
        }
        /* 再执行一次上溯过程 */
        tinySTL::push_heap_aux(first, holeIndex, topIndex, value);
    }

    /*
     * pop_heap_aux 函数
     * 在此函数中调用adjust_heap执行真正的调整操作
     */
    template<typename RandomIter, typename Distance, typename T>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance *) {
        /* 先将首值调至尾节点，然后调整[first, last - 1)使之重新成为一个 max-heap */
        *result = *first;
        tinySTL::adjust_heap(first, static_cast<Distance>(0), last - first, value);
    }

    /*
     * push_heap 函数
     * 暴露给外部的接口
     * 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
     */
    template<typename RandomIter>
    void pop_heap(RandomIter first, RandomIter last) {
        /* 这里通过last-1将堆大小减小了一个 */
        tinySTL::pop_heap_aux(first, last - 1, last - 1, *(last - 1), tinySTL::distance_type(first));
    }

    /*
     * adjust_heap 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Distance, typename T, typename Compared>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compared comp) {
        /* 从指定节点进行下溯操作 */
        auto topIndex = holeIndex;
        /* 获取当前节点右子节点的坐标 */
        auto rchild = 2 * holeIndex + 2;
        /* 若当前节点没有右子节点，退出循环 */
        while (rchild < len) {
            /* 比较左右子节点的大小，选择较大值作为下一个holeIndex */
            if (comp(*(first + rchild), *(first + rchild - 1))) {
                --rchild;
            }
            /* 将较大子节点的置赋给当前节点 */
            *(first + holeIndex) = *(first + rchild);
            /* 将较大子节点设置为新的holeIndex */
            holeIndex = rchild;
            /* 找到新的holeIndex的右子节点 */
            rchild = 2 * rchild + 2;
        }
        /* 若只有左子节点 */
        if (rchild == len) {
            /* 将holeIndex节点与左子节点交换 */
            *(first + holeIndex) = *(first + rchild - 1);
            /* 将左子节点设置为新的holeIndex */
            holeIndex = rchild - 1;
        }
        /* 再执行一次上溯过程 */
        tinySTL::push_heap_aux(first, holeIndex, topIndex, value, comp);
    }

    /*
     * pop_heap_aux 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Distance, typename T, typename Compared>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance *, Compared comp) {
        /* 先将首值调至尾节点，然后调整[first, last - 1)使之重新成为一个 max-heap */
        *result = *first;
        tinySTL::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
    }

    /*
     * push_heap 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Compared>
    void pop_heap(RandomIter first, RandomIter last, Compared comp) {
        /* 这里通过last-1将堆大小减小了一个 */
        tinySTL::pop_heap_aux(first, last - 1, last - 1, *(last - 1), tinySTL::distance_type(first), comp);
    }

    // ===========================================================================================

    /*
     * sort_heap
     * 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
     */

    /*
     * sort_heap函数
     * 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
     * 通过不断调用pop_heap函数达到排序的效果，因为pop_heap函数会不断地将堆顶放到尾部
     */
    template<typename RandomIter>
    void sort_heap(RandomIter first, RandomIter last) {
        /* 每执行一次 pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序 */
        while (last - first > 1) {
            tinySTL::pop_heap(first, last--);
        }
    }

    /*
     * sort_heap 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Compared>
    void sort_heap(RandomIter first, RandomIter last, Compared comp) {
        /* 每执行一次 pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序 */
        while (last - first > 1) {
            tinySTL::pop_heap(first, last--, comp);
        }
    }

    // =========================================================================================

    /*
     * make_heap
     * 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
     */

    /*
     * make_heap_aux 函数
     * first区间头部
     * last区间尾部
     * Distance距离类别
     */
    template<typename RandomIter, typename Distance>
    void make_heap_aux(RandomIter first, RandomIter last, Distance *) {
        if (last - first < 2) {
            return;
        }
        Distance len = last - first;
        /* 找出需要重排的第一个子树的头部 */
        Distance holeIndex = (len - 2) / 2;
        /* 循环式逆序一个个的调整 */
        while (true) {
            /* 重排以 holeIndex 为首的子树 */
            tinySTL::adjust_heap(first, holeIndex, len, *(first + holeIndex));
            if (holeIndex == 0) {
                return;
            }
            holeIndex--;
        }
    }

    /*
     * make_heap 函数
     * 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
     */
    template<typename RandomIter>
    void make_heap(RandomIter first, RandomIter last) {
        tinySTL::make_heap_aux(first, last, tinySTL::distance_type(first));
    }

    /*
     * make_heap_aux 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Distance, typename Compared>
    void make_heap_aux(RandomIter first, RandomIter last, Distance *, Compared comp) {
        if (last - first < 2) {
            return;
        }
        Distance len = last - first;
        /* 找出需要重排的第一个子树的头部 */
        Distance holeIndex = (len - 2) / 2;
        /* 循环式逆序一个个的调整 */
        while (true) {
            /* 重排以 holeIndex 为首的子树 */
            tinySTL::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
            if (holeIndex == 0) {
                return;
            }
            holeIndex--;
        }
    }

    /*
     * make_heap 函数的Comp重载
     * 使用Comp进行比较操作
     */
    template<typename RandomIter, typename Compared>
    void make_heap(RandomIter first, RandomIter last, Compared comp) {
        tinySTL::make_heap_aux(first, last, tinySTL::distance_type(first), comp);
    }

} // namepsace tinySTL

#endif //TINYSTL_HEAP_ALGO_H
