//
// Created by cqupt1811 on 2022/5/17.
//

/*
 * 定义了 string, wstring, u16string, u32string 类型
 */

#ifndef TINYSTL_ASTRING_H
#define TINYSTL_ASTRING_H

#include "basic_string.h"  /* 这个头文件包含一个模板类 basic_string,用于表示字符串类型 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 为string类定义一系列别名
     */
    using string = tinySTL::basic_string<char>;
    using wstring = tinySTL::basic_string<wchar_t>;
    using u16string = tinySTL::basic_string<char16_t>;
    using u32string = tinySTL::basic_string<char32_t>;

}  // namespace tinySTL

#endif //TINYSTL_ASTRING_H
