//
// Created by cqupt1811 on 2022/5/18.
//

/*
 * 这个头文件包含两个模板类 set 和 multiset
 * set      : 集合，键值即实值，集合内元素会自动排序，键值不允许重复
 * multiset : 集合，键值即实值，集合内元素会自动排序，键值允许重复
 * notes:
 * tinySTL::set<Key> / tinySTL::multiset<Key> 满足基本异常保证，对以下等函数做强异常安全保证：
 *      * emplace
 *      * emplace_hint
 *      * insert
 */

#ifndef TINYSTL_SET_H
#define TINYSTL_SET_H

#include "rb_tree.h"  /* 这个头文件包含一个模板类 红黑树 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 模板类 set，键值不允许重复
     * 参数 Key 代表键值类型，参数 Compare 代表键值比较方式，缺省使用 tinySTL::less
     */
    template<typename Key, typename Compare=tinySTL::less<Key>>
    class set {
    public:
        /*
         * 类中使用的别名
         * set的key与value相同类型
         */
        typedef Key key_type;
        typedef Key value_type;
        typedef Compare key_compare;
        typedef Compare value_compare;

    private:
        /*
         * 以 tinySTL::rb_tree 作为底层机制
         */
        typedef tinySTL::rb_tree<value_type, key_compare> base_type;
        base_type tree_;

    public:
        /*
         * 使用红黑树定义的类型
         */
        typedef typename base_type::node_type node_type;
        /* 因为set值是不允许修改，所以指针,引用以及迭代器皆为const型 */
        typedef typename base_type::const_pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::const_reference reference;
        typedef typename base_type::const_reference const_reference;
        typedef typename base_type::const_iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::const_reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;
        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::allocator_type allocator_type;

    public:
        /*
         * 由编译器生成默认构造函数
         */
        set() = default;

        /*
         * 有参构造函数，从迭代器间的值构造set
         * 类成员模板
         */
        template<typename InputIterator>
        set(InputIterator first, InputIterator last):tree_() {
            tree_.insert_unique(first, last);
        }

        /*
         * 支持初始化列表构造set，即{}形式构造
         */
        set(std::initializer_list<value_type> i_list) : tree_() {
            tree_.insert_unique(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝构造函数
         */
        set(const set &rhs) : tree_(rhs.tree_) {

        }

        /*
         * 移动构造函数
         */
        set(set &&rhs) noexcept: tree_(tinySTL::move(rhs.tree_)) {

        }

        /*
         * 拷贝复制函数
         */
        set &operator=(const set &rhs) {
            tree_ = rhs.tree_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        set &operator=(set &&rhs) {
            tree_ = tinySTL::move(rhs.tree_);
            return *this;
        }

        /*
         * 支持初始化列表方式赋值
         */
        set &operator=(std::initializer_list<value_type> i_list) {
            tree_.clear();
            tree_.insert_unique(i_list.begin(), i_list.end());
            return *this;
        }

    public:
        /*
         * 返回键比较器实例
         */
        key_compare key_comp() const {
            return tree_.key_comp();
        }

        /*
         * 返回值比较器实例
         */
        value_compare value_comp() const {
            return tree_.key_comp();
        }

        /*
         * 返回空间配置器实例
         */
        allocator_type get_allocator() const {
            return tree_.get_allocator();
        }

        /*
         * 迭代器相关
         */

        /*
         * 返回头部迭代器
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
         * 查询set是否为空
         */
        bool empty() const noexcept {
            return tree_.empty();
        }

        /*
         * 查询set的大小
         */
        size_type size() const noexcept {
            return tree_.size();
        }

        /*
         * 查询set最大存储多少元素
         */
        size_type max_size() const noexcept {
            return tree_.max_size();
        }

        /*
         * 插入与删除相关操作
         */

        /*
         * 在set中新构建一个元素
         * 类成员模板
         */
        template<typename ...Args>
        pair<iterator, bool> emplace(Args &&...args) {
            return tree_.emplace_unique(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在set中新构建一个元素,带有hint的版本
         * 类成员模板
         */
        template<typename ...Args>
        iterator emplace_hint(iterator hint, Args &&...args) {
            return tree_.emplace_unique_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 在set中插入一个元素
         */
        pair<iterator, bool> insert(const value_type &value) {
            return tree_.insert_unique(value);
        }

        /*
         * 在set中插入一个元素 移动语义
         */
        pair<iterator, bool> insert(value_type &&value) {
            return tree_.insert_unique(tinySTL::move(value));
        }

        /*
         * 在set中插入一个元素 带有hint
         */
        iterator insert(iterator hint, const value_type &value) {
            return tree_.insert_unique(hint, value);
        }

        /*
         * 在set中插入一个元素 移动语义 带有hint
         */
        iterator insert(iterator hint, value_type &&value) {
            return tree_.insert_unique(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器间的值
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_unique(first, last);
        }

        /*
         * 擦除指定位置的元素
         */
        void erase(iterator position) {
            tree_.erase(position);
        }

        /*
         * 擦除指定键的元素
         */
        size_type erase(const key_type &key) {
            return tree_.erase_unique(key);
        }

        /*
         * 擦除指定区间的元素
         */
        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        /*
         * 清除set中所有元素
         */
        void clear() {
            tree_.clear();
        }

        /*
         * set 相关操作
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
         * 计算键为key的元素个数
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
         * set类的swap函数
         */
        void swap(set &rhs) noexcept {
            tree_.swap(rhs.tree_);
        }

    public:
        /*
         * 指定友元函数，重载相等于运算符
         */
        friend bool operator==(const set &lhs, const set &rhs) {
            return lhs.tree_ == rhs.tree_;
        }

        /*
         * 指定友元函数，重载小于运算符
         */
        friend bool operator<(const set &lhs, const set &rhs) {
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
    template<typename Key, typename Compare>
    bool operator==(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于运算符
     */
    template<typename Key, typename Compare>
    bool operator!=(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于运算符
     */
    template<typename Key, typename Compare>
    bool operator<(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于运算符
     */
    template<typename Key, typename Compare>
    bool operator>(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于运算符
     */
    template<typename Key, typename Compare>
    bool operator<=(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于运算符
     */
    template<typename Key, typename Compare>
    bool operator>=(const set<Key, Compare> &lhs, const set<Key, Compare> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename Compare>
    void swap(set<Key, Compare> &lhs, set<Key, Compare> &rhs) noexcept {
        lhs.swap(rhs);
    }


    // =====================================================================================================

    /*
     * 模板类 multiset，键值允许重复
     * 参数 Key 代表键值类型，参数 Compare 代表键值比较方式，缺省使用 tinySTL::less
     */
    template<typename Key, typename Compare=tinySTL::less<Key>>
    class multiset {
        /*
         * 类中使用的别名
         * multiset的key与value相同类型
         */
        typedef Key key_type;
        typedef Key value_type;
        typedef Compare key_compare;
        typedef Compare value_compare;

    private:
        /*
         * 以 tinySTL::rb_tree 作为底层机制
         */
        typedef tinySTL::rb_tree<value_type, key_compare> base_type;
        base_type tree_;

    public:
        /*
         * 使用红黑树定义的类型
         */
        typedef typename base_type::node_type node_type;
        /* 因为set值是不允许修改，所以指针,引用以及迭代器皆为const型 */
        typedef typename base_type::const_pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::const_reference reference;
        typedef typename base_type::const_reference const_reference;
        typedef typename base_type::const_iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::const_reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;
        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::allocator_type allocator_type;

    public:
        /*
         * 由编译器生成默认构造函数
         */
        multiset()=default;

        /*
         * 有参构造函数，从迭代器间的值构造set
         * 类成员模板
         */
        template<typename InputIterator>
        multiset(InputIterator first, InputIterator last):tree_() {
            tree_.insert_multi(first, last);
        }

        /*
         * 支持初始化列表构造multiset，即{}形式构造
         */
        multiset(std::initializer_list<value_type> i_list) : tree_() {
            tree_.insert_multi(i_list.begin(), i_list.end());
        }

        /*
         * 拷贝构造函数
         */
        multiset(const multiset &rhs) : tree_(rhs.tree_) {

        }

        /*
         * 移动构造函数
         */
        multiset(multiset &&rhs) noexcept: tree_(tinySTL::move(rhs.tree_)) {

        }

        /*
         * 拷贝复制函数
         */
        multiset &operator=(const multiset &rhs) {
            tree_ = rhs.tree_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        multiset &operator=(multiset &&rhs) {
            tree_ = tinySTL::move(rhs.tree_);
            return *this;
        }

        /*
         * 支持初始化列表方式赋值
         */
        multiset &operator=(std::initializer_list<value_type> i_list) {
            tree_.clear();
            tree_.insert_multi(i_list.begin(), i_list.end());
            return *this;
        }

    public:
        /*
         * 返回键比较器实例
         */
        key_compare key_comp() const {
            return tree_.key_comp();
        }

        /*
         * 返回值比较器实例
         */
        value_compare value_comp() const {
            return tree_.key_comp();
        }

        /*
         * 返回空间配置器实例
         */
        allocator_type get_allocator() const {
            return tree_.get_allocator();
        }

        /*
         * 迭代器相关
         */

        /*
         * 返回头部迭代器
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
         * 插入与删除相关操作
         */

        /*
         * 在multiset中新构建一个元素
         * 类成员模板
         */
        template<typename ...Args>
        iterator emplace(Args &&...args) {
            return tree_.emplace_multi(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在multiset中新构建一个元素,带有hint的版本
         * 类成员模板
         */
        template<typename ...Args>
        iterator emplace_hint(iterator hint, Args &&...args) {
            return tree_.emplace_multi_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 在multiset中插入一个元素
         */
        iterator insert(const value_type &value) {
            return tree_.insert_multi(value);
        }

        /*
         * 在multiset中插入一个元素 移动语义
         */
        iterator insert(value_type &&value) {
            return tree_.insert_multi(tinySTL::move(value));
        }

        /*
         * 在multiset中插入一个元素 带有hint
         */
        iterator insert(iterator hint, const value_type &value) {
            return tree_.insert_multi(hint, value);
        }

        /*
         * 在multiset中插入一个元素 移动语义 带有hint
         */
        iterator insert(iterator hint, value_type &&value) {
            return tree_.insert_multi(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器间的值
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            tree_.insert_multi(first, last);
        }

        /*
         * 擦除指定位置的元素
         */
        void erase(iterator position) {
            tree_.erase(position);
        }

        /*
         * 擦除指定键的元素
         */
        size_type erase(const key_type &key) {
            return tree_.erase_multi(key);
        }

        /*
         * 擦除指定区间的元素
         */
        void erase(iterator first, iterator last) {
            tree_.erase(first, last);
        }

        /*
         * 清除multiset中所有元素
         */
        void clear() {
            tree_.clear();
        }

        /*
         * multiset 相关操作
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
         * 计算键为key的元素个数
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
         * set类的swap函数
         */
        void swap(multiset &rhs) noexcept {
            tree_.swap(rhs.tree_);
        }

    public:
        /*
         * 指定友元函数，重载相等于运算符
         */
        friend bool operator==(const multiset &lhs, const multiset &rhs) {
            return lhs.tree_ == rhs.tree_;
        }

        /*
         * 指定友元函数，重载小于运算符
         */
        friend bool operator<(const multiset &lhs, const multiset &rhs) {
            return lhs.tree_ < rhs.tree_;
        }
    };

    // ========================================================================================

    /*
     * 全局区域重载操作符
     */

    /*
     * 重载相等于运算符
     */
    template<typename Key, typename Compare>
    bool operator==(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于运算符
     */
    template<typename Key, typename Compare>
    bool operator!=(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于运算符
     */
    template<typename Key, typename Compare>
    bool operator<(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return lhs < rhs;
    }

    /*
     * 重载大于运算符
     */
    template<typename Key, typename Compare>
    bool operator>(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于运算符
     */
    template<typename Key, typename Compare>
    bool operator<=(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于运算符
     */
    template<typename Key, typename Compare>
    bool operator>=(const multiset<Key, Compare> &lhs, const multiset<Key, Compare> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename Compare>
    void swap(multiset<Key, Compare> &lhs, multiset<Key, Compare> &rhs) noexcept {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_SET_H
