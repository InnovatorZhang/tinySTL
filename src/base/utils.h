//
// Created by cqupt1811 on 2022/5/3.
//

/*
 * 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等
 */

#ifndef TINYSTL_UTILS_H
#define TINYSTL_UTILS_H

#include <cstddef>  /* 头文件cstddef与其C对应版本兼容，它是C头文件<stddef.h>较新版本，定义了常用的常量、宏、类型和函数 */
#include "type_traits.h"  /* type_traits.h文件钟引入了标准库钟的type_traits文件，同时定义了一些自己的模板方法 */

/* 首先定义自己的命名空间 */
namespace tinySTL {
    // move,forward===================================================================================================
    /*
     * std::move与std::forward原理 https://blog.csdn.net/qq_33850438/article/details/107193177
     */

    /*
     * 此函数将传入的参数转换为右值
     * 首先用typename std::remove_reference<T>::type提取出类型，remove_reference的功能是去掉引用类型修饰符如&引用符号与&&右值符号，使用到的技巧是偏特化
     * 使用static_cast转型为对应类型的右值
     */
    template<typename T>
    // 定义模板参数
    typename std::remove_reference<T>::type &&  // 返回值为右值
    move(T &&arg) noexcept  // 右值接受参数，会发生引用折叠
    {
        return static_cast<typename std::remove_reference<T>::type &&>(arg);
    }


    /* forward提供两个重载版本， 一个针对左值， 一个针对右值。 */

    /*
     * forward函数的实现
     * 利用函数重载实现的两个不同函数
     * 参数列表为左值引用的实现，即接受T&，转发后为左值
     */
    template<typename T>
    T &&                  // 返回值处会发生引用折叠，返回值类型为引用
    forward(typename std::remove_reference<T>::type &arg) noexcept  // 接受的参数类型为左值引用
    {
        return static_cast<T &&>(arg);  // 引用折叠也会发生在T &&处，所以会转型为左值引用类型
    }

    /*
     * forward函数的实现
     * 利用函数重载实现的两个不同函数
     * 参数列表为右值的实现，即接受T&&,转发后为右值
     */
    template<typename T>
    T &&                  // 返回值处会发生引用折叠，返回值类型为引用
    forward(typename std::remove_reference<T>::type &&arg) noexcept // 接受的参数类型为右值引用
    {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");  // 检查是否是右值
        return static_cast<T &&>(arg);  // 引用折叠也会发生在T &&处，所以会转型为右值引用类型
    }

    // swap===================================================================================================

    /*
     * 调用移动赋值函数进行交换
     */
    template<typename Tp>
    void swap(Tp &lhs, Tp &rhs) {
        auto temp = tinySTL::move(lhs);
        lhs = tinySTL::move(rhs);
        rhs = tinySTL::move(temp);
    }

