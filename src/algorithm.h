//
// Created by cqupt1811 on 2022/5/11.
//

/*
 * 本头文件只是对其他头文件的一个集合包装，导入此文件的同时会导入其他头文件
 * 这个头文件包含了 tinySTL 的所有算法，包括基本算法，数值算法，heap 算法，set 算法和其他算法
 */

#ifndef TINYSTL_ALGORITHM_H
#define TINYSTL_ALGORITHM_H

#include "algobase.h"  /* 这个头文件包含了库中的一些基本算法 */
#include "algo.h"  /* 这个头文件包含了 tinySTL 的一系列算法 */
#include "set_algo.h"  /* 这个头文件包含 set 的四种算法: union, intersection, difference, symmetric_difference */
#include "heap_algo.h"  /* 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap */
#include "numeric.h"  /* 这个头文件包含了 tinySTL 的数值算法 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

}  // namespace tinySTL

#endif //TINYSTL_ALGORITHM_H
