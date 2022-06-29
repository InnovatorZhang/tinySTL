//
// Created by cqupt1811 on 2022/5/12.
//

/*
 * 这个头文件包含了一个模板类 deque
 * deque: 双端队列
 * notes:
 * 异常保证：
 * tinySTL::deque<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
 *      * emplace_front
 *      * emplace_back
 *      * emplace
 *      * push_front
 *      * push_back
 *      * insert
 */

#ifndef TINYSTL_DEQUE_H
#define TINYSTL_DEQUE_H

#include <initializer_list>  /* std::initializer_list<T> 类型对象是一个访问 const T 类型对象数组的轻量代理对象 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "memory.h"  /* 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */
#include "exceptdef.h"  /* 此文件中定义了异常相关的宏 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

/*
 * 取消编译器中定义好的min与max，因为需要使用自己定义的
 */
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

/*
 * deque的map初始值大小，默认为8
 */
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

    // =============================================================================================

    /*
     * 使用模板的形式定义deque每一个连续数据存放区域的大小
     */
    template<typename T>
    struct deque_buf_size {
        /* 如果类型T的大小小于256，缓存区大小为 4096除以类别大小，否则为16个类对象大小 */
        static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
    };

    // ==============================================================================================

    /*
     * deque 的迭代器设计
     * 继承
     * template<class Category, class T, class Distance = ptrdiff_t,
     * class Pointer = T *, class Reference = T &>
     * struct iterator{ ... }
     * 可以自动定义迭代器所需的五种类型
     * deque 的迭代器属于随机访问迭代器类型
     */
    template<typename T, typename Ref, typename Ptr>
    struct deque_iterator : public iterator<tinySTL::random_access_iterator_tag, T> {
        /*
         * 定义类中使用的类型别名
         */
        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef deque_iterator self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *value_pointer;
        typedef T **map_pointer;

        /*
         * 设置deque中每一个连续数据存放区域的大小
         */
        static const size_type buffer_size = deque_buf_size<T>::value;

        /*
         * 迭代器所含成员数据
         * 每一个迭代器有四个成员
         * cur: 指向所在缓冲区的当前元素
         * first: 指向所在缓冲区的头部
         * last: 指向所在缓冲区的尾部
         * node: 缓冲区所在节点,二级指针
         */
        value_pointer cur;
        value_pointer first;
        value_pointer last;
        map_pointer node;

        /*
         * 默认构造函数
         * 所有变量置为空指针
         */
        deque_iterator() noexcept:
                cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {

        }

        /*
         * 有参构造函数，接受一个当前节点指针与map_pointer指针
         * 可以根据这两个信息推算出其他变量信息
         */
        deque_iterator(value_pointer v, map_pointer n) :
                cur(v), first(*n), last(*n + buffer_size), node(n) {

        }

        /*
         * 拷贝构造函数
         */
        deque_iterator(const iterator &rhs) :
                cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {

        }

        /*
         * 移动构造函数
         */
        deque_iterator(iterator &&rhs) :
                cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
            rhs.cur = nullptr;
            rhs.first = nullptr;
            rhs.last = nullptr;
            rhs.node = nullptr;
        }

        /*
         * 接受一个const_iterator的构造函数
         */
        deque_iterator(const const_iterator &rhs) :
                cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {

        }

        /*
         * 拷贝赋值函数
         */
        self &operator=(const iterator &rhs) {
            if (this != &rhs) {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        /*
         * 跳转转到另外一个缓冲区
         */
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + buffer_size;
        }

        /*
         * 重载 * 运算符
         */
        reference operator*() const {
            return *cur;
        }

        /*
         * 重载 -> 运算符
         */
        pointer operator->() const {
            return cur;
        }

        /*
         * 成员函数形式 - 运算符重载
         */
        difference_type operator-(const self &other) const {
            return static_cast<difference_type>(buffer_size) * (node - other.node) + (cur - first) -
                   (other.cur - other.first);
        }

        /*
         * 重载前置 ++ 运算符
         */
        self &operator++() {
            ++cur;
            /* 判断是否到达了本缓冲区的尾部 */
            if (cur == last) {
                /* 如果到达了尾部，需要切换到下一个缓冲区上去 */
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        /*
         * 重载后置 ++ 运算符
         */
        self operator++(int) {
            self temp = *this;
            /* 调用前置++ */
            ++*this;
            return temp;
        }

        /*
         * 重载前置 -- 运算符
         */
        self &operator--() {
            /* 判断是否在缓冲区的最前端 */
            if (cur == first) {
                /* 如果到达了头部，切换到前一个缓冲区上去 */
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        /*
         * 重载后置 -- 运算符
         */
        self operator--(int) {
            self temp = *this;
            /* 调用前置-- */
            --*this;
            return temp;
        }

        /*
         * 重载 += 运算符
         */
        self &operator+=(difference_type n) {
            const difference_type offset = n + (cur - first);
            /* 判断跳转到哪个缓冲区 */
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
                /* 仍在当前缓冲区 */
                cur += n;
            } else {
                /* 要跳到其他的缓冲区 */
                const difference_type node_offset = offset > 0
                                                    ? offset / static_cast<difference_type>(buffer_size)
                                                    : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
            }
            return *this;
        }

        /*
         * 重载 + 运算符
         */
        self operator+(difference_type n) const {
            self temp = *this;
            return temp += n;
        }

        /*
         * 重载 -= 运算符
         */
        self &operator-=(difference_type n) {
            return *this += -n;
        }

        /*
         * 重载 - 运算符
         */
        self operator-(difference_type n) const {
            self temp = *this;
            return temp -= n;
        }

        /*
         * 重载 [] 运算符
         */
        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        /*
         * 重载相等于操作符
         */
        bool operator==(const self &rhs) const {
            return cur == rhs.cur;
        }

        /*
         * 重载不相等于操作符
         */
        bool operator!=(const self &rhs) const {
            return !(*this == rhs);
        }

        /*
         * 重载小于运算符
         */
        bool operator<(const self &rhs) const {
            return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
        }

        /*
         * 重载大于运算符
         */
        bool operator>(const self &rhs) const {
            return rhs < *this;
        }

        /*
         * 重载小于等于操作符
         */
        bool operator<=(const self &rhs) const {
            return !(rhs < *this);
        }

        /*
         * 重载大于等于操作符
         */
        bool operator>=(const self &rhs) const {
            return !(*this < rhs);
        }
    };

    // ================================================================================================

    /*
     * 模板类 deque
     * 模板参数代表数据类型
     */
    template<typename T>
    class deque {
    public:
        /*
         * 定义空间配置器的别名
         */
        typedef tinySTL::allocator<T> allocator_type;
        typedef tinySTL::allocator<T> data_allocator;
        typedef tinySTL::allocator<T *> map_allocator;

        /*
         * deque类中别名定义
         */
        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;
        typedef pointer *map_pointer;
        typedef const_pointer *const_map_pointer;

        /*
         * 迭代器相关别名定义
         */
        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef tinySTL::reverse_iterator<iterator> reverse_iterator;
        typedef tinySTL::reverse_iterator<const_iterator> const_reverse_iterator;

        /*
         * 返回所使用的空间配置器对象
         */
        allocator_type get_allocator() {
            return data_allocator();
        }

        /*
         * 设置deque中每一个连续数据存放区域的大小
         */
        static const size_type buffer_size = deque_buf_size<T>::value;

    private:
        /*
         * 使用一下四个数据描述一个 deque
         * begin_: deque的头部位置迭代器
         * end_: deque的尾部位置迭代器
         * map_: 指向一块 map，map 中的每个元素都是一个指针，指向一个缓冲区
         * map_size_: map 内指针的数目，也就是map的大小
         */
        iterator begin_;
        iterator end_;
        map_pointer map_;
        size_type map_size_;

    public:
        /*
         * 默认构造函数
         */
        deque() {
            fill_init(0, value_type());
        }

        /*
         * 显式有参构造函数
         */
        explicit deque(size_type n) {
            fill_init(n, value_type());
        }

        /*
         * 有参构造函数，构造大小为n，值为value的deque
         */
        deque(size_type n, const value_type &value) {
            fill_init(n, value);
        }

        /*
         * 有参构造函数
         * 接收一对迭代器，使用迭代器间的数据建立deque
         * 迭代器类型至少为input_iterator_tag类型
         * 类成员模板
         */
        template<typename InputIter, typename std::enable_if<
                tinySTL::is_input_iterator<InputIter>::value, int>::type = 0>
        deque(InputIter first, InputIter last) {
            copy_init(first, last, tinySTL::iterator_category(first));
        }

        /*
         * 构造函数 使用初始化列表，即支持以{}的方式构建对象
         */
        deque(std::initializer_list<value_type> i_list) {
            copy_init(i_list.begin(), i_list.end(), tinySTL::forward_iterator_tag());
        }

        /*
         * 拷贝构造
         */
        deque(const deque &rhs) {
            copy_init(rhs.begin(), rhs.end(), tinySTL::forward_iterator_tag());
        }

        /*
         * 移动构造
         */
        deque(deque &&rhs) noexcept
                : begin_(tinySTL::move(rhs.begin_)), end_(tinySTL::move(rhs.end_)),
                  map_(rhs.map_), map_size_(rhs.map_size_) {
            rhs.map_ = nullptr;
            rhs.map_size_ = 0;
        }

        /*
         * 拷贝赋值函数
         */
        deque &operator=(const deque &rhs);

        /*
         * 移动赋值函数
         */
        deque &operator=(deque &&rhs);

        /*
         * 使用初始化列表赋值函数
         */
        deque &operator=(std::initializer_list<value_type> i_list) {
            deque temp(i_list);
            swap(temp);
            return *this;
        }

        /*
         * 析构函数
         */
        ~deque() {
            if (map_ != nullptr) {
                clear();
                data_allocator::deallocate(*begin_.node, buffer_size);
                *begin_.node = nullptr;
                map_allocator::deallocate(map_, map_size_);
                map_ = nullptr;
            }
        }

    public:
        /*
         * 迭代器相关操作
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return begin_;
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return begin_;
        }

        /*
         * 获取尾部迭代器
         */
        iterator end() noexcept {
            return end_;
        }

        /*
         * 获取尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return end_;
        }

        /*
         * 获取头部反向迭代器
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 获取头部反向迭代器 const重载
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 获取尾部反向迭代器
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 获取尾部反向迭代器 const重载
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
         * 返回常量反向头部迭代器
         */
        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        /*
         * 返回常量反向尾部迭代器
         */
        const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /*
         * 容量相关操作
         */

        /*
         * deque是否为空
         */
        bool empty() const noexcept {
            return begin() == end();
        }

        /*
         * 获取deque元素的个数
         */
        size_type size() const noexcept {
            return end_ - begin_;
        }

        /*
         * 返回最大承受容量
         */
        size_type max_size() const noexcept {
            return static_cast<size_type>(-1);
        }

        /*
         * 将deque大小调整为new_size
         */
        void resize(size_type new_size) {
            resize(new_size, value_type());
        }

        /*
         * 将deque大小调整为new_size 填充的值为value
         */
        void resize(size_type new_size, const value_type &value);

        /*
         * 减小容器容量
         * 至少会留下头部缓冲区
         */
        void shrink_to_fit() noexcept;

        /*
         * 访问元素相关操作
         */

        /*
         * 操作符 [] 重载
         */
        reference operator[](size_type n) {
            TINYSTL_DEBUG(n < size());
            return begin_[n];
        }

        /*
         * 操作符 [] 重载 const版本
         */
        const_reference operator[](size_type n) const {
            TINYSTL_DEBUG(n < size());
            return begin_[n];
        }

        /*
         * 获取位置为n的元素
         */
        reference at(size_type n) {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
            return (*this)[n];
        }

        /*
         * 获取位置为n的元素 const版本
         */
        const_reference at(size_type n) const {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
            return (*this)[n];
        }

        /*
         * 获取第一个元素
         */
        reference front() {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }

        /*
         * 获取第一个元素 const重载
         */
        const_reference front() const {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }

        /*
         * 获取最后一个元素
         */
        reference back() {
            TINYSTL_DEBUG(!empty());
            return *(end() - 1);
        }

        /*
         * 获取最后一个元素 const重载
         */
        const_reference back() const {
            TINYSTL_DEBUG(!empty());
            return *(end() - 1);
        }

        /*
         * 修改容器的相关操作
         */

        /*
         * 给deque赋值n个value
         */
        void assign(size_type n, const value_type &value) {
            fill_assign(n, value);
        }

        /*
         * 给deque赋值迭代器之间的值
         * 迭代器至少为input_iterator_tag类
         */
        template<typename InputIter, typename std::enable_if<
                tinySTL::is_input_iterator<InputIter>::value, int>::type = 0>
        void assign(InputIter first, InputIter last) {
            copy_assign(first, last, tinySTL::iterator_category(first));
        }

        /*
         * 使用初始化列表给deque赋值
         */
        void assign(std::initializer_list<value_type> i_list) {
            copy_assign(i_list.begin(), i_list.end(), tinySTL::forward_iterator_tag{});
        }

        /*
         * emplace_front / emplace_back / emplace 操作
         */

        /*
         * 使用传入的参数在deque的头部构建一个对象
         * 类成员模板，可变参数，完美转发
         */
        template<typename...Args>
        void emplace_front(Args &&...args);

        /*
         * 使用传入的参数在deque的尾部构建一个对象
         * 类成员模板，可变参数，完美转发
         */
        template<typename...Args>
        void emplace_back(Args &&...args);

        /*
         * 使用传入的参数在deque的指定位置pos前构建一个对象
         * 类成员模板，可变参数，完美转发
         */
        template<typename...Args>
        iterator emplace(iterator pos, Args &&...args);

        /*
         * push相关操作
         */

        /*
         * 向队头加入一个数据
         */
        void push_front(const value_type &value);

        /*
         * 向队尾巴加入一个数据
         */
        void push_back(const value_type &value);

        /*
         * 向队头加入一个数据 移动语义
         */
        void push_front(value_type &&value) {
            /* 调用emplace_front即可 */
            emplace_front(tinySTL::move(value));
        }

        /*
         * 向队尾巴加入一个数据  移动语义
         */
        void push_back(value_type &&value) {
            /* 调用emplace_backt即可 */
            emplace_back(tinySTL::move(value));
        }

        /*
         * pop_back / pop_front操作
         */

        /*
         * 弹出第一个元素
         */
        void pop_front();

        /*
         * 弹出最后一个元素
         */
        void pop_back();

        /*
         * 插入数据相关操作
         */

        /*
         * 在指定位置前插入数据
         */
        iterator insert(iterator position, const value_type &value);

        /*
         * 在指定位置前插入数据,移动语义版本
         */
        iterator insert(iterator position, value_type &&value);

        /*
         * 在指定位置前插入n个相同数据
         */
        void insert(iterator position, size_type n, const value_type &value);

        /*
         * 在指定位置前插入迭代器间的数据
         * 至少为input_iterator_tag类型
         * 类成员模板
         */
        template<typename InputIter, typename std::enable_if<
                tinySTL::is_input_iterator<InputIter>::value, int>::type = 0>
        void insert(iterator position, InputIter first, InputIter last) {
            insert_dispatch(position, first, last, tinySTL::iterator_category(first));
        }

        /*
         * 删除或清空元素相关操作
         */

        /*
         * 删除指定位置的元素
         */
        iterator erase(iterator position);

        /*
         * 删除指定区间的元素
         */
        iterator erase(iterator first, iterator last);

        /*
         * 清空deque中的元素
         */
        void clear();

        /*
         * 交换两个deque
         */
        void swap(deque &rhs) noexcept;

    private:
        /*
         * 创建或销毁 buffer 与 map 相关函数
         */

        /*
         * 创建deque中的map结构
         */
        map_pointer create_map(size_type size);

        /*
         * 创建n_start与n_finish指定范围内的buffer区域
         */
        void create_buffer(map_pointer n_start, map_pointer n_finish);

        /*
         * 销毁n_start与n_finish指定范围内的buffer区域
         */
        void destroy_buffer(map_pointer n_start, map_pointer n_finish);

        /*
         * 初始化deque相关函数
         */

        /*
         * 初始化deque中的map，n_elem 是元素个数
         */
        void map_init(size_type n_elem);

        /*
         * 给deque中初始化为n个value值
         */
        void fill_init(size_type n, const value_type &value);

        /*
         * 接收一对迭代器，使用迭代器间的数据初始化deque
         * input_iterator_tag 版本
         * 类成员模板
         */
        template<typename InputIter>
        void copy_init(InputIter first, InputIter last, tinySTL::input_iterator_tag);

        /*
         * 接收一对迭代器，使用迭代器间的数据初始化deque
         * forward_iterator_tag 版本
         * 类成员模板
         */
        template<typename ForwardIter>
        void copy_init(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag);

        /*
         * 赋值操作相关函数
         */

        /*
         * 对deque赋值n个value
         */
        void fill_assign(size_type n, const value_type &value);

        /*
         * 对deque赋值迭代器之间的数据
         * 类成员模板
         * input_iterator_tag版本
         */
        template<typename InputIter>
        void copy_assign(InputIter first, InputIter last, tinySTL::input_iterator_tag);

        /*
         * 对deque赋值迭代器之间的数据
         * 类成员模板
         * input_iterator_tag版本
         */
        template<typename ForwardIter>
        void copy_assign(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag);

        /*
         * 插入操作相关函数
         */

        /*
         * 在指定位置前构建插入一个元素
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename...Args>
        iterator insert_aux(iterator position, Args &&...args);

        /*
         * 在指定位置前插入n个value
         */
        void fill_insert(iterator position, size_type n, const value_type &value);

        /*
         * 在指定位置前插入迭代器间的数据
         * 类成员模板
         */
        template<typename ForwardIter>
        void copy_insert(iterator position, ForwardIter first, ForwardIter last, size_type n);

        /*
         * 将迭代器间的数据插入到指定位置前
         * input_iterator_tag版本
         * 类成员函数模板
         */
        template<typename InputIter>
        void insert_dispatch(iterator position, InputIter first, InputIter last, tinySTL::input_iterator_tag);

        /*
         * 将迭代器间的数据插入到指定位置前
         * forward_iterator_tag版本
         * 类成员函数模板
         */
        template<typename ForwardIter>
        void insert_dispatch(iterator position, ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag);

        /*
         * 分配空间相关函数
         */

        /*
         * 在deque头部分配n个空间
         */
        void require_capacity(size_type n, bool front);

        /*
         * 重新分配空间，因为头部空间不足
         * need表示需要多少头部空间(以buffer为单位)
         */
        void reallocate_map_at_front(size_type need_buffer);

        /*
         * 重新分配空间，因为尾部空间不足
         * need表示需要多少尾部空间(以buffer为单位)
         */
        void reallocate_map_at_back(size_type need_buffer);

    };

    // ==========================================================================================

    /*
     * 类成员函数
     */

    /*
     * 拷贝赋值函数
     */
    template<typename T>
    deque<T> &deque<T>::operator=(const deque &rhs) {
        if (this != &rhs) {
            const size_type len = size();
            if (len >= rhs.size()) {
                /* 删除掉多余部分 */
                erase(tinySTL::copy(rhs.begin_, rhs.end_, begin_), end_);
            } else {
                /* 首先赋值相同的长度段，然后多余的使用插入 */
                iterator mid = rhs.begin() + static_cast<difference_type>(len);
                tinySTL::copy(rhs.begin_, mid, begin_);
                insert(end_, mid, rhs.end_);
            }
        }
        return *this;
    }

    /*
     * 移动赋值函数
     */
    template<typename T>
    deque<T> &deque<T>::operator=(deque &&rhs) {
        clear();
        /* 因为clear会留下头部buffer这里需要手动释放掉 */
        data_allocator::deallocate(*begin_.node, buffer_size);
        begin_ = tinySTL::move(rhs.begin_);
        end_ = tinySTL::move(rhs.end_);
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
        return *this;
    }

    /*
     * 将deque大小调整为new_size 填充的值为value
     */
    template<typename T>
    void deque<T>::resize(size_type new_size, const value_type &value) {
        const size_type len = size();
        if (new_size < len) {
            /* 删除后面部分多余的元素 */
            erase(begin_ + new_size, end_);
        } else {
            /* 向后面部分插入value */
            insert(end_, new_size - len, value);
        }
    }

    /*
     * 减小容器容量
     * 至少会留下头部缓冲区
     */
    template<typename T>
    void deque<T>::shrink_to_fit() noexcept {
        /* 至少会留下头部缓冲区 */
        for (map_pointer cur = map_; cur < begin_.node; ++cur) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        for (map_pointer cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    /*
     * 使用传入的参数在deque的头部构建一个对象
     * 类成员模板，可变参数，完美转发
     */
    template<typename T>
    template<typename...Args>
    void deque<T>::emplace_front(Args &&...args) {
        /* 检查是否在当前buffer区间的第一个位置 */
        if (begin_.cur != begin_.first) {
            /* 不在当前buffer的第一个位置上，直接构造即可 */
            --begin_.cur;
            data_allocator::construct(begin_.cur, tinySTL::forward<Args>(args)...);
        } else {
            /* 在当前buffer的第一个位置上，,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做*/
            require_capacity(1, true);
            try {
                --begin_;
                data_allocator::construct(begin_.cur, tinySTL::forward<Args>(args)...);
            }
            catch (...) {
                ++begin_;
                throw;
            }
        }
    }

    /*
     * 使用传入的参数在deque的尾部构建一个对象
     * 类成员模板，可变参数，完美转发
     */
    template<typename T>
    template<typename...Args>
    void deque<T>::emplace_back(Args &&...args) {
        /* 检查是否在当前buffer区间的最后一个位置 */
        if (end_.cur != end_.last - 1) {
            /* 不在当前buffer的最后一个位置上，直接构造即可 */
            data_allocator::construct(end_.cur, tinySTL::forward<Args>(args)...);
            ++end_.cur;
        } else {
            /* 在当前buffer的最后一个位置上,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做 */
            require_capacity(1, false);
            data_allocator::construct(end_.cur, tinySTL::forward<Args>(args)...);
            ++end_;
        }
    }

    /*
     * 使用传入的参数在deque的指定位置pos前构建一个对象
     * 类成员模板，可变参数，完美转发
     */
    template<typename T>
    template<typename...Args>
    typename deque<T>::iterator deque<T>::emplace(iterator pos, Args &&...args) {
        if (pos.cur == end_.cur) {
            /* 在尾部可以直接调用emplace_front函数 */
            emplace_back(tinySTL::forward<Args>(args)...);
            return end_ - 1;
        } else if (pos.cur == begin_.cur) {
            /* 在第一个位置前构建可以直接调用emplace_front函数 */
            emplace_front(tinySTL::forward<Args>(args)...);
            return begin_;
        }
        /* 中间位置调用insert_aux函数 */
        return insert_aux(pos, tinySTL::forward<Args>(args)...);
    }

    /*
     * 向队头加入一个数据
     */
    template<typename T>
    void deque<T>::push_front(const value_type &value) {
        /* 检查是否在当前buffer区间的第一个位置 */
        if (begin_.cur != begin_.first) {
            /* 不在当前buffer的第一个位置上，直接构造即可 */
            --begin_.cur;
            data_allocator::construct(begin_.cur, value);
        } else {
            /* 在当前buffer的第一个位置上，,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做*/
            require_capacity(1, true);
            try {
                --begin_;
                data_allocator::construct(begin_.cur, value);
            }
            catch (...) {
                ++begin_;
                throw;
            }
        }
    }

    /*
     * 向队尾巴加入一个数据
     */
    template<typename T>
    void deque<T>::push_back(const value_type &value) {
        /* 检查是否在当前buffer区间的最后一个位置 */
        if (end_.cur != end_.last - 1) {
            /* 不在当前buffer的最后一个位置上，直接构造即可 */
            data_allocator::construct(end_.cur, value);
            ++end_.cur;
        } else {
            /* 在当前buffer的最后一个位置上,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做 */
            require_capacity(1, false);
            data_allocator::construct(end_.cur, value);
            ++end_;
        }
    }

    /*
     * 弹出第一个元素
     */
    template<typename T>
    void deque<T>::pop_front() {
        TINYSTL_DEBUG(!empty());
        if (begin_.cur != begin_.last - 1) {
            /* 如果不在当前缓冲区的最后一个位置上 */
            data_allocator::destroy(begin_.cur);
            ++begin_.cur;
        } else {
            /* 如果在当前缓冲区的最后一个位置上 */
            data_allocator::destroy(begin_.cur);
            ++begin_;
            destroy_buffer(begin_.node - 1, begin_.node - 1);
        }
    }

    /*
     * 弹出最后一个元素
     */
    template<typename T>
    void deque<T>::pop_back() {
        TINYSTL_DEBUG(!empty());
        if (end_.cur != end_.first) {
            /* 如果不在当前缓冲区的第一个位置上 */
            --end_.cur;
            data_allocator::destroy(end_.cur);
        } else {
            /* 如果在当前缓冲区的第一个位置上 */
            --end_;
            data_allocator::destroy(end_.cur);
            destroy_buffer(end_.node + 1, end_.node + 1);
        }
    }

    /*
     * 在指定位置前插入数据
     */
    template<typename T>
    typename deque<T>::iterator deque<T>::insert(iterator position, const value_type &value) {
        if (position.cur == end_.cur) {
            /* 如果插入位置在尾部,直接调用push_front即可 */
            push_back(value);
            iterator temp = end_;
            --temp;
            return temp;
        } else if (position.cur == begin_.cur) {
            /* 如果插入位置在头部,直接调用push_front即可 */
            push_front(value);
            return begin_;
        } else {
            /* 其余位置调用 insert_aux 函数完成 */
            return insert_aux(position, value);
        }
    }

    /*
     * 在指定位置前插入数据,移动语义版本
     */
    template<typename T>
    typename deque<T>::iterator deque<T>::insert(iterator position, value_type &&value) {
        if (position.cur == end_.cur) {
            /* 如果插入位置在尾部,直接调用emplace_back即可 */
            emplace_back(tinySTL::move(value));
            iterator temp = end_;
            --temp;
            return temp;
        } else if (position.cur == begin_.cur) {
            /* 如果插入位置在头部,直接调用emplace_front即可 */
            emplace_front(tinySTL::move(value));
            return begin_;
        } else {
            /* 其余位置调用 insert_aux 函数完成 */
            return insert_aux(position, tinySTL::move(value));
        }
    }

    /*
     * 在指定位置前插入n个相同数据
     */
    template<typename T>
    void deque<T>::insert(iterator position, size_type n, const value_type &value) {
        if (position.cur == end_.cur) {
            /* 在当前buffer的最后一个位置上,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做 */
            require_capacity(n, false);
            iterator new_end = end_ + n;
            tinySTL::uninitialized_fill_n(end_, n, value);
            end_ = new_end;
        } else if (position.cur == begin_.cur) {
            /* 在当前buffer的第一个位置上，,调用require_capacity函数，空间不够会申请新空间，空间足够则是什么都不做*/
            require_capacity(n, true);
            iterator new_begin = begin_ - n;
            tinySTL::uninitialized_fill_n(new_begin, n, value);
            begin_ = new_begin;
        } else {
            fill_insert(position, n, value);
        }
    }

    /*
     * 删除指定位置的元素
     */
    template<typename T>
    typename deque<T>::iterator deque<T>::erase(iterator position) {
        iterator next = position;
        ++next;
        const size_type elems_before = position - begin_;
        if (elems_before < (size() / 2)) {
            /* 前半部分元素较少，移动前半部分 */
            tinySTL::copy_backward(begin_, position, next);
            /* 弹出头部元素 */
            pop_front();
        } else {
            /* 后半部分元素较少，移动后半部分 */
            tinySTL::copy(next, end_, position);
            pop_back();
        }
        return begin_ + elems_before;
    }

    /*
     * 删除指定区间的元素
     */
    template<typename T>
    typename deque<T>::iterator deque<T>::erase(iterator first, iterator last) {
        if (first == begin_ && last == end_) {
            /* 此分支表示全部删除，调用clear，会留下第一个缓冲区 */
            clear();
            return end_;
        } else {
            const size_type len = last - first;
            const size_type elems_before = first - begin_;
            if (elems_before < ((size() - len) / 2)) {
                /* 移动前面剩余的元素 */
                tinySTL::copy_backward(begin_, first, last);
                iterator new_begin = begin_ + len;
                /* 销毁失效元素 */
                for (map_pointer cur = begin_.node + 1; cur < new_begin.node; ++cur) {
                    data_allocator::destroy(*cur, *cur + buffer_size);
                }
                if (begin_.node != new_begin.node) {
                    tinySTL::destroy(begin_.cur, begin_.last);
                    tinySTL::destroy(new_begin.first, new_begin.cur);
                } else {
                    tinySTL::destroy(begin_.cur, new_begin.cur);
                }
                begin_ = new_begin;
            } else {
                /* 移动后面剩余的元素 */
                tinySTL::copy(last, end_, first);
                iterator new_end = end_ - len;
                /* 销毁失效元素 */
                for (map_pointer cur = new_end.node + 1; cur < end_.node; ++cur) {
                    data_allocator::destroy(*cur, *cur + buffer_size);
                }
                if (new_end.node != end_.node) {
                    tinySTL::destroy(new_end.cur, new_end.last);
                    tinySTL::destroy(end_.first, end_.cur);
                } else {
                    tinySTL::destroy(new_end.cur, end_.cur);
                }
                end_ = new_end;
            }
            /* 释放多余的buffer空间 */
            shrink_to_fit();
            return begin_ + elems_before;
        }
    }

    /*
     * 销毁deque中的元素
     * clear会保留头部的buffer区域
     */
    template<typename T>
    void deque<T>::clear() {
        /* clear会保留头部的buffer区域 */
        for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur) {
            data_allocator::destroy(*cur, *cur + buffer_size);
        }
        if (begin_.node != end_.node) {
            /* 有两个以上的buffer,销毁头尾buffer中的数据 */
            tinySTL::destroy(begin_.cur, begin_.last);
            tinySTL::destroy(end_.first, end_.cur);
        } else {
            /* 销毁对应区域的数据 */
            tinySTL::destroy(begin_.cur, end_.cur);
        }
        /* 减小容器容量，释放空间 */
        end_ = begin_;
        /* 至少会留下头部缓冲区 */
        shrink_to_fit();
    }

    /*
     * 交换两个deque
     */
    template<typename T>
    void deque<T>::swap(deque &rhs) noexcept {
        if (this != &rhs) {
            tinySTL::swap(begin_, rhs.begin_);
            tinySTL::swap(end_, rhs.end_);
            tinySTL::swap(map_, rhs.map_);
            tinySTL::swap(map_size_, rhs.map_size_);
        }
    }


    // ==========================================================================================

    /*
     * 类私有的辅助函数
     */

    /*
     * 创建或销毁 buffer 与 map 相关函数
     */

    /*
     * 创建deque中的map结构
     */
    template<typename T>
    typename deque<T>::map_pointer deque<T>::create_map(size_type size) {
        map_pointer mp = nullptr;
        mp = map_allocator::allocate(size);
        for (size_type i = 0; i < size; ++i) {
            /* 所有节点初始化为空指针 */
            *(mp + i) = nullptr;
        }
        return mp;
    }

    /*
     * 创建n_start与n_finish指定范围内的buffer区域
     */
    template<typename T>
    void deque<T>::create_buffer(map_pointer n_start, map_pointer n_finish) {
        map_pointer cur = nullptr;
        try {
            for (cur = n_start; cur <= n_finish; ++cur) {
                /* 申请缓存空间，并把地址记录在map的对应位置上 */
                *cur = data_allocator::allocate(buffer_size);
            }
        }
        catch (...) {
            while (cur != n_start) {
                --cur;
                data_allocator::deallocate(*cur, buffer_size);
                *cur = nullptr;
            }
            throw;
        }
    }

    /*
     * 销毁n_start与n_finish指定范围内的buffer区域
     */
    template<typename T>
    void deque<T>::destroy_buffer(map_pointer n_start, map_pointer n_finish) {
        for (map_pointer cur = n_start; cur <= n_finish; ++cur) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    /*
     * 初始化deque相关函数
     */

    /*
     * 初始化deque中的map，n_elem 是元素个数
     */
    template<typename T>
    void deque<T>::map_init(size_type n_elem) {
        /* 计算需要分配的缓冲区个数，向上取整 */
        const size_type n_node = n_elem / buffer_size + 1;
        /* map的大小取计算出的刚好放下的节点数n_node与预定义DEQUE_MAP_INIT_SIZE之间的大的值 */
        map_size_ = tinySTL::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), n_node + 2);
        try {
            map_ = create_map(map_size_);
        }
        catch (...) {
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        /* 让 n_start 和 n_finish 都指向 map_ 最中央的区域，方便向头尾扩充 */
        map_pointer n_start = map_ + (map_size_ - n_node) / 2;
        map_pointer n_finish = n_start + n_node - 1;
        try {
            /* 创建这一段map对应的buffer空间 */
            create_buffer(n_start, n_finish);
        }
        catch (...) {
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }
        begin_.set_node(n_start);
        end_.set_node(n_finish);
        /* 从第一个的连续buffer区域的头部开始 */
        begin_.cur = begin_.first;
        /* 计算尾部的位置，将其赋值给end_.cur */
        end_.cur = end_.first + (n_elem % buffer_size);
    }

    /*
     * 给deque中初始化为n个value值
     */
    template<typename T>
    void deque<T>::fill_init(size_type n, const value_type &value) {
        map_init(n);
        if (n != 0) {
            /* 从初始化空间的第一个连续buffer开始初始化值，直到最后一个连续buffer前的一个buffer */
            for (typename iterator::map_pointer cur = begin_.node; cur < end_.node; ++cur) {
                tinySTL::uninitialized_fill(*cur, *cur + buffer_size, value);
            }
            /* 给最后一个连续buffer空间中初始化剩余个数的值 */
            tinySTL::uninitialized_fill(end_.first, end_.cur, value);
        }
    }

    /*
     * 接收一对迭代器，使用迭代器间的数据初始化deque
     * input_iterator_tag 版本
     * 类成员模板
     */
    template<typename T>
    template<typename InputIter>
    void deque<T>::copy_init(InputIter first, InputIter last, tinySTL::input_iterator_tag) {
        /* 根据数据大小建立map */
        const size_type n = tinySTL::distance(first, last);
        map_init(n);
        /* 这里需要调用shrink_to_fit函数,释放掉其余的buffer空间,因为map_init会申请对应数量的buffer,同时将begin_与end_指针设置到对应位置
         * 而之后的emplace_back函数会申请新的buffer且不会释放原来的buffer,所以需要先将begin_=end_,再shrink_to_fit */
        begin_ = end_;
        shrink_to_fit();
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    /*
     * 接收一对迭代器，使用迭代器间的数据初始化deque
     * forward_iterator_tag 版本
     * 类成员模板
     */
    template<typename T>
    template<typename ForwardIter>
    void deque<T>::copy_init(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag) {
        /* 根据数据大小建立map */
        const size_type n = tinySTL::distance(first, last);
        map_init(n);
        for (typename iterator::map_pointer cur = begin_.node; cur < end_.node; ++cur) {
            ForwardIter next = first;
            tinySTL::advance(next, buffer_size);
            /* 一次复制buffer_size大小的数据到buffer中 */
            tinySTL::uninitialized_copy(first, next, *cur);
            first = next;
        }
        /* 复制剩余部分 */
        tinySTL::uninitialized_copy(first, last, end_.first);
    }

    /*
     * 对deque赋值n个value
     */
    template<typename T>
    void deque<T>::fill_assign(size_type n, const value_type &value) {
        if (n > size()) {
            /* 已有部分直接复制,剩余部分插入 */
            tinySTL::fill(begin(), end(), value);
            size_type len = n - size();
            insert(end(), len, value);
        } else {
            /* 首先删除多余部分,剩余部分复制value过去 */
            erase(begin() + n, end());
            tinySTL::fill(begin(), end(), value);
        }
    }

    /*
     * 对deque赋值迭代器之间的数据
     * 类成员模板
     * input_iterator_tag版本
     */
    template<typename T>
    template<typename InputIter>
    void deque<T>::copy_assign(InputIter first, InputIter last, tinySTL::input_iterator_tag) {
        iterator first1 = begin();
        iterator last1 = end();
        /* 长度相同部分复制 */
        for (; first1 != last && first1 != last1; ++first1, ++first) {
            *first1 = *first;
        }
        if (first1 != last1) {
            /* 说明还有多余部分,多余部分删除 */
            erase(first1, last1);
        } else {
            /* 说明原有空间不足,使用insert_dispatch函数插入 */
            insert_dispatch(end_, first, last, tinySTL::input_iterator_tag{});
        }
    }

    /*
     * 对deque赋值迭代器之间的数据
     * 类成员模板
     * input_iterator_tag版本
     */
    template<typename T>
    template<typename ForwardIter>
    void deque<T>::copy_assign(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag) {
        const size_type len1 = size();
        const size_type len2 = tinySTL::distance(first, last);
        if (len1 < len2) {
            ForwardIter next = first;
            tinySTL::advance(next, len1);
            tinySTL::copy(first, next, begin_);
            /* 剩余部分调用insert_dispatch函数插入 */
            insert_dispatch(end_, next, last, tinySTL::forward_iterator_tag{});
        } else {
            erase(tinySTL::copy(first, last, begin_), end_);
        }
    }

    /*
     * 在指定位置前构建插入一个元素
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T>
    template<typename...Args>
    typename deque<T>::iterator deque<T>::insert_aux(iterator position, Args &&...args) {
        /* 计算插入点前方还有多少元素 */
        const size_type elems_before = position - begin_;
        if (elems_before < (size() / 2)) {
            /* 前方元素较少，移动前方的元素 */
            emplace_front(front());
            iterator front1 = begin_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            position = begin_ + elems_before;
            iterator pos = position;
            ++pos;
            tinySTL::copy(front2, pos, front1);
        } else {
            /* 后方元素较少，移动后方的元素 */
            emplace_back(back());
            iterator back1 = end_;
            --back1;
            iterator back2 = back1;
            --back2;
            position = begin_ + elems_before;
            tinySTL::copy_backward(position, back2, back1);
        }
        /* 空位以腾出，将元素插入指定位置 */
        *position = value_type(tinySTL::forward<Args>(args)...);
        return position;
    }

    /*
     * 在指定位置前插入n个value
     */
    template<typename T>
    void deque<T>::fill_insert(iterator position, size_type n, const value_type &value) {
        const size_type elems_before = position - begin_;
        const size_type len = size();
        value_type value_copy = value;
        if (elems_before < (len / 2)) {
            /* 前方数据较少,移动前方的数据,在前方申请n个数据空间 */
            require_capacity(n, true);
            /* 保存副本,原来的迭代器可能会失效 */
            iterator old_begin = begin_;
            iterator new_begin = begin_ - n;
            /* 此句可以删掉 */
            position = begin_ + elems_before;
            try {
                if (elems_before >= n) {
                    /* 插入点之前的数据数量比插入数据多进入此分支 */
                    iterator begin_n = begin_ + n;
                    tinySTL::uninitialized_copy(begin_, begin_n, new_begin);
                    begin_ = new_begin;
                    tinySTL::copy(begin_n, position, old_begin);
                    tinySTL::fill(position - n, position, value_copy);
                } else {
                    /* 插入点之前的数据数量比插入数据少进入此分支 */
                    tinySTL::uninitialized_fill(tinySTL::uninitialized_copy(begin_, position, new_begin),
                                                begin_, value_copy);
                    begin_ = new_begin;
                    tinySTL::fill(old_begin, position, value_copy);
                }
            }
            catch (...) {
                if (new_begin.node != begin_.node) {
                    destroy_buffer(new_begin.node, begin_.node - 1);
                }
                throw;
            }
        } else {
            /* 后方数据较少,移动后方的数据,在后方申请n个数据空间 */
            require_capacity(n, false);
            /* 保存副本,原来的迭代器可能会失效 */
            iterator old_end = end_;
            iterator new_end = end_ + n;
            const size_type elems_after = len - elems_before;
            position = end_ - elems_after;
            try {
                if (elems_after > n) {
                    /* 插入点之后的数据数量比插入数据多进入此分支 */
                    iterator end_n = end_ - n;
                    tinySTL::uninitialized_copy(end_n, end_, end_);
                    end_ = new_end;
                    tinySTL::copy_backward(position, end_n, old_end);
                    tinySTL::fill(position, position + n, value_copy);
                } else {
                    /* 插入点之后的数据数量比插入数据少进入此分支 */
                    tinySTL::uninitialized_fill(end_, position + n, value_copy);
                    tinySTL::uninitialized_copy(position, end_, position + n);
                    end_ = new_end;
                    tinySTL::fill(position, old_end, value_copy);
                }
            }
            catch (...) {
                if (new_end.node != end_.node) {
                    destroy_buffer(end_.node + 1, new_end.node);
                }
                throw;
            }
        }
    }

    /*
     * 在指定位置前插入迭代器间的数据
     * 类成员模板
     */
    template<typename T>
    template<typename ForwardIter>
    void deque<T>::copy_insert(iterator position, ForwardIter first, ForwardIter last, size_type n) {
        const size_type elems_before = position - begin_;
        size_type len = size();
        if (elems_before < (len / 2)) {
            /* 前方数据较少,移动前方的数据,在前方申请n个数据空间 */
            require_capacity(n, true);
            /* 保存副本,原来的迭代器可能会失效 */
            iterator old_begin = begin_;
            iterator new_begin = begin_ - n;
            position = begin_ + elems_before;
            try {
                if (elems_before >= n) {
                    /* 插入点之前的数据数量比插入数据多进入此分支 */
                    iterator begin_n = begin_ + n;
                    tinySTL::uninitialized_copy(begin_, begin_n, new_begin);
                    begin_ = new_begin;
                    tinySTL::copy(begin_n, position, old_begin);
                    tinySTL::copy(first, last, position - n);
                } else {
                    /* 插入点之前的数据数量比插入数据少进入此分支 */
                    ForwardIter mid = first;
                    tinySTL::advance(mid, n - elems_before);
                    tinySTL::uninitialized_copy(first, mid,
                                                tinySTL::uninitialized_copy(begin_, position, new_begin));
                    begin_ = new_begin;
                    tinySTL::copy(mid, last, old_begin);
                }
            }
            catch (...) {
                if (new_begin.node != begin_.node) {
                    destroy_buffer(new_begin.node, begin_.node - 1);
                }
            }
        } else {
            /* 后方数据较少,移动后方的数据,在后方申请n个数据空间 */
            require_capacity(n, false);
            /* 保存副本,原来的迭代器可能会失效 */
            iterator old_end = end_;
            iterator new_end = end_ + n;
            const size_type elems_after = len - elems_before;
            position = end_ - elems_after;
            try {
                if (elems_after > n) {
                    /* 插入点之后的数据数量比插入数据多进入此分支 */
                    iterator end_n = end_ - n;
                    tinySTL::uninitialized_copy(end_n, end_, end_);
                    end_ = new_end;
                    tinySTL::copy_backward(position, end_n, old_end);
                    tinySTL::copy(first, last, position);
                } else {
                    /* 插入点之后的数据数量比插入数据少进入此分支 */
                    ForwardIter mid = first;
                    tinySTL::advance(mid, elems_after);
                    tinySTL::uninitialized_copy(position, end_,
                                                tinySTL::uninitialized_copy(mid, last, end_));
                    end_ = new_end;
                    tinySTL::copy(first, mid, position);
                }
            }
            catch (...) {
                if (new_end.node != end_.node) {
                    destroy_buffer(end_.node + 1, new_end.node);
                }
                throw;
            }
        }
    }

    /*
     * 将迭代器间的数据插入到指定位置前
     * input_iterator_tag版本
     * 类成员函数模板
     */
    template<typename T>
    template<typename InputIter>
    void deque<T>::insert_dispatch(iterator position, InputIter first, InputIter last, tinySTL::input_iterator_tag) {
        if (last <= first) {
            return;
        }
        const size_type n = tinySTL::distance(first, last);
        const size_type elems_before = position - begin_;
        if (elems_before < (size() / 2)) {
            /* 在前方申请n个空间 */
            require_capacity(n, true);
        } else {
            /* 在后方申请n个空间 */
            require_capacity(n, false);
        }
        position = begin_ + elems_before;
        iterator cur = --last;
        for (size_type i = 0; i < n; ++i, --cur) {
            /* 这样很慢,每次都会批量移动前面的数据 */
            insert(position, *cur);
        }
    }

    /*
     * 将迭代器间的数据插入到指定位置前
     * forward_iterator_tag版本
     * 类成员函数模板
     */
    template<typename T>
    template<typename ForwardIter>
    void
    deque<T>::insert_dispatch(iterator position, ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag) {
        if (last <= first) {
            return;
        }
        const size_type n = tinySTL::distance(first, last);
        if (position.cur == end_.cur) {
            /* 在尾巴部位置前插入,在后方申请n个空间 */
            require_capacity(n, false);
            iterator new_end = end_ + n;
            try {
                tinySTL::uninitialized_copy(first, last, end_);
                end_ = new_end;
            }
            catch (...) {
                if (new_end.node != end_.node) {
                    destroy_buffer(end_.node + 1, new_end.node);
                }
                throw;
            }
        } else if (position.cur == begin_.cur) {
            /* 在头部位置前插入,在前方申请n个空间 */
            require_capacity(n, true);
            iterator new_begin = begin_ - n;
            try {
                tinySTL::uninitialized_copy(first, last, new_begin);
                begin_ = new_begin;
            }
            catch (...) {
                if (new_begin.node != begin_.node) {
                    destroy_buffer(new_begin.node, begin_.node - 1);
                }
                throw;
            }
        } else {
            copy_insert(position, first, last, n);
        }
    }

    /*
     * 在deque头部或尾部分配n个空间
     * 第一个参数表示需要n个容量，第二个参数表示头还是尾
     * 如果已经满足要求则不做任何事
     */
    template<typename T>
    void deque<T>::require_capacity(size_type n, bool front) {
        if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n)) {
            /* 若是队头，且当前buffer中的前方剩余空间不足n个，进入此分支 */
            /* 计算需要多少个buffer空间 */
            const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
            /* 如果当前剩余的buffer空间数量不足，调用reallocate_map_at_front函数申请空间 */
            if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
                reallocate_map_at_front(need_buffer);
                return;
            }
            /* 创建指定个数的buffer区域 */
            create_buffer(begin_.node - need_buffer, begin_.node - 1);
        } else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {
            /* 若是队尾，且当前buffer中的后方剩余空间不足n个，进入此分支 */
            /* 计算需要多少个buffer空间 */
            const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
            /* 如果当前剩余的buffer空间数量不足，调用reallocate_map_at_back函数申请空间 */
            if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
                reallocate_map_at_back(need_buffer);
                return;
            }
            /* 创建指定个数的buffer区域 */
            create_buffer(end_.node + 1, end_.node + need_buffer);
        }
    }

    /*
     * 重新分配空间，因为头部空间不足
     * need表示需要多少头部空间(以buffer为单位)
     */
    template<typename T>
    void deque<T>::reallocate_map_at_front(size_type need_buffer) {
        /* 取增长两倍 与 增量后的较大值作为新的map大小 */
        const size_type new_map_size = tinySTL::max(map_size_ << 1,
                                                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer_count = end_.node - begin_.node + 1;
        const size_type new_buffer_count = old_buffer_count + need_buffer;

        /* 令新的 map 中的指针指向原来的 buffer，并开辟根据need_buffer大小开辟新的 buffer */
        /* 让存在buffer的map元素处于map的中间部分,begin->mid存放的是新的buffer地址，mid->end是原有的buffer地址 */
        map_pointer begin = new_map + ((new_map_size - new_buffer_count) / 2);
        map_pointer mid = begin + need_buffer;
        map_pointer end = mid + old_buffer_count;
        /* 创建新的buffer链接到begin->mid-1上 */
        create_buffer(begin, mid - 1);
        /* 将原有的buffer地址复制到新的map上 */
        for (map_pointer begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2) {
            *begin1 = *begin2;
        }

        /* 更新数据 */
        map_allocator::deallocate(map_, map_size_);
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
        end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
    }

    /*
     * 重新分配空间，因为尾部空间不足
     * need表示需要多少尾部空间(以buffer为单位)
     */
    template<typename T>
    void deque<T>::reallocate_map_at_back(size_type need_buffer) {
        /* 取增长两倍 与 增量后的较大值作为新的map大小 */
        const size_type new_map_size = tinySTL::max(map_size_ << 1,
                                                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer_count = end_.node - begin_.node + 1;
        const size_type new_buffer_count = old_buffer_count + need_buffer;

        /* 令新的 map 中的指针指向原来的 buffer，并开辟根据need_buffer大小开辟新的 buffer */
        /* 让存在buffer的map元素处于map的中间部分,mid->end存放的是新的buffer地址，begin->mid是原有的buffer地址 */
        map_pointer begin = new_map + ((new_map_size - new_buffer_count) / 2);
        map_pointer mid = begin + old_buffer_count;
        map_pointer end = mid + need_buffer;
        /* 创建新的buffer链接到mid->end-1上 */
        create_buffer(mid, end - 1);
        /* 将原有的buffer地址复制到新的map上 */
        for (map_pointer begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2) {
            *begin1 = *begin2;
        }

        /* 更新数据 */
        map_allocator::deallocate(map_, map_size_);
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
        end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    }

    // =======================================================================================================

    /*
     * 全局区域重载比较操作符
     */

    /*
     * 重载相等于运算符
     */
    template<typename T>
    bool operator==(const deque<T> &lhs, const deque<T> &rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    /*
     * 重载不相等于运算符
     */
    template<typename T>
    bool operator!=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于运算符
     */
    template<typename T>
    bool operator<(const deque<T> &lhs, const deque<T> &rhs) {
        return tinySTL::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    /*
     * 重载大于运算符
     */
    template<typename T>
    bool operator>(const deque<T> &lhs, const deque<T> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于运算符
     */
    template<typename T>
    bool operator<=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于运算符
     */
    template<typename T>
    bool operator>=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载 tinySTL的swap
     */
    template<typename T>
    void swap(deque<T> &lhs, deque<T> &rhs) {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_DEQUE_H
