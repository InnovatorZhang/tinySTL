//
// Created by cqupt1811 on 2022/5/14.
//
/*
 * 这个头文件包含了一个模板类 stack
 * stack : 栈
 * 默认使用deque作为容器类,stack实质上是一个适配器
 */

#ifndef TINYSTL_STACK_H
#define TINYSTL_STACK_H

#include "deque.h"  /* deque: 双端队列 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 模板类 stack
     * 参数 T 代表数据类型，参数Container代表底层容器类型，缺省使用 tinySTL::deque 作为底层容器
     */
    template<typename T, typename Container=tinySTL::deque<T>>
    class stack {
    public:
        /*
         * 定义类中所用别名
         */
        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;

        /*
         * 检查数据类型T是否与容器的数据类型value_type相同
         */
        static_assert(std::is_same<T, value_type>::value,
                      "the value_type of Container should be same with T");

    private:
        /*
         * 实现stack的底层容器
         */
        container_type c_;

    public:
        /*
         * 编译器生成默认构造函数
         */
        stack() = default;

        /*
         * 显式构造函数,参数为大小n
         */
        explicit stack(size_type n) : c_(n) {

        }

        /*
         * 有参构造函数
         * 调用底层容器对应的构造函数
         */
        stack(size_type n, const value_type &value) : c_(n, value) {

        }

        /*
         * 有参构造函数,接收一对迭代器,使用迭代器间内容构造stack
         * 类成员模板,调用底层容器对应的构造函数
         */
        template<typename InputIter>
        stack(InputIter first, InputIter last):c_(first, last) {

        }

        /*
         * 构造函数 使用初始化列表，即支持以{}的方式构建对象
         */
        stack(std::initializer_list<value_type> i_list) : c_(i_list.begin(), i_list.end()) {

        }

        /*
         * 构造函数,接收一个底层容器对象
         */
        stack(const container_type &c) : c_(c) {

        }

        /*
         * 构造函数,接收一个底层容器对象 移动语义
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        stack(container_type &&c) noexcept(std::is_nothrow_move_constructible<container_type>::value)
                : c_(tinySTL::move(c)) {

        }

        /*
         * 拷贝构造函数
         */
        stack(const stack &rhs) : c_(rhs.c_) {

        }

        /*
         * 移动构造函数
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        stack(stack &&rhs) noexcept(std::is_nothrow_move_assignable<container_type>::value)
                : c_(tinySTL::move(rhs.c_)) {

        }

        /*
         * 拷贝赋值函数
         */
        stack &operator=(const stack &rhs) {
            c_ = rhs.c_;
            return *this;
        }

        /*
         * 移动赋值函数
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        stack &operator=(stack &&rhs) noexcept(std::is_nothrow_move_assignable<container_type>::value) {
            c_ = tinySTL::move(rhs.c_);
            return *this;
        }

        /*
         * 使用初始化列表，即支持以{}的方式赋值对象
         */
        stack &operator=(std::initializer_list<value_type> i_list) {
            c_ = i_list;
            return *this;
        }

        /*
         * 由编译器自动生成析构函数
         */
        ~stack() = default;

    public:
        /*
         * 访问栈顶的元素
         */
        reference top() {
            return c_.back();
        }

        /*
         * 访问栈顶的元素 const重载
         */
        const_reference top() const {
            return c_.back();
        }

        /*
         * stack是否为空
         */
        bool empty() const noexcept {
            return c_.empty();
        }

        /*
         * 获取stack大小
         */
        size_type size() const noexcept {
            return c_.size();
        }

        /*
         * 在stack顶部构建一个数据对象
         * 类成员模板,可变参数,完美转发
         */
        template<typename ...Args>
        void emplace(Args &&...args) {
            c_.emplace_back(tinySTL::forward<Args>(args)...);
        }

        /*
         * 向stack尾部压入一个数据
         */
        void push(const value_type &value) {
            c_.push_back(value);
        }

        /*
         * 向stack尾部压入一个数据 移动语义
         */
        void push(value_type &&value) {
            c_.push_back(tinySTL::move(value));
        }

        /*
         * 从栈顶删除一个元素
         */
        void pop() {
            c_.pop_back();
        }

        /*
         * 清空栈中所有元素
         */
        void clear() {
            while (!empty()) {
                pop();
            }
        }

        /*
         * 属于stack的swap操作
         */
        void swap(stack &rhs) noexcept(noexcept(tinySTL::swap(c_, rhs.c_))) {
            tinySTL::swap(c_, rhs.c_);
        }

    public:
        /*
         * 添加友元,相等于操作符重载
         */
        friend bool operator==(const stack &lhs, const stack &rhs) {
            return lhs.c_ == rhs.c_;
        }

        /*
         * 添加友元,小于操作符重载
         */
        friend bool operator<(const stack &lhs, const stack &rhs) {
            return lhs.c_ < rhs.c_;
        }
    };

    // ==============================================================================================

    /*
     * 在全局区域重载stack类比较操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename T, typename Container>
    bool operator==(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于操作符
     */
    template<typename T, typename Container>
    bool operator!=(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于操作符
     */
    template<typename T, typename Container>
    bool operator<(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于操作符
     */
    template<typename T, typename Container>
    bool operator>(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于操作符
     */
    template<typename T, typename Container>
    bool operator<=(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于操作符
     */
    template<typename T, typename Container>
    bool operator>=(const stack<T, Container> &lhs, const stack<T, Container> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载 tinySTL 的 swap
     */
    template<typename T, typename Container>
    void swap(stack<T, Container> &lhs, stack<T, Container> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
        return lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_STACK_H
