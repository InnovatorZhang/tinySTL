//
// Created by cqupt1811 on 2022/5/6.
//

/*
 * 这个头文件包含两个函数 construct，destroy
 * construct : 负责对象的构造
 * destroy   : 负责对象的析构
 */

#ifndef TINYSTL_CONSTRUCT_H
#define TINYSTL_CONSTRUCT_H

#include <new>  /* 分配内存与释放内存的函数,包括placement new等 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "type_traits.h"  /* type_traits.h文件钟引入了标准库钟的type_traits文件，同时定义了一些自己的模板方法 */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =============================================================================================

    /*
     * construct 模板函数，参数类型自动推断
     */

    /*
     * 使用placement new在ptr指针指定处调用默认构造函数构建对象
     */
    template<class Ty>
    void construct(Ty *ptr) {
        /*  :: 的作用是指定全局作用域下的 placement new函数,  placement new接受void*参数，所以需要转型为void* */
        ::new((void *) ptr)Ty();
    }

    /*
     * 使用placement new在ptr指针指定处调用有参构造函数（一个参数，以引用的方式传入）构建对象
     */
    template<typename Ty1, typename Ty2>
    void construct(Ty1 *ptr, const Ty2 &value) {
        ::new((void *) ptr)Ty1(value);
    }

    /*
     * 使用placement new在ptr指针指定位置调用有参构造函数（可变参数列表+完美转发）构建对象
     */
    template<typename Ty, typename... Args>
    void construct(Ty *ptr, Args &&... args) {
        ::new((void *) ptr)Ty(tinySTL::forward<Args>(args)...);
    }

    //============================================================================================

    /*
     * destroy 模板函数，参数类型自动推断
     */

    /*
     * 销毁函数指针指向的一个对象
     * 函数的重载，若第二个参数判断为std::true_type，则调用此函数，
     * 此函数中不需要对对象进行析构，代表的是由编译器自动生成析构函数的类型
     */
    template<typename Ty>
    void destroy_one(Ty *, std::true_type) {

    }

    /*
     * 销毁函数指针指向的一个对象
     * 函数的重载，若第二个参数判断为std::false_type，在函数中调用对象的析构函数，
     * 此函数中需要对对象进行析构，代表的是由自己编写有析构函数的类型
     */
    template<typename Ty>
    void destroy_one(Ty *pointer, std::false_type) {
        /* 确定指针不为空，然后调用Ty类的析构函数 */
        if (pointer != nullptr) {
            pointer->~Ty();
        }
    }

    /*
     * 销毁迭代器表示的一段对象
     * 函数的重载，若第二个参数判断为std::true_type，则调用此函数，
     * 此函数中不需要对对象进行析构，代表的是由编译器自动生成析构函数的类型
     */
    template<typename ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {

    }

    /*
     * 销毁迭代器表示的一段对象，对区间中的每一个对象调用destroy方法
     * 函数的重载，若第二个参数判断为std::false_type，则调用此函数，
     * 此函数中需要对对象进行析构，代表的是由自己编写有析构函数的类型
     */
    template<typename ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
        for (; first != last; ++first) {
            /* &*first 时为了获取迭代器所指元素的真实位置，因为*操作是由迭代器重载过的 */
            destroy(&*first);
        }
    }

    /*
     * 函数重载，接受一个指针作为参数
     * 调用destroy_one方法销毁一个对象，负责传入第二个参数，
     * 第二个参数由std::is_trivially_destructible<Ty>{}判断得到std::true or std::false
     */
    template<typename Ty>
    void destroy(Ty *pointer) {
        /* is_trivially_destructible 判断类型Ty的析构类型，是否是由编译器自动生成的，自动生成的析构函数不用显式调用 */
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    /*
     * 函数重载，接受一对迭代器作为参数
     * 调用destroy_cat方法销毁区间中对象，负责传入第二个参数，
     * 第二个参数由std::is_trivially_destructible<Ty>判断，其中Ty由萃取机制获取
     * Ty为迭代器对应的value type，目的就是判断迭代器的value type是否为trivial的类型
     */
    template<typename ForwardIter>
    void destroy(ForwardIter first, ForwardIter last) {
        destroy_cat(first, last, std::is_trivially_destructible<
                typename iterator_traits<ForwardIter>::value_type>{});
    }

}  // namespace tinySTL

#endif //TINYSTL_CONSTRUCT_H
