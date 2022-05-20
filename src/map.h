//
// Created by cqupt1811 on 2022/5/18.
//

/*
 * 这个头文件包含了两个模板类 map 和 multimap
 * map      : 映射，元素具有键值和实值，会根据键值大小自动排序，键值不允许重复
 * multimap : 映射，元素具有键值和实值，会根据键值大小自动排序，键值允许重复
 * notes:
 * 异常保证：
 * tinySTL::map<Key, T> / tinySTL::multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
 *      * emplace
 *      * emplace_hint
 *      * insert
 */

#ifndef TINYSTL_MAP_H
#define TINYSTL_MAP_H

#include "rb_tree.h"  /* 这个头文件包含一个模板类 红黑树 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 模板类 map，键值不允许重复
     * 参数 Key 代表键值类型，参数 T 代表实值类型，参数三代表键值的比较方式，缺省使用 tinySTL::less
     */
    template<typename Key, typename T, typename Compare=tinySTL::less<Key>>
    class map {
    public:
        /*
         * map 的嵌套型别定义
         */
        typedef Key key_type;
        typedef T mapped_type;
        /* map的值类型为pair其中第一个为值为const类型，作为键不能更改 */
        typedef tinySTL::pair<const key_type, mapped_type> value_type;
        typedef Compare key_compare;

        /*
         * 内部类，定义一个仿函数，用以进行元素比较
         */
        class value_compare : public binary_function<value_type, value_type, bool> {
            /*
             * 将map类添加为友元
             */
            friend class map<Key, T, Compare>;

        private:
            /*
             * 比较对象成员
             */
            Compare comp;

            /*
             * 构造函数，接受一个函数比较对象
             */
            value_compare(Compare c) : comp(c) {

            }

        public:
            /*
             * () 操作符重载
             */
            bool operator()(const value_type &lhs, const value_type &rhs) const {
                /* 比较键的大小 */
                return comp(lhs.first, rhs.first);
            }
        };

    private:
        /* 以tinySTL::rb_tree 作为底层机制 */
        typedef tinySTL::rb_tree<value_type, key_compare> base_type;
        base_type tree_;

    public:
        /*
         * 定义类型别名
         * 使用 rb_tree 的型别
         */
        typedef typename base_type::node_type node_type;
        typedef typename base_type::pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::reference reference;
        typedef typename base_type::const_reference const_reference;
        typedef typename base_type::iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;
        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::allocator_type allocator_type;

    public:
        /*
         * 由编译器生成默认构造函数
         */
        map() = default;

        /*
         * 使用迭代器间的数据初始化map
         * 类成员模板
         */
        template<typename InputIterator>
        map(InputIterator first, InputIterator last):tree_() {
            tree_.insert_unique(first, last);
        }

        /*
         * 接收初始化列表作为参数，支持{}形式初始化
         */
        map(std::initializer_list<value_type> i_list) : tree_() {
            tree_.insert_unique(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝构造函数
         */
        map(const map &rhs) : tree_(rhs.tree_) {

        }

        /*
         * 移动构造函数
         */
        map(map &&rhs) noexcept: tree_(tinySTL::move(rhs.tree_)) {

        }

        /*
         * 拷贝赋值函数
         */
        map &operator=(const map &rhs) {
            tree_ = rhs.tree_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        map &operator=(map &&rhs) {
            tree_ = tinySTL::move(rhs.tree_);
            return *this;
        }

        /*
         * 支持使用初始化列表，{}的形式赋值
         */
        map &operator=(std::initializer_list<value_type> i_list) {
            tree_.clear();
            tree_.insert_unique(i_list.begin(), i_list.end());
            return *this;
        }

    public:
        /*
         * 获取键大小比较函数对象
         */
        key_compare key_comp() const {
            return tree_.key_comp();
        }

        /*
         * 获取pair比较韩叔叔对象
         */
        value_compare value_comp() const {
            return value_compare(tree_.key_comp());
        }

        /*
         * 获取空间配置器实例
         */
        allocator_type get_allocator() const {
            return tree_.get_allocator();
        }

        /*
         * 迭代器相关函数
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return tree_.begin();
        }

        /*
         * 返回头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return tree_.begin();
        }

        /*
         * 返回为尾部迭代器
         */
        iterator end() noexcept {
            return tree_.end();
        }

        /*
         * 返回尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return tree_.end();
        }

        /*
         * 返回反向头部迭代器
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 返回反向头部迭代器 const重载
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 返回反向尾部迭代器
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 返回反向尾部迭代器 const重载
         */
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        /*
         * 获取常量头部迭代器
         */
        const_iterator cbegin() const noexcept {
            return begin();
        }

        /*
         * 获取常量尾部迭代器
         */
        const_iterator cend() const noexcept {
            return end();
        }

        /*
         * 获取常量反向头部迭代器
         */
        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        /*
         * 获取常量反向尾部迭代器
         */
        const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /*
         * 容量相关
         */

        /*
         * 查询multiset是否为空
         */
        bool empty() const noexcept {
            return tree_.empty();
        }

        /*
         * 查询multiset的大小
         */
        size_type size() const noexcept {
            return tree_.size();
        }

        /*
         * 查询multiset最大存储多少元素
         */
        size_type max_size() const noexcept {
            return tree_.max_size();
        }

        /*
         * 访问元素相关函数
         */

        /*
         * 根据键访问值,若键值不存在，at 会抛出一个异常
         */
        mapped_type &at(const key_type &key) {
            iterator it = lower_bound(key);
            /* it->first >= key */
            THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                                  "map<Key, T> no such element exists");
            return it->second;
        }

        /*
         * 根据键访问值,若键值不存在，at 会抛出一个异常  const重载
         */
        const mapped_type &at(const key_type &key) const {
            iterator it = lower_bound(key);
            /* it->first >= key */
            THROW_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
                                  "map<Key, T> no such element exists");
            return it->second;
        }

        /*
         * 操作符 [] 重载
         * 根据键访问值,若键值不存在，会自动创建对应键值对
         */
        mapped_type &operator[](const key_type &key) {
            iterator it = lower_bound(key);
            /* it->first >= key */
            if (it == end() || key_comp()(key, it->first)) {
                /* 若键值不存在，会自动创建对应键值对 */
                it = emplace_hint(it, key, T{});
            }
            return it->second;
        }

        /*
         * 操作符 [] 重载 移动语义
         * 根据键访问值,若键值不存在，会自动创建对应键值对
         */
        mapped_type &operator[](key_type &&key) {
            iterator it = lower_bound(key);
            /* it->first >= key */
            if (it == end() || key_comp()(key, it->first)) {
                /* 若键值不存在，会自动创建对应键值对 */
                it = emplace_hint(it, tinySTL::move(key), T{});
            }
            return it->second;
        }

        /*
         * 插入删除相关函数
         */

        /*
         * 在map中构建一个新元素
         * 类成员模板,可变参数模板，完美转发
         */
        template<typename ...Args>
        tinySTL::pair<iterator, bool> emplace(Args &&...args) {
            return tree_.emplace_unique(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在map中构建一个新元素 带有hint
         * 类成员模板,可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_hint(iterator hint, Args &&...args) {
            return tree_.emplace_unique_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 插入一个pair到map中
         */
        tinySTL::pair<iterator, bool> insert(const value_type &value) {
            return tree_.insert_unique(value);
        }

        /*
         * 插入一个pair到map中  移动语义
         */
        tinySTL::pair<iterator, bool> insert(value_type &&value) {
            return tree_.insert_unique(tinySTL::move(value));
        }

        /*
         * 插入一个pair到map中 带有hint
         */
        iterator insert(iterator hint, const value_type &value) {
            return tree_.insert_unique(hint, value);
        }

        /*
         * 插入一个pair到map中 带有hint 移动语义
         */
        iterator insert(iterator hint, value_type &&value) {
            return tree_.insert_unique(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器间的pair到map中
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        /*
         * 删除指定位置的元素
         */
        void erase(iterator position) {
            tree_.erase(position);
        }

        /*
         * 删除指定键的元素
         */
        size_type erase(const key_type &key) {
            return tree_.erase_unique(key);
        }

        /*
         * 删除指定区间的元素
         */
        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        /*
         * 清空map
         */
        void clear() {
            tree_.clear();
        }

        /*
         * map的相关操作
         */

        /*
         * 查找指定键的位置
         */
        iterator find(const key_type &key) {
            return tree_.find(key);
        }

        /*
         * 查找指定键的位置 const重载
         */
        const_iterator find(const key_type &key) const {
            return tree_.find(key);
        }

        /*
         * 查询是否存在key
         */
        size_type count(const key_type &key) const {
            return tree_.count_unique(key);
        }

        /*
         * 获取键值不小于 key 的第一个位置
         */
        iterator lower_bound(const key_type &key) {
            return tree_.lower_bound(key);
        }

        /*
         * 获取键值不小于 key 的第一个位置 const重载
         */
        const_iterator lower_bound(const key_type &key) const {
            return tree_.lower_bound(key);
        }

        /*
         * 键值大于 key 的第一个位置
         */
        iterator upper_bound(const key_type &key) {
            return tree_.upper_bound(key);
        }

        /*
         * 键值大于 key 的第一个位置 const重载
         */
        const_iterator upper_bound(const key_type &key) const {
            return tree_.upper_bound(key);
        }

        /*
         * 获取键为key的区间
         */
        tinySTL::pair<iterator, iterator> equal_range(const key_type &key) {
            return tree_.equal_range_unique(key);
        }

        /*
         * 获取键为key的区间 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
            return tree_.equal_range_unique(key);
        }

        /*
         * 交换map对象的内容
         */
        void swap(map &rhs) noexcept {
            tree_.swap(rhs.tree_);
        }

    public:
        /*
         * 指定友元函数，重载相等于运算符
         */
        friend bool operator==(const map &lhs, const map &rhs) {
            return lhs.tree_ == rhs.tree_;
        }

        /*
         * 指定友元函数，重载小于运算符
         */
        friend bool operator<(const map &lhs, const map &rhs) {
            return lhs.tree_ < rhs.tree_;
        }
    };

    // =============================================================================================
    /*
     * 全局区域重载操作符
     */

    /*
     * 重载相等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator==(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator!=(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator<(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator>(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator<=(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator>=(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename T, typename Compare>
    void swap(const map<Key, T, Compare> &lhs, const map<Key, T, Compare> &rhs) noexcept {
        lhs.swap(rhs);
    }

    // =============================================================================================

    /*
     * 模板类 multimap，键值允许重复
     * 参数 Key 代表键值类型，参数 T 代表实值类型，参数三代表键值的比较方式，缺省使用 tinySTL::less
     */
    template<typename Key, typename T, typename Compare=tinySTL::less<Key>>
    class multimap {
        /*
         * map 的嵌套型别定义
         */
        typedef Key key_type;
        typedef T mapped_type;
        /* map的值类型为pair其中第一个为值为const类型，作为键不能更改 */
        typedef tinySTL::pair<const key_type, mapped_type> value_type;
        typedef Compare key_compare;

        /*
         * 内部类，定义一个仿函数，用以进行元素比较
         */
        class value_compare : public binary_function<value_type, value_type, bool> {
            /*
             * 将map类添加为友元
             */
            friend class multimap<Key, T, Compare>;

        private:
            /*
             * 比较对象成员
             */
            Compare comp;

            /*
             * 构造函数，接受一个函数比较对象
             */
            value_compare(Compare c) : comp(c) {

            }

        public:
            /*
             * () 操作符重载
             */
            bool operator()(const value_type &lhs, const value_type &rhs) const {
                /* 比较键的大小 */
                return comp(lhs.first, rhs.first);
            }
        };

    private:
        /* 以tinySTL::rb_tree 作为底层机制 */
        typedef tinySTL::rb_tree<value_type, key_compare> base_type;
        base_type tree_;

    public:
        /*
         * 定义类型别名
         * 使用 rb_tree 的型别
         */
        typedef typename base_type::node_type node_type;
        typedef typename base_type::pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::reference reference;
        typedef typename base_type::const_reference const_reference;
        typedef typename base_type::iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;
        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::allocator_type allocator_type;

    public:
        /*
         * 由编译器生成默认构造函数
         */
        multimap() = default;

        /*
         * 使用迭代器间的数据初始化map
         * 类成员模板
         */
        template<typename InputIterator>
        multimap(InputIterator first, InputIterator last):tree_() {
            tree_.insert_multi(first, last);
        }

        /*
         * 接收初始化列表作为参数，支持{}形式初始化
         */
        multimap(std::initializer_list<value_type> i_list) : tree_() {
            tree_.insert_multi(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝构造函数
         */
        multimap(const multimap &rhs) : tree_(rhs.tree_) {

        }

        /*
         * 移动构造函数
         */
        multimap(multimap &&rhs) noexcept: tree_(tinySTL::move(rhs.tree_)) {

        }

        /*
         * 拷贝赋值函数
         */
        multimap &operator=(const multimap &rhs) {
            tree_ = rhs.tree_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        multimap &operator=(multimap &&rhs) {
            tree_ = tinySTL::move(rhs.tree_);
            return *this;
        }

        /*
         * 支持使用初始化列表，{}的形式赋值
         */
        multimap &operator=(std::initializer_list<value_type> i_list) {
            tree_.clear();
            tree_.insert_multi(i_list.begin(), i_list.end());
            return *this;
        }

    public:
        /*
         * 获取键大小比较函数对象
         */
        key_compare key_comp() const {
            return tree_.key_comp();
        }

        /*
         * 获取pair比较韩叔叔对象
         */
        value_compare value_comp() const {
            return value_compare(tree_.key_comp());
        }

        /*
         * 获取空间配置器实例
         */
        allocator_type get_allocator() const {
            return tree_.get_allocator();
        }

        /*
         * 迭代器相关函数
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return tree_.begin();
        }

        /*
         * 返回头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return tree_.begin();
        }

        /*
         * 返回为尾部迭代器
         */
        iterator end() noexcept {
            return tree_.end();
        }

        /*
         * 返回尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return tree_.end();
        }

        /*
         * 返回反向头部迭代器
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 返回反向头部迭代器 const重载
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 返回反向尾部迭代器
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 返回反向尾部迭代器 const重载
         */
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        /*
         * 获取常量头部迭代器
         */
        const_iterator cbegin() const noexcept {
            return begin();
        }

        /*
         * 获取常量尾部迭代器
         */
        const_iterator cend() const noexcept {
            return end();
        }

        /*
         * 获取常量反向头部迭代器
         */
        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        /*
         * 获取常量反向尾部迭代器
         */
        const_reverse_iterator crend() const noexcept {
            return rend();
        }
        /*
         * 容量相关
         */

        /*
         * 查询multiset是否为空
         */
        bool empty() const noexcept {
            return tree_.empty();
        }

        /*
         * 查询multiset的大小
         */
        size_type size() const noexcept {
            return tree_.size();
        }

        /*
         * 查询multiset最大存储多少元素
         */
        size_type max_size() const noexcept {
            return tree_.max_size();
        }

        /*
         * 插入删除相关函数
         */

        /*
         * 在multimap中构建一个新元素
         * 类成员模板,可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace(Args &&...args) {
            return tree_.emplace_multi(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在multimap中构建一个新元素 带有hint
         * 类成员模板,可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_hint(iterator hint, Args &&...args) {
            return tree_.emplace_multi_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 插入一个pair到multimap中
         */
        iterator insert(const value_type &value) {
            return tree_.insert_multi(value);
        }

        /*
         * 插入一个pair到multimap中  移动语义
         */
        iterator insert(value_type &&value) {
            return tree_.insert_multi(tinySTL::move(value));
        }

        /*
         * 插入一个pair到multimap中 带有hint
         */
        iterator insert(iterator hint, const value_type &value) {
            return tree_.insert_multi(hint, value);
        }

        /*
         * 插入一个pair到multimap中 带有hint 移动语义
         */
        iterator insert(iterator hint, value_type &&value) {
            return tree_.insert_multi(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器间的pair到multimap中
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_multi(first, last);
        }

        /*
         * 删除指定位置的元素
         */
        void erase(iterator position) {
            tree_.erase(position);
        }

        /*
         * 删除指定键的元素
         */
        size_type erase(const key_type &key) {
            return tree_.erase_multi(key);
        }

        /*
         * 删除指定区间的元素
         */
        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        /*
         * 清空multimap
         */
        void clear() {
            tree_.clear();
        }

        /*
         * map的相关操作
         */

        /*
         * 查找指定键的位置
         */
        iterator find(const key_type &key) {
            return tree_.find(key);
        }

        /*
         * 查找指定键的位置 const重载
         */
        const_iterator find(const key_type &key) const {
            return tree_.find(key);
        }

        /*
         * 查询是否存在key
         */
        size_type count(const key_type &key) const {
            return tree_.count_multi(key);
        }

        /*
         * 获取键值不小于 key 的第一个位置
         */
        iterator lower_bound(const key_type &key) {
            return tree_.lower_bound(key);
        }

        /*
         * 获取键值不小于 key 的第一个位置 const重载
         */
        const_iterator lower_bound(const key_type &key) const {
            return tree_.lower_bound(key);
        }

        /*
         * 键值大于 key 的第一个位置
         */
        iterator upper_bound(const key_type &key) {
            return tree_.upper_bound(key);
        }

        /*
         * 键值大于 key 的第一个位置 const重载
         */
        const_iterator upper_bound(const key_type &key) const {
            return tree_.upper_bound(key);
        }

        /*
         * 获取键为key的区间
         */
        tinySTL::pair<iterator, iterator> equal_range(const key_type &key) {
            return tree_.equal_range_multi(key);
        }

        /*
         * 获取键为key的区间 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
            return tree_.equal_range_multi(key);
        }

        /*
         * 交换map对象的内容
         */
        void swap(multimap &rhs) noexcept {
            tree_.swap(rhs.tree_);
        }

    public:
        /*
         * 指定友元函数，重载相等于运算符
         */
        friend bool operator==(const multimap &lhs, const multimap &rhs) {
            return lhs.tree_ == rhs.tree_;
        }

        /*
         * 指定友元函数，重载小于运算符
         */
        friend bool operator<(const multimap &lhs, const multimap &rhs) {
            return lhs.tree_ < rhs.tree_;
        }
    };

    // =============================================================================================
    /*
     * 全局区域重载操作符
     */

    /*
     * 重载相等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator==(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator!=(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator<(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator>(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator<=(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于运算符
     */
    template<typename Key, typename T, typename Compare>
    bool operator>=(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename T, typename Compare>
    void swap(const multimap<Key, T, Compare> &lhs, const multimap<Key, T, Compare> &rhs) noexcept {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_MAP_H
