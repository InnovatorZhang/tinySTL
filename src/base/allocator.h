//
// Created by cqupt1811 on 2022/5/6.
//

/*
 * 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构
 */

#ifndef TINYSTL_ALLOCATOR_H
#define TINYSTL_ALLOCATOR_H

#include "construct.h"  /* 这个头文件包含两个函数 construct，destroy, construct负责对象的构造 destroy   : 负责对象的析构 */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =============================================================================================

    /*
     * 模板类：allocator
     * 模板函数代表数据类型
     * 此类由静态函数构成
     */
    template<typename T>
    class allocator {
    public:
        /* 首先声明一系列类型别名 */
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        /* 定义一系列静态方法 */
        static T *allocate();

        static T *allocate(size_type n);

        static void deallocate(T *ptr);

        static void deallocate(T *ptr, size_type n);

        static void construct(T *ptr);

        static void construct(T *ptr, const T &value);

        static void construct(T *ptr, T &&value);

        template<typename... Args>
        static void construct(T *ptr, Args &&... args);

        static void destroy(T *ptr);

        static void destroy(T *first, T *last);
    };

    /*
     * 申请内存空间，返回对应指针
     * 无参数的allocate方法调用::operator new函数申请 一个 类型T大小的空间，返回类型T的指针
     * 因为::operator new函数返回的是void类型的指针，所以使用static_cast<T *>转型为T类型指针返回
     */
    template<typename T>
    T *allocator<T>::allocate() {
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    /*
     * 接受一个int类型参数的allocate方法调用::operator new函数申请n个类型T大小的空间，返回这段空间的起始地址指针
     * 因为::operator new函数返回的是void类型的指针，所以使用static_cast<T *>转型为T类型指针返回
     * 对于传入的n为0的情况要特殊处理一下
     */
    template<typename T>
    T *allocator<T>::allocate(size_type n) {
        if (n == 0) {
            return nullptr;
        }
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    /*
     * 释放指针对应的内存空间
     * 只接收一个指针，判断指针是否为空指针，不为空则调用::operator delete释放内存空间
     */
    template<typename T>
    void allocator<T>::deallocate(T *ptr) {
        if (ptr == nullptr) {
            return;
        }
        ::operator delete(ptr);
    }

    /*
     * 释放指针对应的内存空间
     * 只接收一个指针，判断指针是否为空指针，不为空则调用::operator delete释放内存空间
     * 该重载多接受一个size类型参数，最终没有用到该size参数
     */
    template<typename T>
    void allocator<T>::deallocate(T *ptr, size_type /* size */) {
        if (ptr == nullptr) {
            return;
        }
        /* 只需要传入指针即可，不需要元素个数 */
        ::operator delete(ptr);
    }

    /*
     * 在指定位置上建立一个类对象，调用默认构造函数
     */
    template<typename T>
    void allocator<T>::construct(T *ptr) {
        tinySTL::construct(ptr);
    }

    /*
     * 在指定位置上建立一个类对象，调用拷贝构造函数
     */
    template<typename T>
    void allocator<T>::construct(T *ptr, const T &value) {
        tinySTL::construct(ptr, value);
    }

    /*
     * 在指定位置上建立一个类对象，调用移动构造函数
     */
    template<typename T>
    void allocator<T>::construct(T *ptr, T &&value) {
        tinySTL::construct(ptr, tinySTL::move(value));
    }

    /*
     * 模板类的成员函数模板，可变参数模板
     * 在指定位置上建立一个类对象，调用对应参数列表的构造函数
     * 使用完美转发
     */
    template<typename T>
    template<typename... Args>
    void allocator<T>::construct(T *ptr, Args &&...args) {
        tinySTL::construct(ptr, tinySTL::forward<Args>(args)...);
    }

    /*
     * 销毁ptr指针指定位置的类对象
     */
    template<typename T>
    void allocator<T>::destroy(T *ptr) {
        tinySTL::destroy(ptr);
    }

    /*
     * 销毁first->last区间中的类对象
     * 指针作为迭代器
     */
    template<typename T>
    void allocator<T>::destroy(T *first, T *last) {
        tinySTL::destroy(first, last);
    }

}  // namespace tinySTL

#endif //TINYSTL_ALLOCATOR_H
