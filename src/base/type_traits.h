//
// Created by cqupt1811 on 2022/5/3.
//

/*
 * type_traits.h文件钟引入了标准库钟的type_traits文件，同时定义了一些自己的模板方法
 */
#ifndef TINYSTL_TYPE_TRAITS_H
#define TINYSTL_TYPE_TRAITS_H

#include <type_traits>
/*
 * https://blog.csdn.net/fengbingchun/article/details/115427439
 * C++11中的头文件type_traits定义了一系列模板类，在编译期获得某一参数、某一变量、某一个类等等类型信息，主要做静态检查。
 * 此头文件包含三部分：
 * Helper类：帮助创建编译时常量的标准模板类。
 * 类型特征(Type traits)：以编译时常量值的形式获取类型特征(characteristics of types)的模板类。
 * 类型转换(Type transformations)：通过对现有类型进行特定的转换来获取新类型的模板类。
 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 创建帮助编译时常量的标准模板类
     * 定义真假信息的类别
     * 而m_true_type类型代表的就是true，m_false_type类型代表的就是false.
     * 仿造std::true_type写的
     */
    template<typename T, T v>
    struct m_integral_constant {
        static constexpr T value = v;
    };

    /*
     * using语法的用法
     */
    template<bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;


    /*
     * 使用模板偏特化技巧赖判断泛型是否为pair的辅助模板
     */
    // 前向声明pair模板类
    template<typename T1, typename T2>
    struct pair;

    // 如果匹配到此模板，说明类型T不是pair,此函数通过tinySTL::ispair<T>调用
    template<typename T>
    struct is_pair : tinySTL::m_false_type {

    };

    // 如果匹配到此模板，说明类型是pair，此函数通过tinySTL::ispair<T>调用
    template<typename T1, typename T2>
    struct is_pair<tinySTL::pair<T1, T2>> : tinySTL::m_true_type {

    };

}  // namespace tinySTL

#endif //TINYSTL_TYPE_TRAITS_H