    /*
     * 交换first1->last1迭代器范围内与first2迭代器后的元素
     * 迭代器类型要求至少为Forward类型
     */
    template<typename ForwardIter1, typename ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            tinySTL::swap(*first1, *first2);
        }
        return first2;
    }

    /*
     * 包装的上一个函数
     * Tp(&a)这个写法没看懂，应该是接收数组作为参数，拿到数组的起始指针
     * https://vinkle.top/2020/10/12/cpp-template-5/#5-4-%E5%8E%9F%E5%A7%8B%E6%95%B0%E7%BB%84%E5%92%8C%E5%AD%97%E7%AC%A6%E4%B8%B2%E5%AD%97%E9%9D%A2%E9%87%8F%E6%A8%A1%E6%9D%BF
     * 这种写法是为了解决泛型参数推导的问题
     */
    template<typename Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) {
        tinySTL::swap_range(a, a + N, b);
    }


    // pair===================================================================================================

    /*
     * 结构体模板 : pair
     * 两个模板参数分别表示两个数据的类型
     * 用 first 和 second 来分别取出第一个数据和第二个数据
     */

    template<typename Ty1, typename Ty2>  // 类模板参数
    struct pair {
        /* 首先定义别名 */
        typedef Ty1 first_type;
        typedef Ty2 second_type;

        /* 声明成员变量 */
        first_type first;
        second_type second;

        /*
         * pair类的默认构造函数，使用类成员模板函数,Other1与Other2为类模板参数
         * 初始化列表中初始化成员变量
         * constexpr构造函数，这样声明以后，就可以在使用constexpr表达式或者constexpr函数的地方使用字面值常量类了。
         * std::enable_if的用法  https://vinkle.top/2021/01/15/cpp-template-6/#6-3-%E4%BD%BF%E7%94%A8-enable-if-lt-gt-%E7%A6%81%E7%94%A8%E6%A8%A1%E6%9D%BF
         * 第三个模板参数会使用 is_default_constructible 检查第一参数钟对应两个类型是否有默认构造函数
         * enable_if的第一个参数作用是检查是否满足条件，如果不满足条件则会建立一个编译期错误，满足条件且有第二个参数的话会返回第二个参数指定的类型
         * typename = typename...这种写法表示匿名类型，因为只是做了类型检查
         */
        template<typename Other1 = Ty1, typename Other2 = Ty2,
                typename = typename std::enable_if<
                        std::is_default_constructible<Other1>::value &&  // 检查Other1类型是否有默认构造函数
                        std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair():first(), second() {

        }

        /*
         * 隐式构造函数
         * implicit constructible for this type
         * 这里采用的是 typename std::enable_if<...,int>::type = 0，这种写法
         * 其实也是匿名写法，表示enable_if第一个参数为true时type类型为int
         * 同template<int = 0>，表示泛型中匿名默认常量参数
         * std::is_convertible<A,B>检查类型间是否可以隐式转换,由A到B，可以转换就返回true
         */
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename std::enable_if<
                        std::is_copy_constructible<U1>::value &&  // 检查U1类型是否有拷贝构造函数
                        std::is_copy_constructible<U2>::value &&
                        std::is_convertible<const U1 &, Ty1>::value &&  // 检查U1&类型是否可以隐式转化为Ty1类型
                        std::is_convertible<const U2 &, Ty2>::value, int>::type = 0>
        constexpr pair(const Ty1 &a, const Ty2 &b):first(a), second(b) {

        }

        /*
         * explicit constructible for this type
         * 作用和上面函数一样
         * 但不允许隐式构造
         */
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename std::enable_if<
                        std::is_copy_constructible<U1>::value &&
                        std::is_copy_constructible<U2>::value &&
                        (!std::is_convertible<const U1 &, Ty1>::value ||
                         !std::is_convertible<const U2 &, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const Ty1 &a, const Ty2 &b): first(a), second(b) {

        }

        /*
         * 由编译器自动生成拷贝构造函数
         */
        pair(const pair &rhs) = default;

        /*
         * 由编译器自动生成移动构造函数
         */
        pair(pair &&rhs) = default;

        /*
         * implicit constructiable for other type
         * 由其他类型参数构造first与second
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&  // 用于检查给定类型T是否是带有参数集的可构造类型,参数集为Other1
                        std::is_constructible<Ty2, Other2>::value &&
                        std::is_convertible<Other1 &&, Ty1>::value &&  // 检查Other1&&类型是否可以隐式转化为Ty1类型
                        std::is_convertible<Other2 &&, Ty2>::value, int>::type= 0>
        constexpr pair(Other1 &&a, Other2 &&b)
                :first(tinySTL::forward<Other1>(a)),
                 second(tinySTL::forward<Other2>(b)) {

        }

        /*
         * explicit constructiable for other type
         * 由其他类型参数构造first与second
         * 作用和上面函数一样
         * 但不允许隐式构造
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&
                        std::is_constructible<Ty2, Other2>::value &&
                        (!std::is_convertible<Other1, Ty1>::value ||
                         !std::is_convertible<Other2, Ty2>::value), int>::type= 0>
        explicit constexpr pair(Other1 &&a, Other2 &&b)
                : first(tinySTL::forward<Other1>(a)),
                  second(tinySTL::forward<Other2>(b)) {

        }

        /*
         * implicit constructiable for other pair
         * 允许由其他pair构造，其他pair中的类型要能够转换为当前pair中first与second的类型
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, const Other1 &>::value &&  // 用于检查给定类型T是否是带有参数集的可构造类型,参数集为Other1
                        std::is_constructible<Ty2, const Other2 &>::value &&
                        std::is_convertible<const Other1 &, Ty1>::value &&  // 检查Other1&&类型是否可以隐式转化为Ty1类型
                        std::is_convertible<const Other2 &, Ty2>::value, int>::type= 0>
        constexpr pair(const pair<Other1, Other2> &other)
                :first(other.first), second(other.second) {

        }

        /*
         * explicit constructiable for other pair
         * 允许由其他pair构造，其他pair中的类型要能够转换为当前pair中first与second的类型
         * 作用和上面函数一样
         * 但不允许隐式构造
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, const Other1 &>::value &&
                        std::is_constructible<Ty2, const Other2 &>::value &&
                        (!std::is_convertible<const Other1 &, Ty1>::value ||
                         !std::is_convertible<const Other2 &, Ty2>::value), int>::type= 0>
        explicit constexpr pair(const pair<Other1, Other2> &other)
                :first(other.first), second(other.second) {

        }

        /*
         * implicit constructiable for other pair
         * 移动语义版本
         * 允许由其他pair构造，其他pair中的类型要能够转换为当前pair中first与second的类型
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&  // 用于检查给定类型T是否是带有参数集的可构造类型,参数集为Other1
                        std::is_constructible<Ty2, Other2>::value &&
                        std::is_convertible<Other1, Ty1>::value &&  // 检查Other1&&类型是否可以隐式转化为Ty1类型
                        std::is_convertible<Other2, Ty2>::value, int>::type= 0>
        constexpr pair(pair<Other1, Other2> &&other)
                :first(tinySTL::forward<Other1>(other.first)),
                 second(tinySTL::forward<Other2>(other.second)) {

        }

        /*
         * explicit constructiable for other pair
         * 移动语义版本
         * 允许由其他pair构造，其他pair中的类型要能够转换为当前pair中first与second的类型
         * 作用和上面函数一样
         * 但不允许隐式构造
         */
        template<typename Other1, typename Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&
                        std::is_constructible<Ty2, Other2>::value &&
                        (!std::is_convertible<Other1, Ty1>::value ||
                         !std::is_convertible<Other2, Ty2>::value), int>::type= 0>
        explicit constexpr pair(pair<Other1, Other2> &&other)
                :first(tinySTL::forward<Other1>(other.first)),
                 second(tinySTL::forward<Other2>(other.second)) {

        }

        /*
         * 拷贝赋值函数
         */
        pair &operator=(const pair &rhs) {
            /* 防止自引用 */
            if (this != &rhs) {
                first = rhs.first;
                second = rhs.second;
            }
            return *this;
        }

        /*
         * 移动赋值函数
         */
        pair &operator=(pair &&rhs) {
            /* 防止自引用 */
            if (this != &rhs) {
                first = tinySTL::move(rhs.first);
                second = tinySTL::move(rhs.second);
            }
            return *this;
        }

        /*
         * 拷贝赋值函数
         * 这里用到了上面定义的template<class Other1, class Other2,typename std::enable_if ...>constexpr pair(Other1 &&a, Other2 &&b)...构造函数
         * Other与Ty所代表的类型要能够进行相互转化才行
         */
        template<typename Other1, typename Other2>
        pair &operator=(const pair<Other1, Other2> &other) {
            first = other.first;
            second = other.second;
            return *this;
        }

        /*
         * 上面函数的移动赋值版本
         */
        template<typename Other1, typename Other2>
        pair &operator=(const pair<Other1, Other2> &&other) {
            first = tinySTL::move(other.first);
            second = tinySTL::move(other.second);
            return *this;
        }

        /*
         * 编译器生成的默认析构函数
         */
        ~pair() = default;

        /*
         * 定义pair类的swap函数
         */
        void swap(pair &other) {
            /* 防止自引用 */
            if (this != &other) {
                tinySTL::swap(first, other.first);
                tinySTL::swap(second, other.second);
            }
        }

    };

    // ===========================================================================================
    /*
     * mystl命名空间全局区定义重载运算符，以非成员函数的形式
     */

    /*
     * 等于比较元素运算符函数的定义
     */
    template<typename Ty1, typename Ty2>
    bool operator==(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    /*
     * 不等运算符
     */
    template<typename Ty1, typename Ty2>
    bool operator!=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        /* 内部调用等于运算符 */
        return !(lhs == rhs);
    }

    /*
     * 小于运算符
     */
    template<typename Ty1, typename Ty2>
    bool operator<(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    /*
     * 大于运算符
     */
    template<typename Ty1, typename Ty2>
    bool operator>(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        /* 内部调用小于运算符 */
        return rhs < lhs;
    }

    /*
     * 大于等于运算符
     */
    template<typename Ty1, typename Ty2>
    bool operator>=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        /* 内部调用小于运算符 */
        return !(lhs < rhs);
    }

    /*
    * 小于等于运算符
    */
    template<typename Ty1, typename Ty2>
    bool operator<=(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        /* 内部调用小于运算符 */
        return !(rhs < lhs);
    }

    /*
     * 重载 tinySTL的swap
     */
    template<typename Ty1, class Ty2>
    void swap(pair<Ty1, Ty2> &lhs, pair<Ty1, Ty2> &rhs) {
        /* 调用成员方法swap */
        lhs.swap(rhs);
    }

    /*
     * 制造一个pair
     * 全局函数，使两个类型的变量成为一个pair
     */
    template<typename Ty1, typename Ty2>
    pair<Ty1, Ty2> make_pair(Ty1 &&first, Ty2 &&second) {
        return pair<Ty1, Ty2>(tinySTL::forward<Ty1>(first), tinySTL::forward<Ty2>(second));
    }

} // namespace tinySTL

#endif //TINYSTL_UTILS_H
