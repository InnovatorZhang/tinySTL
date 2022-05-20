//
// Created by cqupt1811 on 2022/5/6.
//

/*
 * 此文件中定义了异常相关的宏
 */

#ifndef TINYSTL_EXCEPTDEF_H
#define TINYSTL_EXCEPTDEF_H

#include <stdexcept>  /* 定义了一些标准的异常类。分为两大类：逻辑错误和运行时错误 */
#include <cassert>  /* 断言 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

#define TINYSTL_DEBUG(expr) \
  assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)

} // namepsace tinySTL

#endif //TINYSTL_EXCEPTDEF_H
