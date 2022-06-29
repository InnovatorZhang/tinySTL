//
// Created by cqupt1811 on 2022/5/11.
//

/*
 * 这个头文件包含了一个模板类 list
 * list : 双向链表
 * notes:
 * 异常保证：
 * tinySTL::list<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
 *      * emplace_front
 *      * emplace_back
 *      * emplace
 *      * push_front
 *      * push_back
 *      * insert
 */

#ifndef TINYSTL_LIST_H
#define TINYSTL_LIST_H

#include <initializer_list>  /* std::initializer_list<T> 类型对象是一个访问 const T 类型对象数组的轻量代理对象 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "memory.h"  /* 这个头文件负责更高级的动态内存管理,含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "functional.h"  /* 这个头文件包含了 tinySTL 的函数对象与哈希函数 */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */
#include "exceptdef.h"  /* 此文件中定义了异常相关的宏 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =============================================================================================

    /*
     * 前向声明 list_node_base 与 list_node 两个类
     * 用以定义 node_traits 类
     */
    template<typename T>
    struct list_node_base;

    template<typename T>
    struct list_node;

    template<typename T>
    struct node_traits {
        typedef list_node_base<T> *base_ptr;
        typedef list_node<T> *node_ptr;
    };

    /*
     * list 的节点结构
     */
    template<typename T>
    struct list_node_base {
        /*
         * 定义自身指针与子类指针的别名
         */
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        /*
         * 定义节点中的指针，分别指向前一个节点与后一个节点
         */
        base_ptr prev;  // 前一个节点
        base_ptr next;  // 下一个节点

        /*
         * 编译器自动生成默认构造函数
         */
        list_node_base() = default;

        /*
         * 返回自身的指针(list_node_base<T> *)
         */
        base_ptr self() {
            return static_cast<base_ptr>(&*this);
        }

        /*
         * 将自身的指针(list_node_base<T> *)转为子类的指针(list_node<T> *)
         */
        node_ptr as_node() {
            return static_cast<node_ptr>(self());
        }

        /*
         * 断开此节点的连接，并令其指向自身
         */
        void unlink() {
            prev = next = self();
        }
    };

    /*
     * list 的节点结构
     */
    template<typename T>
    struct list_node : public list_node_base<T> {
        /*
         * 定义自身指针与子类指针的别名
         */
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        /*
         * 数据存放位置
         */
        T value;

        /*
         * 编译器自动生成默认构造函数
         */
        list_node() = default;

        /*
         * 定义拷贝构造函数
         * 注意：不需要拷贝指针的值，只需要拷贝其中数据的值，所以拷贝一个节点时只会复制其中数据
         */
        list_node(const T &v) : value(v) {

        }

        /*
         * 定义拷贝构造函数
         * 注意：不需要移动指针的值，只需要移动其中数据的值，所以移动一个节点时只会移动其中数据
         */
        list_node(T &&v) : value(tinySTL::move(v)) {

        }

        /*
         * 返回自身的指针(list_node<T> *)
         */
        node_ptr self() {
            return static_cast<node_ptr>(&*this);
        }

        /*
         * 将自身的指针(list_node<T> *)转为父类的指针(list_node_base<T> *)
         */
        base_ptr as_base() {
            return static_cast<base_ptr>(self());
        }

    };

    // =====================================================================================

    /*
     * list 的迭代器设计
     * 继承
     * template<class Category, class T, class Distance = ptrdiff_t,
     * class Pointer = T *, class Reference = T &>
     * struct iterator{ ... }
     * 可以自动定义迭代器所需的五种类型
     * list 的迭代器属于双向迭代器类型
     */
    template<typename T>
    struct list_iterator : public tinySTL::iterator<tinySTL::bidirectional_iterator_tag, T> {
        /*
         * 定义类中所需的别名
         */
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_iterator<T> self;

        /*
         * 指向当前节点的指针(list_node_base<T> *)
         */
        base_ptr node_;

        /*
         * 编译器自动生成默认构造函数
         */
        list_iterator() = default;

        /*
         * 有参构造函数
         * 接受一个base_ptr类型指针
         */
        list_iterator(base_ptr x) : node_(x) {

        }

        /*
         * 有参构造函数
         * 接受一个node_ptr类型指针
         */
        list_iterator(node_ptr x) : node_(x->as_base()) {

        }

        /*
         * 拷贝构造函数
         */
        list_iterator(const list_iterator &rhs) : node_(rhs.node_) {

        }

        /*
         * 操作符 * 重载
         * 返回节点中保存的值的引用
         */
        reference operator*() const {
            return node_->as_node()->value;
        }

        /*
         * 操作符 -> 重载
         * 返回节点中保存的值的地址
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 操作符前置 ++ 重载
         */
        self &operator++() {
            TINYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        /*
         * 操作符后置 ++ 重载
         * 注意：这里返回值是 self 而不是 self &
         */
        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        /*
         * 操作符前置 -- 重载
         */
        self &operator--() {
            TINYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        /*
         * 操作符后置 -- 重载
         */
        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        /*
         * 重载相等于操作符
         * 是否相等可以通过比较node_指针的值
         */
        bool operator==(const self &rhs) const {
            return node_ == rhs.node_;
        }

        /*
         * 重载不相等操作符
         * 是否相等可以通过比较node_指针的值
         */
        bool operator!=(const self &rhs) const {
            return node_ != rhs.node_;
        }
    };

    /*
     * list 的迭代器设计
     * const 版本
     */
    template<typename T>
    struct list_const_iterator : public iterator<tinySTL::bidirectional_iterator_tag, T> {
        /*
         * 定义类中所需的别名
         */
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_const_iterator<T> self;

        /*
         * 指向当前节点的指针(list_node_base<T> *)
         */
        base_ptr node_;

        /*
         * 编译器自动生成默认构造函数
         */
        list_const_iterator() = default;

        /*
         * 有参构造函数
         * 接受一个base_ptr类型指针
         */
        list_const_iterator(base_ptr x) : node_(x) {

        }

        /*
         * 有参构造函数
         * 接受一个node_ptr类型指针
         */
        list_const_iterator(node_ptr x) : node_(x->as_base()) {

        }

        /*
         * 有参构造函数
         * 接受一个list_iterator对象
         */
        list_const_iterator(const list_iterator<T> &rhs) : node_(rhs.node_) {

        }

        /*
         * 拷贝构造函数
         */
        list_const_iterator(const list_const_iterator &rhs) : node_(rhs.node_) {

        }

        /*
         * 操作符 * 重载
         * 返回节点中保存的值的const引用
         */
        reference operator*() const {
            return node_->as_node()->value;
        }

        /*
         * 操作符 * 重载
         * 返回节点中保存的值的const指针
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 操作符前置 ++ 重载
         */
        self &operator++() {
            TINYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        /*
         * 操作符后置 ++ 重载
         * 注意：这里返回值是 self 而不是 self &
         */
        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        /*
         * 操作符前置 -- 重载
         */
        self &operator--() {
            TINYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        /*
         * 操作符后置 -- 重载
         */
        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        /*
         * 重载相等于操作符
         * 是否相等可以通过比较node_指针的值
         */
        bool operator==(const self &rhs) const {
            return node_ == rhs.node_;
        }

        /*
         * 重载不相等操作符
         * 是否相等可以通过比较node_指针的值
         */
        bool operator!=(const self &rhs) const {
            return node_ != rhs.node_;
        }
    };

    // ===========================================================================================

    /*
     * 模板类: list
     * 模板参数 T 代表数据类型
     * 循环双向链表
     */
    template<typename T>
    class list {
    public:
        /*
         * 定义空间配置器相关的别名
         */
        /* 定义一下两个别名的目的是使用其中的construct方法与destroy方法创建或销毁 节点 中的T类型元素 */
        typedef tinySTL::allocator<T> allocator_type;
        typedef tinySTL::allocator<T> data_allocator;

        typedef tinySTL::allocator<list_node_base<T>> base_allocator;
        typedef tinySTL::allocator<list_node<T>> node_allocator;

        /*
         * 定义类中相关的类型别名
         */
        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        /*
         * 迭代器相关类型别名
         */
        typedef list_iterator<T> iterator;
        typedef list_const_iterator<T> const_iterator;
        typedef tinySTL::reverse_iterator<iterator> reverse_iterator;
        typedef tinySTL::reverse_iterator<const_iterator> const_reverse_iterator;

        /*
         * 指针类型别名
         */
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        /*
         * 返回空间配置器
         */
        allocator_type get_allocator() {
            return data_allocator();
        }

    private:
        /*
         * 指向双向链表的末尾节点
         */
        base_ptr node_;
        /*
         * 链表大小
         */
        size_type size_;

    public:
        /*
         * 默认构造函数
         */
        list() {
            fill_init(0, value_type());
        }

        /*
         * 显式构造函数
         */
        explicit list(size_type n) {
            fill_init(n, value_type());
        }

        /*
         * 有参构造函数
         */
        list(size_type n, const value_type &value) {
            fill_init(n, value);
        }

        /*
         * 有参构造函数，接受一对迭代器
         * 迭代器的类型至少要为input_iterator类型
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type = 0>
        list(Iter first, Iter last) {
            copy_init(first, last);
        }

        /*
         * 构造函数 使用初始化列表，即支持以{}的方式构建对象
         */
        list(std::initializer_list<value_type> i_list) {
            copy_init(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝构造函数
         */
        list(const list &rhs) {
            copy_init(rhs.cbegin(), rhs.cend());
        }

        /*
         * 移动构造函数
         */
        list(list &&rhs) noexcept: node_(rhs.node_), size_(rhs.size_) {
            rhs.node_ = nullptr;
            rhs.size_ = 0;
        }

        /*
         * 拷贝复制函数
         */
        list &operator=(const list &rhs) {
            if (this != &rhs) {
                assign(rhs.begin(), rhs.end());
            }
            return *this;
        }

        /*
         * 移动赋值函数
         */
        list &operator=(list &&rhs) noexcept {
            clear();
            splice(end(), rhs);
            return *this;
        }

        /*
         * 接受一个初始化列表赋值
         */
        list &operator=(std::initializer_list<value_type> i_list) {
            /* 使用了swap技巧，在函数结束时，会自动释放局部变量 */
            list temp(i_list.begin(), i_list.end());
            swap(temp);
            return *this;
        }

        /*
         * 析构函数中释放资源
         */
        ~list() {
            if (node_) {
                clear();
                base_allocator::deallocate(node_);
                node_ = nullptr;
                size_ = 0;
            }
        }


    public:
        /*
         * 类成员方法，外部可以调用的方法
         */

        /*
         * 迭代器相关操作
         */

        /*
         * 返回头部迭代器
         */
        iterator begin() noexcept {
            /* 因为迭代器指向的是list的尾部，头部在尾部之后 */
            return node_->next;
        }

        /*
         * 返回头部迭代器 const重载版
         */
        const_iterator begin() const noexcept {
            return node_->next;
        }

        /*
         * 返回尾部迭代器
         */
        iterator end() noexcept {
            return node_;
        }

        /*
         * 返回尾部迭代器 const 重载版
         */
        const_iterator end() const noexcept {
            return node_;
        }

        /*
         * 返回反向迭代器头部
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 返回反向迭代器头部 const重载版
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 返回反向迭代器尾部
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 返回反向迭代器尾部 const重载版
         */
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        /*
         * 返回常量头部迭代器
         */
        const_iterator cbegin() const noexcept {
            return begin();
        }

        /*
         * 返回常量尾部迭代器
         */
        const_iterator cend() const noexcept {
            return end();
        }

        /*
         * 返回反向常量头部迭代器
         */
        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        /*
         * 返回反向常量尾部迭代器
         */
        const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /*
         * 容量相关操作
         */

        /*
         * list是否为空
         */
        bool empty() const noexcept {
            return node_->next == node_;
        }

        /*
         * 返回list的大小
         */
        size_type size() const noexcept {
            return size_;
        }

        /*
         * 返回最大可以存放的大小
         */
        size_type max_size() const noexcept {
            return static_cast<size_type>(-1);
        }

        /*
         * 访问元素相关操作
         */

        /*
         * 获取第一个节点的值的引用
         */
        reference front() {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }

        /*
         * 获取第一个节点的值的引用
         * const 重载
         */
        const_reference front() const {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }

        /*
         * 获取最后一个节点的值的引用
         */
        reference back() {
            TINYSTL_DEBUG(!empty());
            /* 因为尾节点是没有值的，需要向前移动一位才是最后一个节点 */
            return *(--end());
        }

        /*
         * 获取最后一个节点的值的引用
         * const 重载
         */
        const_reference back() const {
            TINYSTL_DEBUG(!empty());
            /* 因为尾节点是没有值的，需要向前移动一位才是最后一个节点 */
            return *(--end());
        }

        /*
         * 对list赋值操作
         */

        /*
         * 对list赋值n个value
         */
        void assign(size_type n, const value_type &value) {
            fill_assign(n, value);
        }

        /*
         * 对list赋值迭代器之间的值
         * 类成员模板
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type= 0>
        void assign(Iter first, Iter last) {
            copy_assign(first, last);
        }

        /*
         * 用初始化列表对list赋值
         */
        void assign(std::initializer_list<value_type> i_list) {
            copy_assign(i_list.begin(), i_list.end());
        }

        /*
         * emplace_front / emplace_back / emplace操作
         */

        /*
         * 在list的头部新建一个节点
         * 类成员模板,可变参数模板,完美转发
         */
        template<typename...Args>
        void emplace_front(Args &&...args) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(tinySTL::forward<Args>(args)...);
            link_nodes_at_front(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        /*
         * 在list的尾部新建一个节点
         * 类成员模板,可变参数模板,完美转发
         */
        template<typename...Args>
        void emplace_back(Args &&...args) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(tinySTL::forward<Args>(args)...);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        /*
         * 在list的指定位置上插入新节点
         * 类成员模板,可变参数模板,完美转发
         */
        template<typename...Args>
        iterator emplace(const_iterator pos, Args &&...args) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(tinySTL::forward<Args>(args)...);
            link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
            ++size_;
            return iterator(link_node);
        }

        /*
         * 插入操作相关函数
         */

        /*
         * 在指定位置前插入一个value节点
         */
        iterator insert(const_iterator pos, const value_type &value) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(value);
            ++size_;
            return link_iter_node(pos, link_node->as_base());
        }

        /*
         * 以移动赋值的方式插入一个节点
         */
        iterator insert(const_iterator pos, value_type &&value) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(tinySTL::move(value));
            ++size_;
            return link_iter_node(pos, link_node->as_base());
        }

        /*
         * 向指定位置前插入n个值
         */
        iterator insert(const_iterator pos, size_type n, const value_type &value) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            return fill_insert(pos, n, value);
        }

        /*
         * 向指定位置前插入迭代器间的值
         * 类成员模板
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type= 0>
        iterator insert(const_iterator pos, Iter first, Iter last) {
            size_type n = tinySTL::distance(first, last);
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            return copy_insert(pos, n, first);
        }

        /*
         * push_front / push_back / pop_front / pop_back
         */

        /*
         * 向list头部插入一个数据
         */
        void push_front(const value_type &value) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(value);
            link_nodes_at_front(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        /*
         * 向list头部插入一个数据
         * 右值，调用emplace_front完成
         */
        void push_front(value_type &&value) {
            emplace_back(tinySTL::move(value));
        }

        /*
         * 向list尾部插入一个数据
         */
        void push_back(const value_type &value) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            node_ptr link_node = create_node(value);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        /*
         * 向list尾部插入一个数据
         * 右值，调用emplace_front完成
         */
        void push_back(value_type &&value) {
            emplace_back(tinySTL::move(value));
        }

        /*
         * 从list头部弹出一个节点
         */
        void pop_front() {
            TINYSTL_DEBUG(!empty());
            base_ptr cur = node_->next;
            unlink_nodes(cur, cur);
            destroy_node(cur->as_node());
            --size_;
        }

        /*
         * 从list头部弹出一个节点
         */
        void pop_back() {
            TINYSTL_DEBUG(!empty());
            base_ptr cur = node_->prev;
            unlink_nodes(cur, cur);
            destroy_node(cur->as_node());
            --size_;
        }

        /*
         * 删除某些节点，或清空整个list相关方法
         */

        /*
         * 删除指定位置节点
         */
        iterator erase(const_iterator pos);

        /*
         * 删除指定区间的节点
         */
        iterator erase(const_iterator first, const_iterator last);

        /*
         * 清除list中所有数据
         */
        void clear();

        /*
         * 改变list大小，交换list中数据
         */

        /*
         * 将list的大小调整为new_size
         */
        void resize(size_type new_size) {
            resize(new_size, value_type());
        }

        /*
         * 将list的大小调整为new_size,默认值为value
         */
        void resize(size_type new_size, const value_type &value);

        /*
         * list的swap操作
         */
        void swap(list &rhs) noexcept {
            /* 交换一下尾节点指针与size_即可 */
            tinySTL::swap(node_, rhs.node_);
            tinySTL::swap(size_, rhs.size_);
        }

        /*
         * list 相关操作
         */

        /*
         * 将 list other 接合于 pos 之前
         */
        void splice(const_iterator pos, list &other);

        /*
         * 将 it 所指的other的节点接合于 pos 之前
         * 这里没有检测自赋值，是因为这个操作可以用在同一个链表上
         */
        void splice(const_iterator pos, list &other, const_iterator it);

        /*
         * 将 list other 的 [first, last) 内的节点接合于 pos 之前
         */
        void splice(const_iterator pos, list &other, const_iterator first, const_iterator last);

        /*
         * 从list中移除值为value的节点
         */
        void remove(const value_type &value) {
            /* 调用remove_if完成实际操作，传入lambda表达式函数对象 */
            remove_if([&](const value_type &v) { return v == value; });
        }

        /*
         * 从list中将另一元操作 pred 为 true 的所有元素移除
         * 函数模板
         */
        template<typename UnaryPred>
        void remove_if(UnaryPred pred);

        /*
         * 去除list中重复的元素,需要在排好序的list中
         */
        void unique() {
            /* 调用unique完成实际操作，传入函数对象 */
            unique(tinySTL::equal_to<value_type>());
        }

        /*
         * 移除 list 中满足 pred 为 true 重复元素,需要在排好序的list中
         * 函数模板
         */
        template<typename BinaryPred>
        void unique(BinaryPred pred);

        /*
         * 合并两条有序链表，默认升序
         */
        void merge(list &other) {
            merge(other, tinySTL::less<value_type>());
        }

        /*
         * 与另一个 list 合并，按照 comp 为 true 的顺序
         * 类成员模板
         */
        template<typename Compared>
        void merge(list &other, Compared comp);

        /*
         * 对list进行排序
         */
        void sort() {
            /* 调用list_sort函数排序，使用默认函数对象less */
            list_sort(begin(), end(), size(), tinySTL::less<T>());
        }

        /*
         * 对list进行排序
         */
        template<typename Compared>
        void sort(Compared comp) {
            /* 调用list_sort函数排序，使用默认函数对象less */
            list_sort(begin(), end(), size(), comp);
        }

        /*
         * 将list逆序排列
         */
        void reverse();

    private:
        /*
         * 私有的辅助方法
         */

        /*
         * 创建或销毁节点相关的方法
         */

        /*
         * 创建一个新的节点
         * 可变参数类成员模板,完美转发
         */
        template<typename...Args>
        node_ptr create_node(Args &&...args);

        /*
         * 销毁指定节点
         */
        void destroy_node(node_ptr p);

        /*
         * 初始化list的相关方法
         */

        /*
         * 初始化一个值为value的大小为n的链表
         */
        void fill_init(size_type n, const value_type &value);

        /*
         * 接受一对迭代器，使用迭代器间的数据创建list
         * 类成员模板
         */
        template<typename Iter>
        void copy_init(Iter first, Iter last);

        /*
         * 连接或断开连接的相关方法
         */

        /*
         * 将节点连接在指定pos节点之前
         */
        iterator link_iter_node(const_iterator pos, base_ptr node);

        /*
         * 将节点连接在指定位置pos前
         */
        void link_nodes(base_ptr pos, base_ptr first, base_ptr last);

        /*
         * 将节点连接在头部
         */
        void link_nodes_at_front(base_ptr first, base_ptr last);

        /*
         * 将节点连接在尾部
         */
        void link_nodes_at_back(base_ptr first, base_ptr last);

        /*
         * 将指定段节点从list上断开连接
         * first->last
         */
        void unlink_nodes(base_ptr first, base_ptr last);

        /*
         * 对list进行赋值操作的相关方法
         */

        /*
         * 对list赋值n个value
         */
        void fill_assign(size_type n, const value_type &value);

        /*
         * 对list赋值迭代器之间的值
         * 类成员模板
         */
        template<typename Iter>
        void copy_assign(Iter first, Iter last);

        /*
         * 向list指定位置前插入n个指定值
         */
        iterator fill_insert(const_iterator pos, size_type n, const value_type &value);

        /*
         * 向指定位置插入迭代器first后的n个数据
         * 类成员模板
         */
        template<typename Iter>
        iterator copy_insert(const_iterator pos, size_type n, Iter first);

        /*
         * 对list中的数据排序
         * 以comp函数对象作为比较方法
         * 函数模板
         */
        template<typename Compared>
        iterator list_sort(iterator first1, iterator last2, size_type n, Compared comp);

    };

    // ========================================================================================

    /*
     * 类成员方法，外部可以调用的方法
     */

    /*
     * 删除指定位置节点
     */
    template<typename T>
    typename list<T>::iterator list<T>::erase(const_iterator pos) {
        /* 检查是否还有数据 */
        TINYSTL_DEBUG(pos != cend());
        base_ptr cur = pos.node_;
        base_ptr next = cur->next;
        unlink_nodes(cur, cur);
        destroy_node(cur->as_node());
        --size_;
        return iterator(next);
    }

    /*
     * 删除指定区间的节点
     */
    template<typename T>
    typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last) {
        if (first != last) {
            unlink_nodes(first.node_, last.node_->prev);
            while (first != last) {
                base_ptr cur = first.node_;
                ++first;
                destroy_node(cur->as_node());
                --size_;
            }
        }
        return iterator(last.node_);
    }

    /*
     * 清除list中所有数据
     */
    template<typename T>
    void list<T>::clear() {
        if (size_ != 0) {
            base_ptr cur = node_->next;
            for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next) {
                destroy_node(cur->as_node());
            }
            /* 断开连接，前后指针指向自身 */
            node_->unlink();
            size_ = 0;
        }
    }

    /*
     * 将list的大小调整为new_size,默认值为value
     */
    template<typename T>
    void list<T>::resize(size_type new_size, const value_type &value) {
        iterator i = begin();
        size_type len = 0;
        while (i != end() && len < new_size) {
            ++i;
            ++len;
        }
        if (len == new_size) {
            erase(i, node_);
        } else {
            insert(node_, new_size - len, value);
        }
    }

    /*
     * 将 list other 接合于 pos 之前
     */
    template<typename T>
    void list<T>::splice(const_iterator pos, list &other) {
        /* 自赋值检测 */
        TINYSTL_DEBUG(this != &other);
        if (!other.empty()) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - other.size_, "list<T>'s size too big");
            base_ptr f = other.node_->next;
            base_ptr l = other.node_->prev;
            /* 解除other对象的链接 */
            other.unlink_nodes(f, l);
            /* 将链表节点连接到此对象中 */
            link_nodes(pos.node_, f, l);

            size_ += other.size_;
            other.size_ = 0;
        }
    }

    /*
     * 将 it 所指的other的节点接合于 pos 之前
     * 这里没有检测自赋值，是因为这个操作可以用在同一个链表上
     */
    template<typename T>
    void list<T>::splice(const_iterator pos, list &other, const_iterator it) {
        /* 确保节点不同，且不相邻 */
        if (pos.node_ != it.node_ && pos.node_ != it.node_->next) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            base_ptr f = it.node_;
            /* 解除other对象的链接 */
            other.unlink_nodes(f, f);
            /* 将链表节点连接到此对象中 */
            link_nodes(pos.node_, f, f);

            ++size_;
            --other.size_;
        }
    }

    /*
     * 将 list other 的 [first, last) 内的节点接合于 pos 之前
     */
    template<typename T>
    void list<T>::splice(const_iterator pos, list &other, const_iterator first, const_iterator last) {
        if (first != last && this != &other) {
            size_type n = tinySTL::distance(first, last);
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            base_ptr f = first.node_;
            base_ptr l = last.node_->prev;

            /* 解除other对象的链接 */
            other.unlink_nodes(f, l);
            /* 将链表节点连接到此对象中 */
            link_nodes(pos.node_, f, l);

            size_ += n;
            other.size_ -= n;
        }
    }

    /*
     * 从list中将另一元操作 pred 为 true 的所有元素移除
     * 函数模板
     */
    template<typename T>
    template<typename UnaryPred>
    void list<T>::remove_if(UnaryPred pred) {
        iterator f = begin();
        iterator l = end();
        for (iterator next = f; f != l; f = next) {
            ++next;
            if (pred(*f)) {
                erase(f);
            }
        }
    }

    /*
     * 移除 list 中满足 pred 为 true 重复元素,需要在排好序的list中
     * 函数模板
     */
    template<typename T>
    template<typename BinaryPred>
    void list<T>::unique(BinaryPred pred) {
        iterator i = begin();
        iterator e = end();
        iterator j = i;
        ++j;
        while (j != e) {
            if (pred(*i, *j)) {
                erase(j);
            } else {
                i = j;
            }
            j = i;
            ++j;
        }
    }

    /*
     * 与另一个 list 合并，按照 comp 为 true 的顺序
     * 类成员模板
     */
    template<typename T>
    template<typename Compared>
    void list<T>::merge(list &other, Compared comp) {
        if (this != &other) {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - other.size_, "list<T>'s size too big");

            iterator first1 = begin();
            iterator last1 = end();
            iterator first2 = other.begin();
            iterator last2 = other.end();

            while (first1 != last1 && first2 != last2) {
                if (comp(*first2, *first1)) {
                    /* 使 comp 为 true 的一段区间 */
                    iterator next = first2;
                    ++next;
                    for (; next != last2 && comp(*next, *first1); ++next);
                    base_ptr f = first2.node_;
                    base_ptr l = next.node_->prev;
                    first2 = next;

                    /* 链接这一段区间,先从other中断开链接 */
                    other.unlink_nodes(f, l);
                    /* 将这一段区间连接到此对象中 */
                    link_nodes(first1.node_, f, l);
                    ++first1;
                } else {
                    ++first1;
                }
            }

            /* 链接剩余部分 */
            if (first2 != last2) {
                base_ptr f = first2.node_;
                base_ptr l = last2.node_->prev;
                /* 先从other中断开链接 */
                other.unlink_nodes(f, l);
                /* 将这一段区间连接到此对象中 */
                link_nodes(last1.node_, f, l);
            }

            size_ += other.size_;
            other.size_ = 0;
        }
    }

    /*
     * 将list逆序排列
     */
    template<typename T>
    void list<T>::reverse() {
        if (size_ <= 1) {
            return;
        }
        iterator i = begin();
        iterator e = end();
        while (i.node_ != e.node_) {
            /* 交换节点中的指向上一个节点指针(i.node_->prev)的值与指向下一个节点指针(i.node_->next)的值即可 */
            tinySTL::swap(i.node_->prev, i.node_->next);
            i.node_ = i.node_->prev;
        }
        tinySTL::swap(e.node_->prev, e.node_->next);
    }

    // =========================================================================================

    /*
     * 私有的辅助方法
     */

    /*
     * 创建一个新的节点
     * 可变参数类成员模板,完美转发
     */
    template<typename T>
    template<typename...Args>
    typename list<T>::node_ptr list<T>::create_node(Args &&...args) {
        node_ptr p = node_allocator::allocate(1);
        try {
            data_allocator::construct(tinySTL::address_of(p->value), tinySTL::forward<Args>(args)...);
            p->prev = nullptr;
            p->next = nullptr;
        }
        catch (...) {
            node_allocator::deallocate(p);
            throw;
        }
        return p;
    }

    /*
     * 销毁指定节点
     */
    template<typename T>
    void list<T>::destroy_node(node_ptr p) {
        data_allocator::destroy(tinySTL::address_of(p->value));
        node_allocator::deallocate(p);
    }

    /*
     * 初始化一个值为value的大小为n的链表
     */
    template<typename T>
    void list<T>::fill_init(size_type n, const value_type &value) {
        /* 这里使用的是base_allocator，因为list的为节点不需要数据域 */
        node_ = base_allocator::allocate(1);
        /* 前后指针指向自身 */
        node_->unlink();
        size_ = n;
        try {
            for (; n > 0; --n) {
                node_ptr node = create_node(value);
                link_nodes_at_back(node->as_base(), node->as_base());
            }
        }
        catch (...) {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

    /*
     * 接受一对迭代器，使用迭代器间的数据创建list
     * 类成员模板
     */
    template<typename T>
    template<typename Iter>
    void list<T>::copy_init(Iter first, Iter last) {
        /* 这里使用的是base_allocator，因为list的为节点不需要数据域 */
        node_ = base_allocator::allocate(1);
        /* 前后指针指向自身 */
        node_->unlink();
        size_type n = tinySTL::distance(first, last);
        size_ = n;
        try {
            for (; n > 0; --n, ++first) {
                node_ptr node = create_node(*first);
                link_nodes_at_back(node->as_base(), node->as_base());
            }
        }
        catch (...) {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

    /*
     * 将节点连接在指定pos节点之前
     */
    template<typename T>
    typename list<T>::iterator list<T>::link_iter_node(const_iterator pos, base_ptr node) {
        if (pos.node_ == node_->next) {
            /* 如果pos为list的头，调用link_nodes_at_front方法 */
            link_nodes_at_front(node, node);
        } else if (pos.node_ == node_) {
            /* 如果pos为list的尾，调用link_nodes_at_back方法 */
            link_nodes_at_back(node, node);
        } else {
            /* 其余位置调用link_nodes方法 */
            link_nodes(pos.node_, node, node);
        }
        return iterator(node);
    }

    /*
     * 将节点连接在指定位置pos前
     */
    template<typename T>
    void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
        pos->prev->next = first;
        first->prev = pos->prev;
        pos->prev = last;
        last->next = pos;
    }

    /*
     * 将节点连接在头部
     */
    template<typename T>
    void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
        first->prev = node_;
        last->next = node_->next;
        last->next->prev = last;
        node_->next = first;
    }

    /*
     * 将一段节点连接在尾部
     */
    template<typename T>
    void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
        last->next = node_;
        first->prev = node_->prev;
        first->prev->next = first;
        last->next->prev = last;
    }

    /*
     * 将指定段节点从list上断开连接
     * first->last
     */
    template<typename T>
    void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
        first->prev->next = last->next;
        last->next->prev = first->prev;
    }

    /*
     * 对list赋值n个value
     */
    template<typename T>
    void list<T>::fill_assign(size_type n, const value_type &value) {
        iterator i = begin();
        iterator e = end();
        /* 在原有的空间上赋值 */
        for (; n > 0 && i != e; --n, ++i) {
            *i = value;
        }
        if (n > 0) {
            /* 空间不够则插入剩余数量新节点 */
            insert(e, n, value);
        } else {
            /* 原空间比需求的多，删除多余的空间 */
            erase(i, e);
        }
    }

    /*
     * 对list赋值迭代器之间的值
     * 类成员模板
     */
    template<typename T>
    template<typename Iter>
    void list<T>::copy_assign(Iter first, Iter last) {
        iterator f1 = begin();
        iterator l1 = end();
        /* 在原有的空间上赋值 */
        for (; f1 != l1 && first != last; ++f1, ++first) {
            *f1 = *first;
        }
        if (first != last) {
            /* 空间不够则插入剩余数量新节点 */
            insert(l1, first, last);
        } else {
            /* 原空间比需求的多，删除多余的空间 */
            erase(f1, l1);
        }
    }

    /*
     * 向list指定位置前插入n个指定值
     */
    template<typename T>
    typename list<T>::iterator list<T>::fill_insert(const_iterator pos, size_type n, const value_type &value) {
        iterator r(pos.node_);
        if (n != 0) {
            const size_type add_size = n;
            node_ptr node = create_node(value);
            node->prev = nullptr;
            r = iterator(node);
            iterator end = r;
            try {
                /* 前面已经创建了一个节点，还需 n - 1 个 */
                for (--n; n > 0; --n, ++end) {
                    /* 创建节点 */
                    node_ptr next = create_node(value);
                    /* 连接节点 */
                    end.node_->next = next->as_base();
                    next->prev = end.node_;
                }
                size_ += add_size;
            }
            catch (...) {
                base_ptr e_node = end.node_;
                while (true) {
                    base_ptr prev = e_node->prev;
                    destroy_node(e_node->as_node());
                    if (prev == nullptr) {
                        break;
                    }
                    e_node = prev;
                }
                throw;
            }
            /* 将刚才创建的一段链表连接到list的指定位置前去 */
            link_nodes(pos.node_, r.node_, end.node_);
        }
        return r;
    }

    /*
     * 向指定位置插入迭代器first后的n个数据
     * 类成员模板
     */
    template<typename T>
    template<typename Iter>
    typename list<T>::iterator list<T>::copy_insert(const_iterator pos, size_type n, Iter first) {
        iterator r(pos.node_);
        if (n != 0) {
            const size_type add_size = n;
            node_ptr node = create_node(*first);
            node->prev = nullptr;
            r = iterator(node);
            iterator end = r;
            try {
                /* 前面已经创建了一个节点，还需 n - 1 个 */
                for (--n, ++first; n > 0; --n, ++first, ++end) {
                    /* 创建节点 */
                    node_ptr next = create_node(*first);
                    end.node_->next = next->as_base();
                    next->prev = end.node_;
                }
                size_ += add_size;
            }
            catch (...) {
                base_ptr e_node = end.node_;
                while (true) {
                    base_ptr prev = e_node->prev;
                    destroy_node(e_node->as_node());
                    if (prev == nullptr) {
                        break;
                    }
                    e_node = prev;
                }
                throw;
            }
            /* 将刚才创建的一段链表连接到list的指定位置前去 */
            link_nodes(pos.node_, r.node_, end.node_);
        }
        return r;
    }

    /*
     * 对list中的数据排序
     * 以comp函数对象作为比较方法
     * 函数模板
     */
    template<typename T>
    template<typename Compared>
    typename list<T>::iterator list<T>::list_sort(iterator first1, iterator last2, size_type n, Compared comp) {
        if (n < 2) {
            return first1;
        }
        if (n == 2) {
            if (comp(*--last2, *first1)) {
                base_ptr ln = last2.node_;
                unlink_nodes(ln, ln);
                link_nodes(first1.node_, ln, ln);
                return last2;
            }
            return first1;
        }

        /* 将list分为两个部分 */
        size_type n2 = n / 2;
        iterator last1 = first1;
        tinySTL::advance(last1, n2);
        /* 前半段的最小位置 */
        iterator result = first1 = list_sort(first1, last1, n2, comp);
        /* 后半段的最小位置 */
        iterator first2 = last1 = list_sort(last1, last2, n - n2, comp);

        /* 把较小的一段区间移到前面 */
        if (comp(*first2, *first1)) {
            iterator m = first2;
            ++m;
            for (; m != last2 && comp(*m, *first1); ++m);
            base_ptr f = first2.node_;
            base_ptr l = m.node_->prev;
            result = first2;
            last1 = first2 = m;
            unlink_nodes(f, l);
            m = first1;
            ++m;
            link_nodes(first1.node_, f, l);
            first1 = m;
        } else {
            ++first1;
        }

        /* 合并两段有序区间 */
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                iterator m = first2;
                ++m;
                for (; m != last2 && comp(*m, *first1); ++m);
                base_ptr f = first2.node_;
                base_ptr l = m.node_->prev;
                if (last1 == first2) {
                    last1 = m;
                }
                first2 = m;
                unlink_nodes(f, l);
                m = first1;
                ++m;
                link_nodes(first1.node_, f, l);
                first1 = m;
            } else {
                ++first1;
            }
        }
        return result;
    }

    // ==============================================================================================

    /*
     * 全局区域重载比较操作符
     */

    /*
     * 重载等于比较操作符
     */
    template<typename T>
    bool operator==(const list<T> &lhs, const list<T> &rhs) {
        auto first1 = lhs.cbegin();
        auto last1 = lhs.cend();
        auto first2 = rhs.cbegin();
        auto last2 = rhs.cend();
        for (; first1 != last1 && first2 != last2 && *first1 == *first2; ++first1, ++first2);
        return first1 == last1 && first2 == last2;
    }

    /*
     * 重载不等于比较操作符
     */
    template<typename T>
    bool operator!=(const list<T> &lhs, const list<T> &rhs) {
        /* 调用等于操作符完成 */
        return !(lhs == rhs);
    }

    /*
     * 小于操作符
     */
    template<typename T>
    bool operator<(const list<T> &lhs, const list<T> &rhs) {
        return tinySTL::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /*
     * 大于操作符
     */
    template<typename T>
    bool operator>(const list<T> &lhs, const list<T> &rhs) {
        /* 调用小于操作符完成 */
        return rhs < lhs;
    }

    /*
     * 小于等于操作符
     */
    template<typename T>
    bool operator<=(const list<T> &lhs, const list<T> &rhs) {
        /* 调用小于操作符完成 */
        return !(rhs < lhs);
    }

    /*
     * 大于等于操作符
     */
    template<typename T>
    bool operator>=(const list<T> &lhs, const list<T> &rhs) {
        /* 调用小于操作符完成 */
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename T>
    void swap(list<T> &lhs, list<T> &rhs) noexcept {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_LIST_H
