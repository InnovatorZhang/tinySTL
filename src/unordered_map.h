//
// Created by cqupt1811 on 2022/5/20.
//

/*
 * 个头文件包含两个模板类 unordered_map 和 unordered_multimap
 * 功能与用法与 map 和 multimap 类似，不同的是使用 hashtable 作为底层实现机制，容器内的元素不会自动排序
 * notes:
 * 异常保证：
 * tinySTL::unordered_map<Key, T> / tinySTL::unordered_multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
 *      * emplace
 *      * emplace_hint
 *       * insert
 */

#ifndef TINYSTL_UNORDERED_MAP_H
#define TINYSTL_UNORDERED_MAP_H

#include "hashtable.h"

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * 模板类 unordered_map，键值不允许重复
     * 参数 Key 代表键值类型，参数 T 代表实值类型，参数 HashFun 代表哈希函数，缺省使用 tinySTL::hash
     * 参数 KeyEqual 代表键值比较方式，缺省使用 tinSTL::equal_to
     */
    template<typename Key, typename T, typename HashFun=tinySTL::hash<Key>, typename KeyEqual=tinySTL::equal_to<Key>>
    class unordered_map {
    public:
        /*
         * 定义相关类型的别名，使用hashtable的型别
         */
        /* 这里传入的类型为 tinySTL::pair<const Key, T>，这也是后面的value_type*/
        typedef hashtable<tinySTL::pair<const Key, T>, HashFun, KeyEqual> base_type;

        typedef typename base_type::allocator_type allocator_type;
        typedef typename base_type::key_type key_type;
        typedef typename base_type::mapped_type mapped_type;
        typedef typename base_type::value_type value_type;
        typedef typename base_type::hasher hasher;
        typedef typename base_type::key_equal key_equal;

        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::reference reference;
        typedef typename base_type::const_reference const_reference;

        /*
         * 定义迭代器别名
         */
        typedef typename base_type::iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::local_iterator local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        /*
         * 获取空间配置器实例
         */
        allocator_type get_allocator() const {
            return ht_.get_allocator();
        }

    private:
        /*
         * 使用 hashtable 作为底层机制
         */
        base_type ht_;

    public:
        /*
         * 默认构造函数
         */
        unordered_map() : ht_(100, HashFun(), KeyEqual()) {

        }

        /*
         * 显式有参构造函数
         */
        explicit unordered_map(size_type bucket_count, const HashFun &hash = HashFun(),
                               const KeyEqual &equal = KeyEqual())
                : ht_(bucket_count, hash, equal) {

        }

        /*
         * 构造函数，使用迭代器间的数据构建unordered_map
         * 类成员模板
         */
        template<typename InputIterator>
        unordered_map(InputIterator first, InputIterator last,
                      const size_type bucket_count = 100,
                      const HashFun &hash = HashFun(),
                      const KeyEqual &equal = KeyEqual())
                :ht_(tinySTL::max(bucket_count, static_cast<size_type>(tinySTL::distance(first, last))), hash, equal) {
            for(;first!=last;++first){
                ht_.insert_unique_noresize(*first);
            }
        }

        /*
         * 支持使用初始化列表对unordered_map初始化，即{}的形式
         */
        unordered_map(std::initializer_list<value_type> i_list,
                      const size_type bucket_count = 100,
                      const HashFun &hash = HashFun(),
                      const KeyEqual &equal = KeyEqual())
                : ht_(tinySTL::max(bucket_count, static_cast<size_type>(i_list.size())), hash, equal) {
            for (auto first = i_list.begin(), last = i_list.end(); first != last; ++first) {
                ht_.insert_unique_noresize(*first);
            }
        }

        /*
         * 拷贝构造函数
         */
        unordered_map(const unordered_map &rhs) : ht_(rhs.ht_) {

        }

        /*
         * 移动构造函数
         */
        unordered_map(unordered_map &&rhs) noexcept: ht_(tinySTL::move(rhs.ht_)) {

        }

        /*
         * 拷贝赋值函数
         */
        unordered_map &operator=(const unordered_map &rhs) {
            ht_ = rhs.ht_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        unordered_map &operator=(unordered_map &&rhs) {
            ht_ = tinySTL::move(rhs.ht_);
            return *this;
        }

        /*
         * 支持初始化列表的方式赋值，即{}
         */
        unordered_map &operator=(std::initializer_list<value_type> i_list) {
            ht_.clear();
            ht_.reserve(i_list.size());
            for (auto first = i_list.begin(), last = i_list.end(); first != last; ++first) {
                ht_.insert_unique_noresize(*first);
            }
            return *this;
        }

        /*
         * 由编译器自动生成析构函数
         */
        ~unordered_map() = default;

    public:
        /*
         * 迭代器相关函数
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return ht_.begin();
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return ht_.begin();
        }

        /*
         * 获取尾部迭代器
         */
        iterator end() noexcept {
            return ht_.end();
        }

        /*
         * 获取尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return ht_.end();
        }

        /*
         * 获取常量头部迭代器 const重载
         */
        const_iterator cbegin() const noexcept {
            return ht_.cbegin();
        }

        /*
         * 获取常量尾部迭代器 const重载
         */
        const_iterator cend() const noexcept {
            return ht_.cend();
        }

        /*
         * 容量相关函数
         */

        /*
         * 查询容量是否为空
         */
        bool empty() const noexcept {
            return ht_.empty();
        }

        /*
         * 查询unordered_map大小
         */
        size_type size() const noexcept {
            return ht_.size();
        }

        /*
         * 查询unordered_map的最大容量
         */
        size_type max_size() const noexcept {
            return ht_.max_size();
        }

        /*
         * 修改容器的操作
         */

        /*
         * 在unordered_map中构建一个新元素
         * 成员函数模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        tinySTL::pair<iterator, bool> emplace(Args &&...args) {
            return ht_.emplace_unique(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在unordered_map中构建一个新元素 带有hint
         * 成员函数模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_hint(const_iterator hint, Args &&...args) {
            return ht_.emplace_unique_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 插入值到unordered_map中
         */
        tinySTL::pair<iterator, bool> insert(const value_type &value) {
            return ht_.insert_unique(value);
        }

        /*
         * 插入值到unordered_map中 移动语义
         */
        tinySTL::pair<iterator, bool> insert(value_type &&value) {
            return ht_.emplace_unique(tinySTL::move(value));
        }

        /*
         * 插入值到unordered_map中 带有hint
         */
        iterator insert(const_iterator hint, const value_type &value) {
            return ht_.insert_unique_use_hint(hint, value);
        }

        /*
         * 插入值到unordered_map中 带有hint 移动语义
         */
        iterator insert(const_iterator hint, value_type &&value) {
            return ht_.insert_unique_use_hint(hint, tinySTL::move(value));
        }

        /*
         * 将迭代器间的数据插入到unordered_map中
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            ht_.insert_unique(first, last);
        }

        /*
         * 删除指定元素
         */
        void erase(iterator it) {
            ht_.erase(it);
        }

        /*
         * 删除指定区间元素
         */
        void erase(iterator first, iterator last) {
            ht_.erase(first, last);
        }

        /*
         * 删除指定key值元素
         */
        size_type erase(const key_type &key) {
            return ht_.erase_unique(key);
        }

        /*
         * 清空unordered_map中所有元素
         */
        void clear() {
            ht_.clear();
        }

        /*
         * 交换两个unordered_map对象
         */
        void swap(unordered_map &other) noexcept {
            ht_.swap(other.ht_);
        }

        /*
         * 查找相关函数
         */

        /*
         * 获取指定键为key的元素
         */
        mapped_type &at(const key_type &key){
            iterator it=ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        /*
         * 获取指定键为key的元素 const重载
         */
        const mapped_type &at(const key_type &key)const{
            const_iterator it=ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        /*
         * 重载操作符 []
         */
        mapped_type &operator[](const key_type &key){
            iterator it=ht_.find(key);
            if(it.node== nullptr){
                /* 若没有找到，就将此值加入map中 */
                it = ht_.emplace_unique(key,T{}).first;
            }
            return it->second;
        }

        /*
         * 重载操作符 [] 移动语义
         */
        mapped_type &operator[](key_type &&key){
            iterator it=ht_.find(key);
            if(it.node== nullptr){
                /* 若没有找到，就将此值加入map中 */
                it = ht_.emplace_unique(tinySTL::move(key),T{}).first;
            }
            return it->second;
        }

        /*
         * 统计键为key的元素数量
         */
        size_type count(const key_type &key) const {
            return ht_.count(key);
        }

        /*
         * 查找指定键元素
         */
        iterator find(const key_type &key) {
            return ht_.find(key);
        }

        /*
         * 查找指定键元素 const重载
         */
        const_iterator find(const key_type &key) const {
            return ht_.find(key);
        }

        /*
         * 查找键为key的的区间
         */
        tinySTL::pair<iterator, iterator> equal_range(const key_type &key) {
            return ht_.equal_range_unique(key);
        }

        /*
         * 查找键为key的的区间 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
            return ht_.equal_range_unique(key);
        }

        /*
         * bucket相关接口函数
         */

        /*
         * 获取指定bucket中链表的头部迭代器
         */
        local_iterator begin(size_type n) noexcept {
            return ht_.begin(n);
        }

        /*
         * 获取指定bucket中链表的头部迭代器 const重载
         */
        const_local_iterator begin(size_type n) const noexcept {
            return ht_.begin(n);
        }

        /*
         * 获取指定bucket中链表的尾部迭代器
         */
        local_iterator end(size_type n) noexcept {
            return ht_.end(n);
        }

        /*
         * 获取指定bucket中链表的尾部迭代器 const重载
         */
        const_local_iterator end(size_type n) const noexcept {
            return ht_.end(n);
        }

        /*
         * 获取指定bucket中链表的常量头部迭代器
         */
        const_local_iterator cbegin(size_type n) const noexcept {
            return ht_.cbegin(n);
        }

        /*
         * 获取指定bucket中链表的常量尾部迭代器
         */
        const_local_iterator cend(size_type n) const noexcept {
            return ht_.cend(n);
        }

        /*
         * 获取bucket的数量
         */
        size_type bucket_count() const noexcept {
            return ht_.bucket_count();
        }

        /*
         * 获取最大的bucket数量
         */
        size_type max_bucket_count() const noexcept {
            return ht_.max_bucket_count();
        }

        /*
         * 获取指定bucket的中链表的长度
         */
        size_type bucket_size(size_type n)const noexcept{
            return ht_.bucket_size(n);
        }

        /*
         * 根据key获取对应bucket编号
         */
        size_type bucket(const key_type &key) const {
            return ht_.bucket(key);
        }

        /*
         * hash相关的函数
         */

        /*
         * 获取承载因子
         */
        float load_factor() const noexcept {
            return ht_.load_factor();
        }

        /*
         * 获取最大承载因子
         */
        float max_load_factor() const noexcept {
            return ht_.max_load_factor();
        }

        /*
         * 设置最大承载因子
         */
        void max_load_factor(float ml) {
            ht_.max_load_factor(ml);
        }

        /*
         * 对hashtable容量更改为count，重新排布元素
         */
        void rehash(size_type count) {
            ht_.rehash(count);
        }

        /*
         * 分配能够装下count个元素的bucket空间
         */
        void reserve(size_type count) {
            ht_.reserve(count);
        }

        /*
         * 返回hash函数对象
         */
        hasher hash_fcn() const {
            return ht_.hash_fcn();
        }

        /*
         * 返回key比较函数对象
         */
        key_equal key_eq() const {
            return ht_.key_eq();
        }

    public:
        /*
         * 指定此类的友元函数
         */

        /*
         * 相等于操作符重载
         */
        friend bool operator==(const unordered_map &lhs, const unordered_map &rhs) {
            return lhs.ht_.equal_to_unique(rhs.ht_);
        }

        /*
         * 不相等于操作符重载
         */
        friend bool operator!=(const unordered_map &lhs, const unordered_map &rhs) {
            return !lhs.ht_.equal_to_unique(rhs.ht_);
        }
    };

    // ========================================================================================

    /*
     * 全局区域重载比较操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    bool operator==(const unordered_map<Key, HashFun, KeyEqual> &lhs,
                    const unordered_map<Key, HashFun, KeyEqual> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于操作符
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    bool operator!=(const unordered_map<Key, HashFun, KeyEqual> &lhs,
                    const unordered_map<Key, HashFun, KeyEqual> &rhs) {
        return lhs != rhs;
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    void swap(unordered_map<Key, HashFun, KeyEqual> &lhs,
              unordered_map<Key, HashFun, KeyEqual> &rhs) noexcept {
        return lhs.swap(rhs);
    }

    // ===========================================================================================

    /*
     * 模板类 unordered_multimap，键值允许重复
     * 参数 Key 代表键值类型，参数 T 代表实值类型，参数 HashFun 代表哈希函数，缺省使用 tinySTL::hash
     * 参数 KeyEqual 代表键值比较方式，缺省使用 tinSTL::equal_to
     */
    template<typename Key, typename T, typename HashFun=tinySTL::hash<Key>, typename KeyEqual=tinySTL::equal_to<Key>>
    class unordered_multimap {
    public:
        /*
         * 定义相关类型的别名，使用hashtable的型别
         */
        /* 这里传入的类型为 tinySTL::pair<const Key, T>，这也是后面的value_type*/
        typedef hashtable<tinySTL::pair<const Key, T>, HashFun, KeyEqual> base_type;

        typedef typename base_type::allocator_type allocator_type;
        typedef typename base_type::key_type key_type;
        typedef typename base_type::mapped_type mapped_type;
        typedef typename base_type::value_type value_type;
        typedef typename base_type::hasher hasher;
        typedef typename base_type::key_equal key_equal;

        typedef typename base_type::size_type size_type;
        typedef typename base_type::difference_type difference_type;
        typedef typename base_type::pointer pointer;
        typedef typename base_type::const_pointer const_pointer;
        typedef typename base_type::reference reference;
        typedef typename base_type::const_reference const_reference;

        /*
         * 定义迭代器别名
         */
        typedef typename base_type::iterator iterator;
        typedef typename base_type::const_iterator const_iterator;
        typedef typename base_type::local_iterator local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        /*
         * 获取空间配置器实例
         */
        allocator_type get_allocator() const {
            return ht_.get_allocator();
        }

    private:
        /*
         * 使用 hashtable 作为底层机制
         */
        base_type ht_;

    public:
        /*
         * 默认构造函数
         */
        unordered_multimap() : ht_(100, HashFun(), KeyEqual()) {

        }

        /*
         * 显式有参构造函数
         */
        explicit unordered_multimap(size_type bucket_count, const HashFun &hash = HashFun(),
        const KeyEqual &equal = KeyEqual())
        : ht_(bucket_count, hash, equal) {

        }

        /*
         * 构造函数，使用迭代器间的数据构建unordered_multimap
         * 类成员模板
         */
        template<typename InputIterator>
        unordered_multimap(InputIterator first, InputIterator last,
        const size_type bucket_count = 100,
        const HashFun &hash = HashFun(),
        const KeyEqual &equal = KeyEqual())
        :ht_(tinySTL::max(bucket_count, static_cast<size_type>(tinySTL::distance(first, last))), hash, equal) {
            for(;first!=last;++first){
                ht_.insert_multi_noresize(*first);
            }
        }

        /*
         * 支持使用初始化列表对unordered_map初始化，即{}的形式
         */
        unordered_multimap(std::initializer_list<value_type> i_list,
        const size_type bucket_count = 100,
        const HashFun &hash = HashFun(),
        const KeyEqual &equal = KeyEqual())
        : ht_(tinySTL::max(bucket_count, static_cast<size_type>(i_list.size())), hash, equal) {
            for (auto first = i_list.begin(), last = i_list.end(); first != last; ++first) {
                ht_.insert_multi_noresize(*first);
            }
        }

        /*
         * 拷贝构造函数
         */
        unordered_multimap(const unordered_multimap &rhs) : ht_(rhs.ht_) {

        }

        /*
         * 移动构造函数
         */
        unordered_multimap(unordered_multimap &&rhs) noexcept: ht_(tinySTL::move(rhs.ht_)) {

        }

        /*
         * 拷贝赋值函数
         */
        unordered_multimap &operator=(const unordered_multimap &rhs) {
            ht_ = rhs.ht_;
            return *this;
        }

        /*
         * 移动赋值函数
         */
        unordered_multimap &operator=(unordered_multimap &&rhs) {
            ht_ = tinySTL::move(rhs.ht_);
            return *this;
        }

        /*
         * 支持初始化列表的方式赋值，即{}
         */
        unordered_multimap &operator=(std::initializer_list<value_type> i_list) {
            ht_.clear();
            ht_.reserve(i_list.size());
            for (auto first = i_list.begin(), last = i_list.end(); first != last; ++first) {
                ht_.insert_multi_noresize(*first);
            }
            return *this;
        }

        /*
         * 由编译器自动生成析构函数
         */
        ~unordered_multimap() = default;

    public:
        /*
         * 迭代器相关函数
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return ht_.begin();
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return ht_.begin();
        }

        /*
         * 获取尾部迭代器
         */
        iterator end() noexcept {
            return ht_.end();
        }

        /*
         * 获取尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return ht_.end();
        }

        /*
         * 获取常量头部迭代器 const重载
         */
        const_iterator cbegin() const noexcept {
            return ht_.cbegin();
        }

        /*
         * 获取常量尾部迭代器 const重载
         */
        const_iterator cend() const noexcept {
            return ht_.cend();
        }

        /*
         * 容量相关函数
         */

        /*
         * 查询容量是否为空
         */
        bool empty() const noexcept {
            return ht_.empty();
        }

        /*
         * 查询unordered_multimap大小
         */
        size_type size() const noexcept {
            return ht_.size();
        }

        /*
         * 查询unordered_multimap的最大容量
         */
        size_type max_size() const noexcept {
            return ht_.max_size();
        }

        /*
         * 修改容器的操作
         */

        /*
         * 在unordered_multimap中构建一个新元素
         * 成员函数模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace(Args &&...args) {
            return ht_.emplace_multi(tinySTL::forward<Args>(args)...);
        }

        /*
         * 在unordered_multimap中构建一个新元素 带有hint
         * 成员函数模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_hint(const_iterator hint, Args &&...args) {
            return ht_.emplace_multi_use_hint(hint, tinySTL::forward<Args>(args)...);
        }

        /*
         * 插入值到unordered_multimap中
         */
        iterator insert(const value_type &value) {
            return ht_.insert_multi(value);
        }

        /*
         * 插入值到unordered_multimap中 移动语义
         */
        iterator insert(value_type &&value) {
            return ht_.emplace_multi(tinySTL::move(value));
        }

        /*
         * 插入值到unordered_multimap中 带有hint
         */
        iterator insert(const_iterator hint, const value_type &value) {
            return ht_.insert_multi_use_hint(hint, value);
        }

        /*
         * 插入值到unordered_multimap中 带有hint 移动语义
         */
        iterator insert(const_iterator hint, value_type &&value) {
            return ht_.insert_multi_use_hint(hint, tinySTL::move(value));
        }

        /*
         * 将迭代器间的数据插入到unordered_multimap中
         * 类成员模板
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            ht_.insert_multi(first, last);
        }

        /*
         * 删除指定元素
         */
        void erase(iterator it) {
            ht_.erase(it);
        }

        /*
         * 删除指定区间元素
         */
        void erase(iterator first, iterator last) {
            ht_.erase(first, last);
        }

        /*
         * 删除指定key值元素
         */
        size_type erase(const key_type &key) {
            return ht_.erase_multi(key);
        }

        /*
         * 清空unordered_multimap中所有元素
         */
        void clear() {
            ht_.clear();
        }

        /*
         * 交换两个unordered_multimap对象
         */
        void swap(unordered_multimap &other) noexcept {
            ht_.swap(other.ht_);
        }

        /*
         * 统计键为key的元素数量
         */
        size_type count(const key_type &key) const {
            return ht_.count(key);
        }

        /*
         * 查找指定键元素
         */
        iterator find(const key_type &key) {
            return ht_.find(key);
        }

        /*
         * 查找指定键元素 const重载
         */
        const_iterator find(const key_type &key) const {
            return ht_.find(key);
        }

        /*
         * 查找键为key的的区间
         */
        tinySTL::pair<iterator, iterator> equal_range(const key_type &key) {
            return ht_.equal_range_multi(key);
        }

        /*
         * 查找键为key的的区间 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
            return ht_.equal_range_multi(key);
        }

        /*
         * bucket相关接口函数
         */

        /*
         * 获取指定bucket中链表的头部迭代器
         */
        local_iterator begin(size_type n) noexcept {
            return ht_.begin(n);
        }

        /*
         * 获取指定bucket中链表的头部迭代器 const重载
         */
        const_local_iterator begin(size_type n) const noexcept {
            return ht_.begin(n);
        }

        /*
         * 获取指定bucket中链表的尾部迭代器
         */
        local_iterator end(size_type n) noexcept {
            return ht_.end(n);
        }

        /*
         * 获取指定bucket中链表的尾部迭代器 const重载
         */
        const_local_iterator end(size_type n) const noexcept {
            return ht_.end(n);
        }

        /*
         * 获取指定bucket中链表的常量头部迭代器
         */
        const_local_iterator cbegin(size_type n) const noexcept {
            return ht_.cbegin(n);
        }

        /*
         * 获取指定bucket中链表的常量尾部迭代器
         */
        const_local_iterator cend(size_type n) const noexcept {
            return ht_.cend(n);
        }

        /*
         * 获取bucket的数量
         */
        size_type bucket_count() const noexcept {
            return ht_.bucket_count();
        }

        /*
         * 获取最大的bucket数量
         */
        size_type max_bucket_count() const noexcept {
            return ht_.max_bucket_count();
        }

        /*
         * 获取指定bucket的中链表的长度
         */
        size_type bucket_size(size_type n)const noexcept{
            return ht_.bucket_size(n);
        }

        /*
         * 根据key获取对应bucket编号
         */
        size_type bucket(const key_type &key) const {
            return ht_.bucket(key);
        }

        /*
         * hash相关的函数
         */

        /*
         * 获取承载因子
         */
        float load_factor() const noexcept {
            return ht_.load_factor();
        }

        /*
         * 获取最大承载因子
         */
        float max_load_factor() const noexcept {
            return ht_.max_load_factor();
        }

        /*
         * 设置最大承载因子
         */
        void max_load_factor(float ml) {
            ht_.max_load_factor(ml);
        }

        /*
         * 对hashtable容量更改为count，重新排布元素
         */
        void rehash(size_type count) {
            ht_.rehash(count);
        }

        /*
         * 分配能够装下count个元素的bucket空间
         */
        void reserve(size_type count) {
            ht_.reserve(count);
        }

        /*
         * 返回hash函数对象
         */
        hasher hash_fcn() const {
            return ht_.hash_fcn();
        }

        /*
         * 返回key比较函数对象
         */
        key_equal key_eq() const {
            return ht_.key_eq();
        }

    public:
        /*
         * 指定此类的友元函数
         */

        /*
         * 相等于操作符重载
         */
        friend bool operator==(const unordered_multimap &lhs, const unordered_multimap &rhs) {
            return lhs.ht_.equal_to_multi(rhs.ht_);
        }

        /*
         * 不相等于操作符重载
         */
        friend bool operator!=(const unordered_multimap &lhs, const unordered_multimap &rhs) {
            return !lhs.ht_.equal_to_multi(rhs.ht_);
        }
    };

    // ========================================================================================

    /*
     * 全局区域重载比较操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    bool operator==(const unordered_multimap<Key, HashFun, KeyEqual> &lhs,
                    const unordered_multimap<Key, HashFun, KeyEqual> &rhs) {
        return lhs == rhs;
    }

    /*
     * 重载不相等于操作符
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    bool operator!=(const unordered_multimap<Key, HashFun, KeyEqual> &lhs,
                    const unordered_multimap<Key, HashFun, KeyEqual> &rhs) {
        return lhs != rhs;
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename Key, typename HashFun, typename KeyEqual>
    void swap(unordered_multimap<Key, HashFun, KeyEqual> &lhs,
              unordered_multimap<Key, HashFun, KeyEqual> &rhs) noexcept {
        return lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_UNORDERED_MAP_H
