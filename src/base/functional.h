//
// Created by cqupt1811 on 2022/5/7.
//

/*
 * 这个头文件包含了 tinySTL 的函数对象与哈希函数
 */

#ifndef TINYSTL_FUNCTIONAL_H
#define TINYSTL_FUNCTIONAL_H

#include <cstddef>  /* 头文件cstddef与其C对应版本兼容，它是C头文件<stddef.h>较新版本，定义了常用的常量、宏、类型和函数 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 定义一元函数的参数类型与返回类型
     */
    template<typename Arg, typename Result>
    struct unary_function {
        typedef Arg argument_type;
        typedef Result result_type;
    };

    /*
     * 定义二元函数的参数类型与返回类型
     */
    template<typename Arg1, typename Arg2, typename Result>
    struct binary_function {
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argument_type;
        typedef Result result_type;
    };

    /*
     * 仿函数 加法
     */
    template<typename T>
    struct plus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const {
            return x + y;
        }
    };

    /*
     * 仿函数 减法
     */
    template<typename T>
    struct minus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const {
            return x - y;
        }
    };

    /*
     * 仿函数 乘法
     */
    template<typename T>
    struct multiplies : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const {
            return x * y;
        }
    };

    /*
     * 仿函数 除法
     */
    template<typename T>
    struct divides : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const {
            return x / y;
        }
    };

    /*
     * 仿函数 取模
     */
    template<typename T>
    struct modulus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const {
            return x % y;
        }
    };

    /*
     * 仿函数 取反
     */
    template<typename T>
    struct negate : public unary_function<T, T> {
        T operator()(const T &x) const {
            return -x;
        }
    };

    /*
     * 加法的证同元素
     * 函数模板
     */
    template<typename T>
    T identity_element(plus<T>) {
        return T(0);
    }

    /*
     * 乘法的证同元素
     * 函数模板
     */
    template<typename T>
    T identity_element(multiplies<T>) {
        return T(1);
    }

    /*
     * 仿函数 等于
     */
    template<typename T>
    struct equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x == y;
        }
    };

    /*
     * 仿函数 不等于
     */
    template<typename T>
    struct not_equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x != y;
        }
    };

    /*
     * 仿函数 大于
     */
    template<typename T>
    struct greater : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x > y;
        }
    };

    /*
     * 仿函数 小于
     */
    template<typename T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x < y;
        }
    };

    /*
     * 仿函数 大于等于
     */
    template<typename T>
    struct greater_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x >= y;
        }
    };

    /*
     * 仿函数 小于等于
     */
    template<typename T>
    struct less_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x <= y;
        }
    };

    /*
     * 仿函数 逻辑与
     */
    template<typename T>
    struct logical_and : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x && y;
        }
    };

    /*
     * 仿函数 逻辑或
     */
    template<typename T>
    struct logical_or : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const {
            return x || y;
        }
    };

    /*
     * 仿函数 逻辑非
     */
    template<typename T>
    struct logical_not : public unary_function<T, bool> {
        bool operator()(const T &x) const {
            return !x;
        }
    };

    /*
     * 仿函数 证同函数，返回元素本身
     */
    template<typename T>
    struct identity : public unary_function<T, T> {
        const T &operator()(const T &x) const {
            return x;
        }
    };

    /*
     * 仿函数 接受一个pair对象，返回pair对象的第一个元素
     */
    template<typename Pair>
    struct select_first : public unary_function<Pair, typename Pair::first_type> {
        const typename Pair::first_type &operator()(const Pair &x) const {
            return x.first;
        }
    };

    /*
     * 仿函数 接受一个pair对象，返回pair对象的第二个元素
     */
    template<typename Pair>
    struct select_second : public unary_function<Pair, typename Pair::second_type> {
        const typename Pair::second_type &operator()(const Pair &x) const {
            return x.second;
        }
    };

    /*
     * 投影函数 返回第一个参数
     */
    template<typename Arg1, typename Arg2>
    struct project_first : public binary_function<Arg1, Arg2, Arg1> {
        Arg1 operator()(const Arg1 &x, const Arg2 &y) const {
            return x;
        }
    };

    /*
     * 投影函数 返回第二个参数
     */
    template<typename Arg1, typename Arg2>
    struct project_second : public binary_function<Arg1, Arg2, Arg2> {
        Arg2 operator()(const Arg1 &x, const Arg2 &y) const {
            return y;
        }
    };

    // hash===============================================================================================

    /*
     * hash 仿函数模板
     */
    template<typename key>
    struct hash {

    };

    /*
     * hash 仿函数，指针类型偏特化
     */
    template<typename T>
    struct hash<T *> {
        size_t operator()(T *p) const noexcept {
            /* 将指针p重新转型解释为 size_t 类型 */
            return reinterpret_cast<size_t>(p);
        }
    };

    /*
     * 定义一个宏，该宏将Type对应的类型全都重新解释为size_t类型
     * 对于所有整形类型，返回原值即可
     */
#define TINYSTL_TRIVIAL_HASH_FCN(Type)          \
template<> struct hash<Type>                    \
{                                               \
    size_t operator()(Type val) const noexcept  \
    {                                           \
        return static_cast<size_t>(val);        \
    }                                           \
};

    TINYSTL_TRIVIAL_HASH_FCN(bool);

    TINYSTL_TRIVIAL_HASH_FCN(char);

    TINYSTL_TRIVIAL_HASH_FCN(signed char);

    TINYSTL_TRIVIAL_HASH_FCN(unsigned char);

    TINYSTL_TRIVIAL_HASH_FCN(wchar_t);

    TINYSTL_TRIVIAL_HASH_FCN(char16_t);

    TINYSTL_TRIVIAL_HASH_FCN(char32_t);

    TINYSTL_TRIVIAL_HASH_FCN(short);

    TINYSTL_TRIVIAL_HASH_FCN(unsigned short);

    TINYSTL_TRIVIAL_HASH_FCN(int);

    TINYSTL_TRIVIAL_HASH_FCN(unsigned int);

    TINYSTL_TRIVIAL_HASH_FCN(long);

    TINYSTL_TRIVIAL_HASH_FCN(unsigned long);

    TINYSTL_TRIVIAL_HASH_FCN(long long);

    TINYSTL_TRIVIAL_HASH_FCN(unsigned long long);

#undef TINYSTL_TRIVIAL_HASH_FCN

    /*
     * hash 仿函数
     * 对于浮点数，逐位hash
     * 辅助函数
     */
    inline size_t bitwise_hash(const unsigned char *first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
#else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
#endif
        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i) {
            result ^= (size_t) first[i];
            result *= fnv_prime;
        }
        return result;
    }

    /*
     * hash 仿函数
     * 对float类的全特化模板
     */
    template<>
    struct hash<float> {
        size_t operator()(const float &val) {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) &val, sizeof(float));
        }
    };

    /*
     * hash 仿函数
     * 对double类的全特化模板
     */
    template<>
    struct hash<double> {
        size_t operator()(const double &val) {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) &val, sizeof(double));
        }
    };

    /*
     * hash 仿函数
     * 对long double类的全特化模板
     */
    template<>
    struct hash<long double> {
        size_t operator()(const long double &val) {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) &val, sizeof(long double));
        }
    };

}  // namespace tinySTL

#endif //TINYSTL_FUNCTIONAL_H
