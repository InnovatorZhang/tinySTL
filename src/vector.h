//
// Created by cqupt1811 on 2022/5/8.
//

/*
 * 这个头文件包含一个模板类 vector
 *
 * 异常保证：
 * tinySTL::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
 *      * emplace
 *      * emplace_back
 *      * push_back
 * 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
 *      * reserve
 *      * resize
 *      * insert
 */

#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include <initializer_list>  /* std::initializer_list<T> 类型对象是一个访问 const T 类型对象数组的轻量代理对象 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "memory.h"  /* 这个头文件负责更高级的动态内存管理,包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */
#include "exceptdef.h"  /* 此文件中定义了异常相关的宏 */
#include "algo.h"  /* 这个头文件包含了 tinySTL 的一系列算法 */

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

    // ==============================================================================================
    template<typename T>
    class vector {
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in tinySTL");
    public:
        /*
         * vector类中的类型别名
         */

        /*
         * 定义vector所用空间配置器的别名
         */
        typedef tinySTL::allocator<T> allocator_type;
        typedef tinySTL::allocator<T> data_allocator;

        /*
         * 定义类型别名，包装allocator中的类型
         */
        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        /*
         * 定义迭代器类型
         * 直接使用指针当作迭代器
         */
        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef typename tinySTL::reverse_iterator<iterator> reverse_iterator;
        typedef typename tinySTL::reverse_iterator<const_iterator> const_reverse_iterator;

        /*
         * 返回空间配置器信息
         */
        allocator_type get_allocator() {
            return data_allocator();
        }

    private:
        /*
         * 三个迭代器描述当前vector的数据空间
         * 当前存储数据头部，当前存储数据尾部，数据的最大空间尾部
         */
        iterator begin_;
        iterator end_;
        iterator cap_;

    public:
        /*
         * 默认构造函数
         */
        vector() noexcept {
            try_init();
        }

        /*
         * 显式构造函数 接受一个空间大小参数
         */
        explicit vector(size_type n) {
            fill_init(n, value_type());
        }

        /*
         * 构造函数 接受一个空间大小参数以及默认值
         */
        vector(size_type n, const value_type &value) {
            fill_init(n, value);
        }

        /*
         * 类成员模板 接受两个迭代器，使用迭代器间的数据初始化vector
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last) {
            TINYSTL_DEBUG(!(last < first));
            range_init(first, last);
        }

        /*
         * 拷贝构造函数
         */
        vector(const vector &rhs) {
            range_init(rhs.begin_, rhs.end_);
        }

        /*
         * 移动构造函数
         */
        vector(vector &&rhs) noexcept: begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
            /* 将被移动的对象中的变量置空，因为空间被当前对象接管了 */
            rhs.begin_ = nullptr;
            rhs.end_ = nullptr;
            rhs.cap_ = nullptr;
        }

        /*
         * 构造函数 使用初始化列表，即支持以{}的方式构建对象
         */
        vector(std::initializer_list<value_type> i_list) {
            range_init(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝赋值函数
         */
        vector &operator=(const vector &rhs);

        /*
         * 移动赋值函数
         */
        vector &operator=(vector &&rhs) noexcept;

        /*
         * 拷贝赋值函数 接受一个初始化列表
         */
        vector &operator=(std::initializer_list<value_type> i_list) {
            /* 这里用到了swap交换的技巧，临时对象析构自动释放空间 */
            vector temp(i_list.begin(), i_list.end());
            swap(temp);
            return *this;
        }

        /*
         * 析构函数 析构函数中释放资源
         */
        ~vector() {
            destroy_and_recover(begin_, end_, cap_ - begin_);
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
        }

    public:

        /*
         * 迭代器相关操作
         */

        /*
         * 返回头部迭代器
         */
        iterator begin() noexcept {
            return begin_;
        }

        /*
         * 返回头部迭代器 常量版
         */
        const_iterator begin() const noexcept {
            return begin_;
        }

        /*
         * 返回尾部迭代器
         */
        iterator end() noexcept {
            return end_;
        }

        /*
         * 返回尾部迭代器 常量版
         */
        const_iterator end() const noexcept {
            return end_;
        }

        /*
         * 返回反向迭代器
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 返回反向迭代器 常量版
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 返回反向迭代器
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 返回反向迭代器 常量版
         */
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        /*
         * 返回常量的头部迭代器
         */
        const_iterator cbegin() const noexcept {
            return begin();
        }

        /*
         * 返回常量的尾部迭代器
         */
        const_iterator cend() const noexcept {
            return end();
        }

        /*
         * 返回常量的头部反向迭代器
         */
        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        /*
         * 返回常量的尾部反向迭代器
         */
        const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /*
         * vector空间大小相关操作
         */

        /*
         * 返回vector是否为空
         */
        bool empty() const noexcept {
            return begin_ == end_;
        }

        /*
         * 返回vector的当前使用大小
         */
        size_type size() const noexcept {
            return static_cast<size_type>(end_ - begin_);
        }

        /*
         * 返回vector的最大可申请容量
         */
        size_type max_size() const noexcept {
            /* 最大容量等于size_type个字节 */
            return static_cast<size_type>(-1) / sizeof(T);
        }

        /*
         * 返回vector的容量
         */
        size_type capacity() const noexcept {
            return static_cast<size_type>(cap_ - begin_);
        }

        /*
         * 预留空间大小，当原容量小于要求大小时，才会重新分配
         */
        void reserve(size_type n);

        /*
         * 将空间压缩到刚好满足当前需求
         */
        void shrink_to_fit();

        /*
         * 访问元素相关操作
         */

        /*
         * 重载 [] 运算符
         */
        reference operator[](size_type n) {
            /* 必须保证下标小于当前的size */
            TINYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }

        /*
         * 重载 [] 运算符 常量版
         */
        const_reference operator[](size_type n) const {
            /* 必须保证下标小于当前的size */
            TINYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }

        /*
         * 获取第n个元素的引用
         */
        reference at(size_type n) {
            /* 必须保证下标小于当前的size */
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            /* 调用重载的运算符完成即可 */
            return (*this)[n];
        }

        /*
         * 获取第n个元素的引用 常量版
         */
        const_reference at(size_type n) const {
            /* 必须保证下标小于当前的size */
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            /* 调用重载的运算符完成即可 */
            return (*this)[n];
        }

        /*
         * 获取头部元素的引用
         */
        reference front() {
            TINYSTL_DEBUG(!empty());
            return *begin_;
        }

        /*
         * 获取头部元素的引用 常量版
         */
        const_reference front() const {
            TINYSTL_DEBUG(!empty());
            return *begin_;
        }

        /*
         * 获取尾部元素的引用
         */
        reference back() {
            TINYSTL_DEBUG(!empty());
            /* 因为 end_ 指向的是数据后一个位置，所以需要将end_减一 */
            return *(end_ - 1);
        }

        /*
         * 获取尾部元素的引用 常量版
         */
        const_reference back() const {
            TINYSTL_DEBUG(!empty());
            /* 因为 end_ 指向的是数据后一个位置，所以需要将end_减一 */
            return *(end_ - 1);
        }

        /*
         * 返回第一个元素的位置，以指针的形式
         */
        pointer data() noexcept {
            return begin_;
        }

        /*
         * 返回第一个元素的位置，以指针的形式 常量版
         */
        const_pointer data() const noexcept {
            return begin_;
        }

        /*
         * 修改容器的相关操作,赋值操作
         */

        /*
         * 在容器中从头填充n个value对象
         */
        void assign(size_type n, const value_type &value) {
            fill_assign(n, value);
        }

        /*
         * 类成员模板
         * 将迭代器间的对象复制到vector中
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last) {
            TINYSTL_DEBUG(!(last < first));
            copy_assign(first, last, tinySTL::iterator_category(first));
        }

        /*
         * 将初始化列表中的对象复制到vector中
         */
        void assign(std::initializer_list<value_type> i_list) {
            copy_assign(i_list.begin(), i_list.end(), tinySTL::forward_iterator_tag{});
        }

        /*
         * 在指定位置构造对象
         */

        /*
         * 类成员模板
         * 可变参数模板
         * 在指定位置构造对象
         */
        template<typename... Args>
        iterator emplace(const_iterator pos, Args &&...args);

        /*
         * 类成员模板
         * 可变参数模板
         * 在尾部构造对象
         */
        template<typename... Args>
        void emplace_back(Args &&...args);

        /*
         * 向vector尾部插入或弹出元素
         */

        /*
         * 向vector尾部插入元素
         */
        void push_back(const value_type &value);

        /*
         * 向vector尾部插入元素
         */
        void push_back(value_type &&value) {
            emplace_back(tinySTL::move(value));
        }

        /*
         * 从vector尾部弹出元素
         */
        void pop_back();

        /*
         * 插入元素操作
         */

        /*
         * 在指定位置插入一个元素
         */
        iterator insert(const_iterator pos, const value_type &value);

        /*
         * 在指定位置插入一个元素
         * 移动语义版
         */
        iterator insert(const_iterator pos, value_type &&value) {
            return emplace(pos, tinySTL::move(value));
        }

        /*
         * 在指定位置插入n个元素
         */
        iterator insert(const_iterator pos, size_type n, const value_type &value) {
            TINYSTL_DEBUG(pos >= begin() && pos <= end());
            return fill_insert(const_cast<iterator>(pos), n, value);
        }

        /*
         * 在指定位置插入迭代器间的数据
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type= 0>
        void insert(const_iterator pos, Iter first, Iter last) {
            TINYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
            copy_insert(const_cast<iterator>(pos), first, last);
        }

        /*
         * 修改容器的相关操作,删除元素操作
         */

        /*
         * 删除指定位置的元素
         */
        iterator erase(const_iterator pos);

        /*
         * 删除指定区间的元素
         */
        iterator erase(const_iterator first, const_iterator last);

        /*
         * 清除所有元素
         */
        void clear() {
            erase(begin(), end());
        }

        /*
         * 改变vector的大小以及逆序
         */

        /*
         * 改变vector的大小至new_size
         */
        void resize(size_type new_size) {
            return resize(new_size, value_type());
        }

        /*
         * 改变vector的大小至new_size
         */
        void resize(size_type new_size, const value_type &value);

        /*
         * 将元素逆序
         */
        void reverse() {
            tinySTL::reverse(begin(), end());
        }

        /*
         * vector 类的swap操作，与另一个vector交换空间
         */
        void swap(vector &rhs) noexcept;

    private:
        /*
         * 使用到的一些辅助函数
         */

        /*
         * try_init 函数，若分配失败则忽略，不抛出异常
         */
        void try_init() noexcept;

        /*
         * 申请 cap 大小的空间， 预留其中的前 size 个空间
         */
        void init_space(size_type size, size_type cap);

        /*
         * 申请空间，同时初始化空间
         */
        void fill_init(size_type n, const value_type &value);

        /*
         * 使用迭代器间的数据初始化vector
         */
        template<typename Iter>
        void range_init(Iter first, Iter last);

        /*
         * 销毁所有元素，同时释放vector申请的空间
         */
        void destroy_and_recover(iterator first, iterator last, size_type n);

        /*
         * 添加add_size大小的空间，计算vector新的容量
         */
        size_type get_new_cap(size_type add_size);

        /*
         * 在容器中从头填充n个value对象
         */
        void fill_assign(size_type n, const value_type &value);

        /*
         * 将迭代器间的对象复制到vector中
         * input_iterator_tag 版本
         */
        template<typename IIter>
        void copy_assign(IIter first, IIter last, tinySTL::input_iterator_tag);

        /*
         * 将迭代器间的对象复制到vector中
         * forward_iterator_tag 版本
         */
        template<typename FIter>
        void copy_assign(FIter first, FIter last, tinySTL::forward_iterator_tag);

        /*
         * 重新分配空间并在 pos 处就地构造元素
         * 类成员模板
         */
        template<typename... Args>
        void reallocate_emplace(iterator pos, Args &&...args);

        /*
         * 重新分配空间并在 pos 处就地插入元素
         */
        void reallocate_insert(iterator pos, const value_type &value);

        /*
         * 在指定位置插入n个值
         */
        iterator fill_insert(iterator pos, size_type n, const value_type &value);

        /*
         * 在指定位置插入迭代器之间的值
         * 类成员模板
         */
        template<typename IIter>
        void copy_insert(iterator pos, IIter first, IIter last);

        /*
         * 放弃多余的容量
         * 插入到新的位置
         */
        void reinsert(size_type size);

    };

    // ===============================================================================================

    /*
     * vector类成员函数实现
     */

    /*
     * 拷贝赋值函数
     */
    template<typename T>
    vector<T> &vector<T>::operator=(const vector &rhs) {
        if (this != &rhs) {
            const auto len = rhs.size();
            if (len > capacity()) {
                vector temp(rhs.begin(), rhs.end());
                swap(temp);
            } else if (size() >= len) {
                auto i = tinySTL::copy(rhs.begin(), rhs.end(), begin());
                data_allocator::destroy(i, end_);
                end_ = begin_ + len;
            } else {
                tinySTL::copy(rhs.begin(), rhs.begin() + size(), begin_);
                tinySTL::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
                cap_ = end_ = begin_ + len;
            }
        }
        return *this;
    }

    /*
     * 移动赋值函数
     */
    template<typename T>
    vector<T> &vector<T>::operator=(vector<T> &&rhs) noexcept {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        cap_ = rhs.cap_;
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;

        return *this;
    }

    /*
     * 预留空间大小，当原容量小于要求大小时，才会重新分配
     */
    template<typename T>
    void vector<T>::reserve(size_type n) {
        if (capacity() < n) {
            THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size() in vector<T>::reserve(n)");
            const auto old_size = size();
            auto temp = data_allocator::allocate(n);
            tinySTL::uninitialized_move(begin_, end_, temp);
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = temp;
            end_ = temp + old_size;
            cap_ = begin_ + n;
        }
    }

    /*
     * 将空间压缩到刚好满足当前需求
     */
    template<typename T>
    void vector<T>::shrink_to_fit() {
        if (end_ < cap_) {
            reinsert(size());
        }
    }

    /*
     * 类成员模板
     * 可变参数模板
     * 在指定位置构造对象
     */
    template<typename T>
    template<typename... Args>
    typename vector<T>::iterator  // 返回值
    vector<T>::emplace(const_iterator pos, Args &&...args) {
        TINYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator cur_pos = const_cast<iterator>(pos);
        const size_type n = cur_pos - begin_;
        if (end_ != cap_ && cur_pos == end_) {
            data_allocator::construct(tinySTL::address_of(*end_), tinySTL::forward<Args>(args)...);
            ++end_;
        } else if (end_ != cap_) {
            data_allocator::construct(tinySTL::address_of(*end_), *(end_ - 1));
            tinySTL::copy_backward(cur_pos, end_ - 1, end_);
            *cur_pos = value_type(tinySTL::forward<Args>(args)...);
            ++end_;
        } else {
            reallocate_emplace(cur_pos, tinySTL::forward<Args>(args)...);
        }
        return begin() + n;
    }

    /*
     * 类成员模板
     * 可变参数模板
     * 在尾部构造对象
     */
    template<typename T>
    template<typename... Args>
    void vector<T>::emplace_back(Args &&...args) {
        if (end_ < cap_) {
            data_allocator::construct(tinySTL::address_of(*end_), tinySTL::forward<Args>(args)...);
            ++end_;
        } else {
            reallocate_emplace(end_, tinySTL::forward<Args>(args)...);
        }
    }

    /*
     * 在vector尾部插入元素
     */
    template<typename T>
    void vector<T>::push_back(const value_type &value) {
        if (end_ != cap_) {
            data_allocator::construct(tinySTL::address_of(*end_), value);
            ++end_;
        } else {
            reallocate_insert(end_, value);
        }
    }

    /*
     * 弹出vector尾部元素
     */
    template<typename T>
    void vector<T>::pop_back() {
        TINYSTL_DEBUG(!empty());
        data_allocator::destroy(tinySTL::address_of(*(end_ - 1)));
        --end_;
    }

    /*
     * 在指定位置插入元素
     */
    template<typename T>
    typename vector<T>::iterator vector<T>::insert(const_iterator pos, const value_type &value) {
        TINYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator cur_pos = const_cast<iterator>(pos);
        const size_type n = pos - begin_;
        if (end_ != cap_ && cur_pos == end_) {
            data_allocator::construct(tinySTL::address_of(*end_), value);
            ++end_;
        } else if (end_ != cap_) {
            auto new_end = end_;
            data_allocator::construct(tinySTL::address_of(*end_), *(end_ - 1));
            ++new_end;
            auto value_copy = value;  // 避免元素因以下复制操作而被改变
            tinySTL::copy_backward(cur_pos, end_ - 1, end_);
            *cur_pos = tinySTL::move(value_copy);
            end_ = new_end;
        } else {
            reallocate_insert(cur_pos, value);
        }
        return begin_ + n;
    }

    /*
     * 删除指定位置的元素
     */
    template<typename T>
    typename vector<T>::iterator  // 返回值
    vector<T>::erase(const_iterator pos) {
        /* 确保范围在begin()与end()之间 */
        TINYSTL_DEBUG(pos >= begin() && pos < end());
        iterator cur_pos = begin_ + (pos - begin_);
        tinySTL::move(cur_pos + 1, end_, cur_pos);
        data_allocator::destroy(end_ - 1);
        --end_;
        return cur_pos;
    }

    /*
     * 删除指定区间的元素
     */
    template<typename T>
    typename vector<T>::iterator  // 返回值
    vector<T>::erase(const_iterator first, const_iterator last) {
        /* 确保范围在begin()与end()之间 */
        TINYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - begin();
        iterator r = begin_ + (first - begin());
        data_allocator::destroy(tinySTL::move(r + (last - first), end_, r), end_);
        end_ = end_ - (last - first);
        return begin_ + n;
    }

    /*
     * 改变vector的大小至new_size
     */
    template<typename T>
    void vector<T>::resize(size_type new_size, const value_type &value) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), value);
        }
    }

    /*
     * vector 类的swap操作，与另一个vector交换空间
     */
    template<typename T>
    void vector<T>::swap(vector<T> &rhs) noexcept {
        /* 自赋值检测 */
        if (this != &rhs) {
            tinySTL::swap(begin_, rhs.begin_);
            tinySTL::swap(end_, rhs.end_);
            tinySTL::swap(cap_, rhs.cap_);
        }
    }

    // ========================================================================================

    /*
     * 使用到的一些辅助函数
     */

    /*
     * try_init 函数，若分配失败则忽略，不抛出异常
     */
    template<typename T>
    void vector<T>::try_init() noexcept {
        try {
            /* 第一次分配16个元素空间 */
            begin_ = data_allocator::allocate(16);
            /* 尾部等于头部 */
            end_ = begin_;
            /* 最大容量等于头部加上申请的空间大小 */
            cap_ = begin_;
        }
        catch (...) {
            /*  恢复初始状态 */
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
        }
    }

    /*
     * 申请 cap 大小的空间， 预留其中的前 size 个空间
     */
    template<typename T>
    void vector<T>::init_space(size_type size, size_type cap) {
        try {
            /* 第一次分配cap个元素空间 */
            begin_ = data_allocator::allocate(cap);
            /* 尾部等于头部+size */
            end_ = begin_ + size;
            /* 最大容量等于头部加上申请的空间大小 */
            cap_ = begin_ + cap;
        }
        catch (...) {
            /*  恢复初始状态 */
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
            /* 抛出异常 */
            throw;
        }
    }

    /*
     * 申请空间，同时初始化空间
     */
    template<typename T>
    void vector<T>::fill_init(size_type n, const value_type &value) {
        const size_type init_size = tinySTL::max(static_cast<size_type>(16), n);
        init_space(n, init_size);
        /* 初始化前n个元素 */
        tinySTL::uninitialized_fill_n(begin_, n, value);
    }

    /*
     * 使用迭代器间的数据初始化vector
     */
    template<typename T>
    template<typename Iter>
    void vector<T>::range_init(Iter first, Iter last) {
        const size_type init_size = tinySTL::max(static_cast<size_type>(last - first),
                                                 static_cast<size_type>(16));
        init_space(static_cast<size_type>(last - first), init_size);
        /* 将first->last区间中的数据赋值到vector的空间中 */
        tinySTL::uninitialized_copy(first, last, begin_);
    }

    /*
     * 销毁所有元素，同时释放vector申请的空间
     */
    template<typename T>
    void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n) {
        /* 销毁空间中存在的对象 */
        data_allocator::destroy(first, last);
        /* 释放申请的所有空间 */
        data_allocator::deallocate(first, n);
    }

    /*
     * 添加add_size大小的空间，计算vector新的容量
     */
    template<typename T>
    typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size + add_size > max_size(), "vector<T>'s size too big");
        if (old_size > max_size() - old_size / 2) {
            return old_size + add_size > max_size() - 16 ? old_size + add_size : old_size + add_size + 16;
        }
        const size_type new_size = old_size == 0
                                   ? tinySTL::max(add_size, static_cast<size_type>(16))
                                   : tinySTL::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
    }

    /*
     * 在容器中从头填充n个value对象
     */
    template<typename T>
    void vector<T>::fill_assign(size_type n, const value_type &value) {
        if (n > capacity()) {
            vector temp(n, value);
            swap(temp);
        } else if (n > size()) {
            tinySTL::fill(begin(), end(), value);
            end_ = tinySTL::uninitialized_fill_n(end_, n - size(), value);
        } else {
            erase(tinySTL::fill_n(begin_, n, value), end_);
        }
    }

    /*
     * 将迭代器间的对象复制到vector中
     * input_iterator_tag 版本
     */
    template<typename T>
    template<typename IIter>
    void vector<T>::copy_assign(IIter first, IIter last, tinySTL::input_iterator_tag) {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first, ++cur) {
            *cur = *first;
        }
        if (first == last) {
            erase(cur, end_);
        } else {
            insert(end_, first, last);
        }
    }

    /*
     * 将迭代器间的对象复制到vector中
     * forward_iterator_tag 版本
     */
    template<typename T>
    template<typename FIter>
    void vector<T>::copy_assign(FIter first, FIter last, tinySTL::forward_iterator_tag) {
        const size_type len = tinySTL::distance(first, last);
        if (len > capacity()) {
            vector temp(first, last);
            swap(temp);
        } else if (size() >= len) {
            auto new_end = tinySTL::copy(first, last, begin_);
            data_allocator::destroy(new_end, end_);
            end_ = new_end;
        } else {
            auto mid = first;
            tinySTL::advance(mid, size());
            tinySTL::copy(first, mid, begin_);
            auto new_end = tinySTL::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        }
    }

    /*
     * 重新分配空间并在 pos 处就地构造元素
     */
    template<typename T>
    template<typename ...Args>
    void vector<T>::reallocate_emplace(iterator pos, Args &&...args) {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = tinySTL::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(tinySTL::address_of(*new_end), tinySTL::forward<Args>(args)...);
            ++new_end;
            new_end = tinySTL::uninitialized_move(pos, end_, new_end);
        }
        catch (...) {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    /*
     * 重新分配空间并在 pos 处插入元素
     */
    template<typename T>
    void vector<T>::reallocate_insert(iterator pos, const value_type &value) {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        const value_type &value_copy = value;
        try {
            new_end = tinySTL::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(tinySTL::address_of(*new_end), value_copy);
            ++new_end;
            new_end = tinySTL::uninitialized_move(pos, end_, new_end);
        }
        catch (...) {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    /*
     * 在指定位置插入n个值
     */
    template<typename T>
    typename vector<T>::iterator  // 返回值
    vector<T>::fill_insert(iterator pos, size_type n, const value_type &value) {
        if (n == 0) {
            return pos;
        }
        const size_type cur_pos = pos - begin_;
        const value_type value_copy = value;  // 避免被覆盖
        if (static_cast<size_type>(cap_ - end_) >= n) {
            const size_type after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n) {
                tinySTL::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                tinySTL::move_backward(pos, old_end - n, old_end);
                tinySTL::uninitialized_fill_n(pos, n, value_copy);
            } else {
                end_ = tinySTL::uninitialized_fill_n(end_, n - after_elems, value_copy);
                end_ = tinySTL::uninitialized_move(pos, old_end, end_);
                tinySTL::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        } else {
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try {
                new_end = tinySTL::uninitialized_move(begin_, pos, new_begin);
                new_end = tinySTL::uninitialized_fill_n(new_end, n, value);
                new_end = tinySTL::uninitialized_move(pos, end_, new_end);
            }
            catch (...) {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = new_begin + new_size;
        }
        return begin_ + cur_pos;
    }

    /*
     * 在指定位置插入迭代器之间的值
     * 类成员模板
     */
    template<typename T>
    template<typename IIter>
    void vector<T>::copy_insert(iterator pos, IIter first, IIter last) {
        if (first == last) {
            return;
        }
        const auto n = tinySTL::distance(first, last);
        if ((cap_ - end_) >= n) {
            const auto after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n) {
                end_ = tinySTL::uninitialized_copy(end_ - n, end_, end_);
                tinySTL::move_backward(pos, old_end - n, old_end);
                tinySTL::uninitialized_copy(first, last, pos);
            } else {
                auto mid = first;
                tinySTL::advance(mid, after_elems);
                end_ = tinySTL::uninitialized_copy(mid, last, end_);
                end_ = tinySTL::uninitialized_move(pos, old_end, end_);
                tinySTL::uninitialized_copy(first, mid, pos);
            }
        } else {
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try {
                new_end = tinySTL::uninitialized_move(begin_, pos, new_begin);
                new_end = tinySTL::uninitialized_copy(first, last, new_end);
                new_end = tinySTL::uninitialized_move(pos, end_, new_end);
            }
            catch (...) {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
    }

    /*
     * 放弃多余的容量
     * 插入到新的位置
     */
    template<typename T>
    void vector<T>::reinsert(size_type size) {
        auto new_begin = data_allocator::allocate(size);
        try {
            tinySTL::uninitialized_move(begin_, end_, new_begin);
        }
        catch (...) {
            data_allocator::deallocate(new_begin, size);
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = begin_ + size;
        cap_ = end_;
    }

    // ============================================================================================

    /*
     * 重载全局的比较操作符
     */

    /*
     * 重载vector类的相等运算符
     */
    template<typename T>
    bool operator==(const vector<T> &lhs, const vector<T> &rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(lhs.begin(), lhs.end(), rhs.begin());
    }


    /*
     * 重载vector类的不相等运算符
     */
    template<typename T>
    bool operator!=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载vector类的小于运算符
     * 字典序比较法
     */
    template<typename T>
    bool operator<(const vector<T> &lhs, const vector<T> &rhs) {
        return tinySTL::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    /*
     * 重载vector类的大于运算符
     * 字典序比较法
     */
    template<typename T>
    bool operator>(const vector<T> &lhs, const vector<T> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载vector类的小于等于运算符
     * 字典序比较法
     */
    template<typename T>
    bool operator<=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载vector类的大于等于运算符
     * 字典序比较法
     */
    template<typename T>
    bool operator>=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载 tinySTL 的 swap
     */
    template<typename T>
    void swap(vector<T> &lhs, vector<T> &rhs) {
        lhs.swap(rhs);
    }

}  // namespace tinySTL


#endif //TINYSTL_VECTOR_H
