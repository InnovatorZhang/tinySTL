//
// Created by cqupt1811 on 2022/5/14.
//

/*
 * 这个头文件包含了两个模板类 queue 和 priority_queue
 * queue          : 队列
 * priority_queue : 优先队列
 */

#ifndef TINYSTL_QUEUE_H
#define TINYSTL_QUEUE_H

#include "deque.h"  /* 这个头文件包含了一个模板类 deque */
#include "vector.h"  /* 这个头文件包含一个模板类 vector */
#include "functional.h"  /* 这个头文件包含了 tinySTL 的函数对象与哈希函数 */
#include "heap_algo.h"  /* 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 模板类 queue
     * 数据 T 代表数据类型,参数 Container 代表底层容器类型,缺省使用 tinySTL::deque 作为底层容器
     */
    template<typename T, typename Container=tinySTL::deque<T>>
    class queue {
    public:
        /*
         * 类中使用的别名定义
         */
        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;

        /*
         * 检查 T 与 value_type 定义是否一致
         */
        static_assert(std::is_same<T, value_type>::value,
                      "the value_type of Container should be same with T");

    private:
        /*
         * queue 使用的底层容器
         */
        container_type c_;

    public:
        /*
         * 由编译器生成默认构造函数
         */
        queue() = default;

        /*
         * 显式构造函数
         */
        explicit queue(size_type n) : c_(n) {

        }

        /*
         * 有参构造函数
         */
        queue(size_type n, const value_type &value) : c_(n, value) {

        }

        /*
         * 有参构造函数,接收一对迭代器
         * 类成员模板
         */
        template<typename InputIter>
        queue(InputIter first, InputIter last):c_(first, last) {

        }

        /*
         * 支持初始化列表的构造方式,即使用{}构造queue
         */
        queue(std::initializer_list<value_type> i_list) : c_(i_list.begin(), i_list.end()) {

        }

        /*
         * 有参构造函数
         * 接收一个内部容器对象
         */
        queue(const container_type &c) : c_(c) {

        }

        /*
         * 有参构造函数 移动语义
         * 接收一个内部容器对象
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        queue(container_type &&c) noexcept(std::is_nothrow_move_constructible<container_type>::value)
                : c_(tinySTL::move(c)) {

        }

        /*
         * 拷贝构造函数
         */
        queue(const queue &rhs) : c_(rhs.c_) {

        }

        /*
         * 移动构造函数
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        queue(queue &&rhs) noexcept(std::is_nothrow_move_constructible<container_type>::value)
                : c_(tinySTL::move(rhs.c_)) {

        }

        /*
         * 拷贝赋值函数
         */
        queue &operator=(const queue &rhs) {
            c_ = rhs.c_;
            return *this;
        }

        /*
         * 移动构造函数
         * 内部容易需要支持move语义,使用is_nothrow_move_assignable检测
         */
        queue &operator=(queue &&rhs) noexcept(std::is_nothrow_move_assignable<container_type>::value) {
            c_ = tinySTL::move(rhs.c_);
            return *this;
        }

        /*
         * 赋值函数,接收一个初始化列表,支持以{}的形式赋值
         */
        queue &operator=(std::initializer_list<value_type> i_list) {
            c_ = i_list;
            return *this;
        }

        /*
         * 由编译器生成析构函数
         */
        ~queue() = default;

    public:
        /*
         * 获取队头元素
         */
        reference front() {
            return c_.front();
        }

        /*
         * 获取队头元素 const重载版
         */
        const_reference front() const {
            return c_.front();
        }

        /*
         * 获取队头元素
         */
        reference back() {
            return c_.back();
        }

        /*
         * 获取队头元素 const重载版
         */
        const_reference back() const {
            return c_.back();
        }

        /*
         * queue是否为空
         */
        bool empty() const noexcept {
            return c_.empty();
        }

        /*
         * 获取queue的大小
         */
        size_type size() const noexcept {
            return c_.size();
        }

        /*
         * 在队尾构建一个元素
         * 类成员模板,可变参数模板,完美转发
         */
        template<typename ...Args>
        void emplace(Args &&...args) {
            c_.emplace_back(tinySTL::forward<Args>(args)...);
        }

        /*
         * 向队尾加入一个元素的副本
         */
        void push(const value_type &value) {
            c_.push_back(value);
        }

        /*
         * 向队尾加入一个元素的副本 移动语义版
         */
        void push(value_type &&value) {
            c_.push_back(tinySTL::move(value));
        }

        /*
         * 从队头弹出一个数据
         */
        void pop() {
            c_.pop_front();
        }

        /*
         * 清空队中的数据
         */
        void clear() {
            while (!empty()) {
                pop();
            }
        }

        /*
         * 定义queue的swap方法
         */
        void swap(queue &rhs) noexcept(noexcept(tinySTL::swap(c_, rhs.c_))) {
            tinySTL::swap(c_, rhs.c_);
        }

    private:
        /*
         * 定义操作符重载友元函数
         */
        friend bool operator==(const queue &lhs, const queue &rhs) {
            return lhs.c_ == rhs.c_;
        }

        /*
         * 定义操作符重载友元函数
         */
        friend bool operator<(const queue &lhs, const queue &rhs) {
            return lhs.c_ < rhs.c_;
        }

    };

    // ==============================================================================================

    /*
     * 全局区域定义重载的操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename T, typename Container>
    bool operator==(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于操作符
     */
    template<typename T, typename Container>
    bool operator!=(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于操作符
     */
    template<typename T, typename Container>
    bool operator<(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于操作符
     */
    template<typename T, typename Container>
    bool operator>(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于操作符
     */
    template<typename T, typename Container>
    bool operator<=(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于操作符
     */
    template<typename T, typename Container>
    bool operator>=(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename T, typename Container>
    void swap(queue<T, Container> &lhs, queue<T, Container> &rhs) {
        lhs.swap(rhs);
    }


    // ===========================================================================================

    /*
     * 模板类 priority_queue
     * 参数 T 代表数据类型，参数 Container 代表容器类型，缺省使用 tinySTL::vector 作为底层容器
     * 参数三代表比较权值的方式，缺省使用 tinySTL::less 作为比较方式
     */
    template<typename T, typename Container=tinySTL::vector<T>,
            typename Compare = tinySTL::less<typename Container::value_type>>
    class priority_queue {
    public:
        /*
         * 定义类中所使用的别名
         */
        typedef Container container_type;
        typedef Compare value_compare;
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;

        /*
         * 检查类型 T 与 value_type 是否相同
         */
        static_assert(std::is_same<T, value_type>::value,
                      "the value_type of Container should be same with T");

    private:
        /*
         * 使用的底层容器
         */
        container_type c_;
        /*
         * 元素比较标准
         */
        value_compare comp_;

    public:

        /*
         * 由编译器生成默认构造函数
         */
        priority_queue() = default;

        /*
         * 有参构造函数 接收一个比较函数对象
         */
        priority_queue(const Compare &c) : c_(), comp_(c) {

        }

        /*
         * 显式有参构造函数,接收一个大小参数
         */
        explicit priority_queue(size_type n) : c_(n) {
            /* 根据传入的大小,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(size_type n, const value_type &value) : c_(n, value) {
            /* 根据传入的大小,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 有参构造函数 接收一对迭代器作为参数,使用迭代器间数据构造优先队列
         * 类成员模板
         */
        template<typename InputIter>
        priority_queue(InputIter first, InputIter last):c_(first, last) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 接收初始化列表作为参数,支持{}形式初始化
         */
        priority_queue(std::initializer_list<value_type> i_list) : c_(i_list) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 有参构造函数,接收一个内部容器对象
         */
        priority_queue(const container_type &s) : c_(s) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 有参构造函数,接收一个内部容器对象 移动语义版本
         */
        priority_queue(container_type &&s) : c_(tinySTL::move(s)) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 拷贝构造函数
         */
        priority_queue(const priority_queue &rhs) : c_(rhs.c_), comp_(rhs.comp_) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 移动构造函数
         */
        priority_queue(priority_queue &&rhs) :
                c_(tinySTL::move(rhs.c_)), comp_(tinySTL::move(rhs.comp_)) {
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 拷贝赋值函数
         */
        priority_queue &operator=(const priority_queue &rhs) {
            c_ = rhs.c_;
            comp_ = rhs.comp_;
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        /*
         * 移动赋值函数
         */
        priority_queue &operator=(priority_queue &&rhs) {
            c_ = tinySTL::move(rhs.c_);
            comp_ = tinySTL::move(rhs.comp_);
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        /*
         * 赋值函数,接收初始化列表,支持以{}的形式赋值
         */
        priority_queue &operator=(std::initializer_list<value_type> i_list) {
            c_ = i_list;
            comp_ = value_compare();
            /* 根据传入的数据,制作一个heap */
            tinySTL::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        /*
         * 由编译器生成析构函数
         */
        ~priority_queue() = default;

    public:
        /*
         * 获取优先队列头部元素
         */
        const_reference top() const {
            return c_.front();
        }

        /*
         * 优先队列是否为空
         */
        bool empty() const noexcept {
            return c_.empty();
        }

        /*
         * 获取优先队列大小
         */
        size_type size() const noexcept {
            return c_.size();
        }

        /*
         * 构建一个元素加入优先队列,底层调用push_heap方法入堆
         * 类成员模板,可变模板参数,完美转发
         */
        template<typename ...Args>
        void emplace(Args &&...args) {
            /* 先将数据构建在内部容器的尾部 */
            c_.emplace_back(tinySTL::forward<Args>(args)...);
            /* 再将此节点入堆 */
            tinySTL::push_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 将会一个元素加入优先队列中
         */
        void push(const value_type &value) {
            /* 先将数据加入到内部容器的尾部 */
            c_.push_back(value);
            /* 再将此节点入堆 */
            tinySTL::push_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 将会一个元素加入优先队列中 移动语义版本
         */
        void push(value_type &&value) {
            /* 先将数据加入到内部容器的尾部 */
            c_.push_back(tinySTL::move(value));
            /* 再将此节点入堆 */
            tinySTL::push_heap(c_.begin(), c_.end(), comp_);
        }

        /*
         * 将优先队列最优先元素弹出
         * 将堆顶元素弹出
         */
        void pop() {
            /* 此方法会将最优元素放到容器尾部 */
            tinySTL::pop_heap(c_.begin(), c_.end(), comp_);
            /* 将最优元素弹出 */
            c_.pop_back();
        }

        /*
         * 清除优先队列中所有元素
         */
        void clear() {
            while (!empty()) {
                pop();
            }
        }

        /*
         * 定义专属的swap方法
         */
        void swap(priority_queue &rhs) noexcept(noexcept(tinySTL::swap(c_, rhs.c_)) &&
                                                noexcept(tinySTL::swap(comp_, rhs.comp_))) {
            tinySTL::swap(c_, rhs.c_);
            tinySTL::swap(comp_, rhs.comp_);
        }

    public:
        /*
         * 定义友元函数,相等于操作符重载
         */
        friend bool operator==(const priority_queue &lhs, const priority_queue &rhs) {
            return lhs.c_ == rhs.c_;
        }

        /*
         * 定义友元函数,不等于操作符重载
         */
        friend bool operator!=(const priority_queue &lhs, const priority_queue &rhs) {
            return lhs.c_ != rhs.c_;
        }
    };

    // ===========================================================================================
    /*
     * 再全局区域重载操作符,优先队列只有相等于与不等于操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename T, typename Container, typename Compare>
    bool operator==(const priority_queue<T, Container, Compare> &lhs,
                    const priority_queue<T, Container, Compare> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不等于操作符
     */
    template<typename T, typename Container, typename Compare>
    bool operator!=(const priority_queue<T, Container, Compare> &lhs,
                    const priority_queue<T, Container, Compare> &rhs) {
        return lhs != rhs;
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename T, typename Container, typename Compare>
    void swap(priority_queue<T, Container, Compare> &lhs,
              priority_queue<T, Container, Compare> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_QUEUE_H
