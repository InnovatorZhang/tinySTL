//
// Created by cqupt1811 on 2022/5/7.
//

/*
 * 这个头文件负责更高级的动态内存管理
 * 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr
 */

#ifndef TINYSTL_MEMORY_H
#define TINYSTL_MEMORY_H

#include <cstddef>  /* 头文件cstddef与其C对应版本兼容，它是C头文件<stddef.h>较新版本，定义了常用的常量、宏、类型和函数 */
#include <cstdlib>  /* 定义了一些通用功能函数，包括动态存储器管理等... */
#include <climits>  /* 决定了各种变量类型的各种属性。定义在该头文件中的宏限制了各种变量类型（比如 char、int 和 long）的值 */
#include "algobase.h"  /* 这个头文件包含了库中的一些基本算法 */
#include "allocator.h"  /* 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构 */
#include "construct.h"  /* 这个头文件包含两个函数 construct 负责对象的构造，destroy 负责对象的析构*/
#include "uninitialized.h"  /* 这个头文件用于对未初始化空间构造元素 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // ============================================================================================

    /*
     * 获取对象的地址
     */
    template<typename Tp>
    constexpr Tp *address_of(Tp &value) noexcept {
        return &value;
    }

    /*
     * 获取 缓冲区的辅助函数
     */
    template<typename T>
    pair<T *, ptrdiff_t>  // 返回值
    get_buffer_helper(ptrdiff_t len, T *) {
        /* len的最大长度不能超过INT_MAX / sizeof(T)，控制申请空间的大小小于等于INT_MAX字节 */
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
            len = INT_MAX / sizeof(T);
        }
        /* 一直尝试获取len个类型T大小的缓冲区，不成功则申请空间减半后再试 */
        while (len > 0) {
            T *temp = static_cast<T *>(malloc(static_cast<size_t>(len) * sizeof(T)));
            if (temp) {
                return pair<T *, ptrdiff_t>(temp, len);
            }
            len /= 2;
        }
        return pair<T *, ptrdiff_t>(nullptr, 0);
    }

    /*
     * 获取缓冲区的函数
     */
    template<typename T>
    pair<T *, ptrdiff_t>  // 返回值
    get_temporary_buffer(ptrdiff_t len) {
        return get_buffer_helper(len, static_cast<T *>(0));
    }

    /*
     * 获取缓冲区的函数
     */
    template<typename T>
    pair<T *, ptrdiff_t>  // 返回值
    get_temporary_buffer(ptrdiff_t len, T *) {
        return get_buffer_helper(len, static_cast<T *>(0));
    }

    /*
     * 释放缓冲区的函数
     */
    template<typename T>
    void release_temporary_buffer(T *ptr) {
        free(ptr);
    }

    // ===============================================================================================

    /*
     * 类模板 : temporary_buffer
     * 进行临时缓冲区的申请与释放
     */
    template<typename ForwardIterator, typename T>
    class temporary_buffer {
    private:
        ptrdiff_t original_len;  // 缓冲区申请的大小
        ptrdiff_t len;  // 缓冲区实际大小
        T *buffer;  // 指向缓冲区的指针

    public:
        /*
         * 构造函数
         */
        temporary_buffer(ForwardIterator first, ForwardIterator last);

        /*
         * 析构函数销毁对象，释放空间
         */
        ~temporary_buffer() {
            /* 销毁空间上的对象，然后再释放空间 */
            tinySTL::destroy(buffer, buffer + len);
            free(buffer);
        }

    public:
        /*
         * 返回缓冲区实际大小
         */
        ptrdiff_t size() const noexcept {
            return len;
        }

        /*
         * 请求的空间大小
         */
        ptrdiff_t requested_size() const noexcept {
            return original_len;
        }

        /*
         * 缓冲区开始地址
         */
        T *begin() noexcept {
            return buffer;
        }

        /*
         * 缓冲区结束地址
         */
        T *end() noexcept {
            return buffer + len;
        }

    private:
        /*
         * 使用malloc分配缓存空间
         */
        void allocate_buffer();

        /*
         * 若第二参数为 std::true_type，不需要做任何擦欧总
         */
        void initialize_buffer(const T &, std::true_type) {

        }

        /*
         * 初始化缓存空间为指定值
         * 在第二个参数为std::false_type的情况下
         */
        void initialize_buffer(const T &value, std::false_type) {
            tinySTL::uninitialized_fill_n(buffer, len, value);
        }

        /*
         * 将拷贝构造函数设置为私有函数，防止编译器自动生成
         */
        temporary_buffer(const temporary_buffer &);

        /*
         * 将赋值函数设置为私有函数，防止编译器自动生成
         */
        void operator=(const temporary_buffer &);
    };

    /*
     * temporary_buffer类构造函数
     */
    template<typename ForwardIterator, typename T>
    temporary_buffer<ForwardIterator, T>::temporary_buffer(ForwardIterator first, ForwardIterator last) {
        try {
            len = tinySTL::distance(first, last);
            allocate_buffer();
            if (len > 0) {
                /* std::is_trivially_default_constructible检查类型T是否有trivial的构造函数，根据结果调用对应的initialize_buffer函数 */
                initialize_buffer(*first, std::is_trivially_default_constructible<T>());
            }
        }
        catch (...) {
            /* 若发生异常，需要清空申请的空间，重置变量的值 */
            free(buffer);
            buffer = nullptr;
            len = 0;
        }
    }

    /*
     * 使用malloc分配缓存空间
     */
    template<typename ForwardIterator, typename T>
    void temporary_buffer<ForwardIterator, T>::allocate_buffer() {
        original_len = len;
        /* len的最大长度不能超过INT_MAX / sizeof(T)，控制申请空间的大小小于等于INT_MAX字节 */
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
            len = INT_MAX / sizeof(T);
        }
        /* 一直尝试获取len个类型T大小的缓冲区，不成功则申请空间减半后再试 */
        while (len > 0) {
            buffer = static_cast<T *>(malloc(len * sizeof(T)));
            if (buffer) {
                break;
            }
            len /= 2;
        }
    }

    // ==============================================================================================

    /*
     * 模板类: auto_ptr
     * 一个具有严格对象所有权的小型智能指针
     */
    template<typename T>
    class auto_ptr {
    public:
        /* 定义使用类型的别名 */
        typedef T elem_type;

    private:
        /* 包装的实际的指针 */
        T *m_ptr;

    public:
        /*
         * 显式构造函数
         */
        explicit auto_ptr(T *p = nullptr) : m_ptr(p) {

        }

        /*
         * 拷贝构造函数
         */
        auto_ptr(auto_ptr &rhs) : m_ptr(rhs.release()) {

        }

        /*
         * 类成员模板函数，拷贝构造函数
         */
        template<typename U>
        auto_ptr(auto_ptr<U> &rhs):m_ptr(rhs.release()) {

        }

        /*
         * 拷贝赋值函数
         */
        auto_ptr &operator=(auto_ptr &rhs) {
            /* 检查是否自赋值 */
            if (this != &rhs) {
                delete m_ptr;
                m_ptr = rhs.release();
            }
            return *this;
        }

        /*
         * 类成员模板函数,拷贝赋值函数
         */
        template<typename U>
        auto_ptr &operator=(auto_ptr<U> &rhs) {
            /* 对象所保存的指针不相同的情况下才赋值 */
            if (this->get() != rhs.get()) {
                delete m_ptr;
                m_ptr = rhs.release();
            }
            return *this;
        }

        /*
         * 析构函数，其中释放指针对应的空间
         */
        ~auto_ptr() {
            delete m_ptr;
        }

    public:
        /*
         * 重载 * 操作符，使其行为类似指针
         */
        T &operator*() const {
            return *m_ptr;
        }

        /*
         * 重载 -> 操作符，使其行为类似指针
         */
        T *operator->() const {
            return m_ptr;
        }

        /*
         * 提供获取原始指针的方法
         */
        T *get() const {
            return m_ptr;
        }

        /*
         * 释放类中保存的指针
         */
        T *release() {
            T *temp = m_ptr;
            m_ptr = nullptr;
            return temp;
        }

        /*
         * 重置指针
         */
        void reset(T *p = nullptr) {
            if (m_ptr != p) {
                delete m_ptr;
                m_ptr = p;
            }
        }
    };

}  // namespace tinySTL

#endif //TINYSTL_MEMORY_H
