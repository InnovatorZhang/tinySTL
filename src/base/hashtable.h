//
// Created by cqupt1811 on 2022/5/19.
//

/*
 * 这个头文件包含了一个模板类 hashtable
 * hashtable : 哈希表，使用开链法处理冲突
 */

#ifndef TINYSTL_HASHTABLE_H
#define TINYSTL_HASHTABLE_H

#include <initializer_list>  /* std::initializer_list<T> 类型对象是一个访问 const T 类型对象数组的轻量代理对象 */
#include "algo.h"  /* 这个头文件包含了 tinySTL 的一系列算法 */
#include "functional.h"  /* 这个头文件包含了 tinySTL 的函数对象与哈希函数 */
#include "memory.h"  /* 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "vector.h"  /* 这个头文件包含一个模板类 vector */
#include "utils.h"  /* 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 */
#include "exceptdef.h"  /* 此文件中定义了异常相关的宏 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * hashtable的节点定义
     */
    template<typename T>
    struct hashtable_node {
        /* 指向下一个节点 */
        hashtable_node *next;
        /* 存储实值 */
        T value;

        /*
         * 由编译器生成默认构造函数
         */
        hashtable_node() = default;

        /*
         * 接收一个实值的构造函数
         */
        hashtable_node(const T &n) : next(nullptr), value(n) {

        }

        /*
         * 拷贝构造函数
         */
        hashtable_node(const hashtable_node &node) : next(node.next), value(node.value) {

        }

        /*
         * 移动构造函数
         */
        hashtable_node(hashtable_node &&node) : next(node.next), value(tinySTL::move(node.value)) {
            node.next = nullptr;
        }
    };

    // ===========================================================================================

    /*
     * 使用萃取机制萃取出类型T的value的键与值
     */
    template<typename T, bool>
    struct ht_value_traits_imp {
        typedef T key_type;
        typedef T mapped_type;
        typedef T value_type;

        /*
         * 获取key,类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value;
        }

        /*
         * 获取value,类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value;
        }
    };

    /*
     * 使用萃取机制萃取出类型T的value的键与值
     * 偏特化
     */
    template<typename T>
    struct ht_value_traits_imp<T, true> {
        /* 去除T::first_type中的const与volatile属性 */
        typedef typename std::remove_cv<typename T::first_type>::type key_type;
        typedef typename T::second_type mapped_type;
        typedef T value_type;

        /*
         * 获取key,类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value.first;
        }

        /*
         * 获取value,类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value;
        }
    };

    /*
     * 使用萃取机制萃取出类型T的value的键与值
     */
    template<typename T>
    struct ht_value_traits {
        /* 判断类型T是否为pair，根据判断结果调用偏特化的模板 */
        static constexpr bool is_map = tinySTL::is_pair<T>::value;
        /* 根据判断结果调用偏特化的模板 */
        typedef ht_value_traits_imp<T, is_map> value_traits_type;
        typedef typename value_traits_type::key_type key_type;
        typedef typename value_traits_type::mapped_type mapped_type;
        typedef typename value_traits_type::value_type value_type;

        /*
         * 获取key,类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value_traits_type::get_key(value);
        }

        /*
         * 获取value,类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value_traits_type::get_value(value);
        }
    };

    // ===============================================================================

    /*
     * 前向声明
     */
    template<typename T, typename HashFun, typename KeyEqual>
    class hashtable;

    template<typename T, typename HashFun, typename KeyEqual>
    struct ht_iterator;

    template<typename T, typename HashFun, typename KeyEqual>
    struct ht_const_iterator;

    template<typename T>
    struct ht_local_iterator;

    template<typename T>
    struct ht_const_local_iterator;

    // =========================================================================================

    /*
     * 定义hashtable的迭代器
     */

    /*
     * hashtable迭代器基类
     * 继承
     * template<class Category, class T, class Distance = ptrdiff_t,
     * class Pointer = T *, class Reference = T &>
     * struct iterator{ ... }
     * 可以自动定义迭代器所需的五种类型
     * hashtable 的迭代器属于单向迭代器类型
     */
    template<typename T, typename HashFun, typename KeyEqual>
    struct ht_iterator_base : public tinySTL::iterator<tinySTL::forward_iterator_tag, T> {
        /*
         * 定义类中使用的别名
         */
        typedef tinySTL::hashtable<T, HashFun, KeyEqual> hashtable;
        typedef tinySTL::ht_iterator_base<T, HashFun, KeyEqual> base;
        typedef tinySTL::ht_iterator<T, HashFun, KeyEqual> iterator;
        typedef tinySTL::ht_const_iterator<T, HashFun, KeyEqual> const_iterator;
        typedef tinySTL::hashtable_node<T> *node_ptr;
        typedef hashtable *contain_ptr;
        typedef const node_ptr const_node_ptr;
        typedef const contain_ptr const_contain_ptr;

        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        /*
         * 成员变量
         * node: 迭代器当前所指节点
         * ht: 保持与容器的连结
         */
        node_ptr node;
        contain_ptr ht;

        /*
         * 由编译器生成默认构造函数
         */
        ht_iterator_base() = default;

        /*
         * 重载相等于操作符
         */
        bool operator==(const base &rhs) const {
            return node == rhs.node;
        }

        /*
         * 重载不相等于操作符
         */
        bool operator!=(const base &rhs) const {
            return node != rhs.node;
        }
    };

    template<typename T, typename HashFun, typename KeyEqual>
    struct ht_iterator : public ht_iterator_base<T, HashFun, KeyEqual> {
        /*
         * 定义类中使用的别名
         */
        typedef tinySTL::ht_iterator_base<T, HashFun, KeyEqual> base;
        typedef typename base::hashtable hashtable;
        typedef typename base::iterator iterator;
        typedef typename base::const_iterator const_iterator;
        typedef typename base::node_ptr node_ptr;
        typedef typename base::contain_ptr contain_ptr;
        typedef typename base::size_type size_type;

        typedef iterator self;

        typedef tinySTL::ht_value_traits<T> value_traits;
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;

        /*
         * 使用基类中的node与ht变量
         */
        using base::node;
        using base::ht;

        /*
         * 由系统自动生成默认构造函数
         */
        ht_iterator() = default;

        /*
         * 有参构造函数，接收一个hashtable容器指针与节点指针
         */
        ht_iterator(node_ptr n, contain_ptr t) {
            node = n;
            ht = t;
        }

        /*
         * 拷贝构造函数
         */
        ht_iterator(const self &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }

        /*
         * 有参构造函数，接收一个const_iterator类对象
         */
        ht_iterator(const const_iterator &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }

        /*
         * 拷贝赋值函数
         */
        self &operator=(const self &rhs) {
            if (this != &rhs) {
                node = rhs.node;
                ht = rhs.ht;
            }
            return *this;
        }

        /*
         * 赋值函数，接受一个const_iterator类对象
         */
        self &operator=(const const_iterator &rhs) {
            if (this != &rhs) {
                node = rhs.node;
                ht = rhs.ht;
            }
            return *this;
        }

        /*
         * 重载操作符
         */

        /*
         * 重载 * 操作符
         */
        reference operator*() const {
            return node->value;
        }

        /*
         * 重载 -> 操作符
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载前置 ++ 操作符
         */
        self &operator++() {
            TINYSTL_DEBUG(node != nullptr);
            const node_ptr old = node;
            node = node->next;
            if (node == nullptr) {
                /* 如果下一个位置为空，跳到下一个 bucket 的起始处 */
                size_type index = ht->hash(value_traits::get_key(old->value));
                while (!node && ++index < ht->bucket_size_) {
                    /* 找到下一个node不为空或到了hashtable尾部就退出循环 */
                    node = ht->buckets_[index];
                }
            }
            return *this;
        }

        /*
         * 重载后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }
    };

    template<typename T, typename HashFun, typename KeyEqual>
    struct ht_const_iterator : public ht_iterator_base<T, HashFun, KeyEqual> {
        /*
         * 定义类中使用的别名
         */
        typedef tinySTL::ht_iterator_base<T, HashFun, KeyEqual> base;
        typedef typename base::hashtable hashtable;
        typedef typename base::iterator iterator;
        typedef typename base::const_iterator const_iterator;
        typedef typename base::const_node_ptr node_ptr;
        typedef typename base::const_contain_ptr contain_ptr;
        typedef typename base::size_type size_type;

        typedef const_iterator self;

        typedef tinySTL::ht_value_traits<T> value_traits;
        typedef T value_type;
        typedef const value_type *pointer;
        typedef const value_type &reference;

        /*
         * 使用基类中的node与ht变量
         */
        using base::node;
        using base::ht;

        /*
         * 由系统自动生成默认构造函数
         */
        ht_const_iterator() = default;

        /*
         * 有参构造函数，接收一个hashtable容器指针与节点指针
         */
        ht_const_iterator(node_ptr n, contain_ptr t) {
            node = n;
            ht = t;
        }

        /*
         * 拷贝构造函数
         */
        ht_const_iterator(const self &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }

        /*
         * 有参构造函数，接收一个const_iterator类对象
         */
        ht_const_iterator(const iterator &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }

        /*
         * 拷贝赋值函数
         */
        self &operator=(const self &rhs) {
            if (this != &rhs) {
                node = rhs.node;
                ht = rhs.ht;
            }
            return *this;
        }

        /*
         * 赋值函数，接受一个const_iterator类对象
         */
        self &operator=(const iterator &rhs) {
            if (this != &rhs) {
                node = rhs.node;
                ht = rhs.ht;
            }
            return *this;
        }

        /*
         * 重载操作符
         */

        /*
         * 重载 * 操作符
         */
        reference operator*() const {
            return node->value;
        }

        /*
         * 重载 -> 操作符
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载前置 ++ 操作符
         */
        self &operator++() {
            TINYSTL_DEBUG(node != nullptr);
            const node_ptr old = node;
            node = node->next;
            if (node == nullptr) {
                /* 如果下一个位置为空，跳到下一个 bucket 的起始处 */
                size_type index = ht->hash(value_traits::get_key(old->value));
                while (!node && ++index < ht->bucket_size_) {
                    /* 找到下一个node不为空或到了hashtable尾部就退出循环 */
                    node = ht->buckets_[index];
                }
            }
            return *this;
        }

        /*
         * 重载后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }
    };

    // ==========================================================================================

    /*
     * 定义hashtable的local_iterator
     */

    /*
     * 继承
     * template<class Category, class T, class Distance = ptrdiff_t,
     * class Pointer = T *, class Reference = T &>
     * struct iterator{ ... }
     * 可以自动定义迭代器所需的五种类型
     * hashtable 的迭代器属于单向迭代器类型
     */
    template<typename T>
    struct ht_local_iterator : public tinySTL::iterator<tinySTL::forward_iterator_tag, T> {
        /*
         * 定义类中使用的别名
         */
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef hashtable_node<T> *node_ptr;

        /*
         * 定义迭代器别名
         */
        typedef ht_local_iterator<T> self;
        typedef ht_local_iterator<T> local_iterator;
        typedef ht_const_local_iterator<T> const_local_iterator;

        /*
         * 定义成员变量
         * node: 指向对应节点的指针
         */
        node_ptr node;

        /*
         * 有参构造函数，接收一个hashtable_node指针
         */
        ht_local_iterator(node_ptr n) : node(n) {

        }

        /*
         * 拷贝构造函数
         */
        ht_local_iterator(const self &rhs) : node(rhs.node) {

        }

        /*
         * 有参构造函数，接受一个const_local_iterator类对象
         */
        ht_local_iterator(const const_local_iterator &rhs) : node(rhs.node) {

        }

        /*
         * 操作符重载
         */

        /*
         * 操作符 * 重载
         */
        reference operator*() const {
            return node->value;
        }

        /*
         * 操作符 -> 重载
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载前置 ++ 操作符
         */
        self &operator++() {
            TINYSTL_DEBUG(node != nullptr);
            node = node->next;
            return *this;
        }

        /*
         * 重载后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }

        /*
         * 重载相等于运算符
         */
        bool operator==(const self &other) const {
            return node == other.node;
        }

        /*
         * 重载不相等于运算符
         */
        bool operator!=(const self &other) const {
            return node != other.node;
        }
    };

    template<typename T>
    struct ht_const_local_iterator : public tinySTL::iterator<tinySTL::forward_iterator_tag, T> {
        /*
         * 定义类中使用的别名
         */
        typedef T value_type;
        typedef const value_type *pointer;
        typedef const value_type &reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef const hashtable_node<T> *node_ptr;

        /*
         * 定义迭代器别名
         */
        typedef ht_const_local_iterator<T> self;
        typedef ht_local_iterator<T> local_iterator;
        typedef ht_const_local_iterator<T> const_local_iterator;

        /*
         * 定义成员变量
         * node: 指向对应节点的指针
         */
        node_ptr node;

        /*
         * 有参构造函数，接收一个hashtable_node指针
         */
        ht_const_local_iterator(node_ptr n) : node(n) {

        }

        /*
         * 拷贝构造函数
         */
        ht_const_local_iterator(const self &rhs) : node(rhs.node) {

        }

        /*
         * 有参构造函数，接受一个const_local_iterator类对象
         */
        ht_const_local_iterator(const local_iterator &rhs) : node(rhs.node) {

        }

        /*
         * 操作符重载
         */

        /*
         * 操作符 * 重载
         */
        reference operator*() const {
            return node->value;
        }

        /*
         * 操作符 -> 重载
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载前置 ++ 操作符
         */
        self &operator++() {
            TINYSTL_DEBUG(node != nullptr);
            node = node->next;
            return *this;
        }

        /*
         * 重载后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }

        /*
         * 重载相等于运算符
         */
        bool operator==(const self &other) const {
            return node == other.node;
        }

        /*
         * 重载不相等于运算符
         */
        bool operator!=(const self &other) const {
            return node != other.node;
        }
    };

// ===========================================================================================
/*
 * 根据不同的操作系统位数定义一些常量
 * hashtable的每一次bucket的的大小
 */
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

    /*
     * 每个素数的计算获取规则
     * 1. 开始于 p = 101
     * 2. p = next_prime(p * 1.7)
     * 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
     * 4. 结束于 p = prev_prime(2 << 63 - 1)
     */
    static constexpr size_t ht_prime_list[] = {
            101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
            4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
            119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
            2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
            23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
            264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
            2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
            15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
            115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
            585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
            2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
            15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
            76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
            384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
            1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
            9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
            49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
            252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
            1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
            6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
    };

#else

#define PRIME_NUM 44

    /*
     * 每个素数的计算获取规则
     * 1. 开始于 p = 101
     * 2. p = next_prime(p * 1.7)
     * 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
     * 4. 结束于 p = prev_prime(2 << 31 - 1)
     */
    static constexpr size_t ht_prime_list[] = {
      101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
      10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
      402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
      10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
      176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
      2006689337u, 3010034021u, 4294967291u,
    };

#endif

    /*
     * 辅助函数
     * 找出最接近并大于等于 n 的那个质数
     */
    inline size_t ht_next_prime(size_t n) {
        const size_t *first = ht_prime_list;
        const size_t *last = ht_prime_list + PRIME_NUM;
        const size_t *pos = tinySTL::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    // ======================================================================================================

    /*
     * 模板类 hashtable
     * 参数 T 代表数据类型，参数 HashFun 代表哈希函数，参数 KeyEqual 代表键值相等的比较函数
     */
    template<typename T, typename HashFun, typename KeyEqual>
    class hashtable {
        /*
         * 声明迭代器类为友元类，因为迭代器需要访问私有成员buckets_与bucket_size_
         */
        friend struct tinySTL::ht_iterator<T, HashFun, KeyEqual>;
        friend struct tinySTL::ht_const_iterator<T, HashFun, KeyEqual>;

    public:
        /*
         * hashtable类的别名定义
         */
        typedef ht_value_traits<T> value_traits;
        typedef typename value_traits::key_type key_type;
        typedef typename value_traits::mapped_type mapped_type;
        typedef typename value_traits::value_type value_type;

        typedef HashFun hasher;
        typedef KeyEqual key_equal;
        typedef hashtable_node<value_type> node_type;
        typedef node_type *node_ptr;
        /* 使用vector存储hashtable中的指针，因为方便扩容 */
        typedef tinySTL::vector<node_ptr> bucket_type;

        /*
         * 定义空间配置器
         */
        typedef tinySTL::allocator<value_type> allocator_type;
        typedef tinySTL::allocator<value_type> data_allocator;
        typedef tinySTL::allocator<node_type> node_allocator;

        /*
         * 定义类型别名
         */
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        /*
         * 定义迭代器相关别名
         */
        typedef tinySTL::ht_iterator<T, HashFun, KeyEqual> iterator;
        typedef tinySTL::ht_const_iterator<T, HashFun, KeyEqual> const_iterator;
        typedef tinySTL::ht_local_iterator<T> local_iterator;
        typedef tinySTL::ht_const_local_iterator<T> const_local_iterator;

        /*
         * 返回空间配置器实例
         */
        allocator_type get_allocator() const {
            return data_allocator();
        }

    private:
        /*
         * 用以下六个参数来表现 hashtable
         * buckets_: 存储hashtable节点指针的vector类型对象
         * buckets_size_: buckets的大小
         * size_: 存储节点的个数
         * mlf_: max_load_factor最大承载参数，超过该值就需要扩容
         * hash_; hash对象，使用该对象计算hash值，找到hashtable中合适的位置存放
         * equal_: 判断键是否相同的函数对象
         */
        bucket_type buckets_;
        size_type bucket_size_;
        size_type size_;
        float mlf_;
        hasher hash_;
        key_equal equal_;

    private:
        /*
         * 判断键是否相等
         */
        bool is_equal(const key_type &key1, const key_type &key2) {
            return equal_(key1, key2);
        };

        /*
         * 判断键是否相等 const重载
         */
        bool is_equal(const key_type &key1, const key_type &key2) const {
            return equal_(key1, key2);
        };

        /*
         * 获取指定节点的const_iterator类型迭代器
         */
        const_iterator M_cit(node_ptr node) const noexcept {
            return const_iterator(node, const_cast<hashtable *>(this));
        }

        /*
         * 获取第一个节点位置的迭代器
         */
        iterator M_begin() noexcept {
            for (size_type n = 0; n < bucket_size_; ++n) {
                if (buckets_[n]) {
                    /* 找到第一个有节点的位置就返回 */
                    return iterator(buckets_[n], this);
                }
            }
            return iterator(nullptr, this);
        }

        /*
         * 获取第一个节点位置的迭代器 const重载
         */
        const_iterator M_begin() const noexcept {
            for (size_type n = 0; n < bucket_size_; ++n) {
                if (buckets_[n]) {
                    /* 找到第一个有节点的位置就返回 */
                    return M_cit(buckets_[n]);
                }
            }
            return M_cit(nullptr);
        }

    public:
        /*
         * 显式有参构造函数
         */
        explicit hashtable(size_type bucket_count, const hasher &hash = hasher(),
                           const key_equal &equal = key_equal())
                : size_(0), mlf_(1.0f), hash_(hash), equal_(equal) {
            init(bucket_count);
        }

        /*
         * 有参构造函数，由一堆迭代器间的数据构建hashtable
         * 类成员模板，迭代器至少为input_iterator_tag类型
         */
        template<typename Iter, typename std::enable_if<
                tinySTL::is_input_iterator<Iter>::value, int>::type = 0>
        hashtable(Iter first, Iter last,
                  size_type bucket_count, const hasher &hash = hasher(),
                  const key_equal &equal = key_equal())
                :size_(tinySTL::distance(first, last)),
                 mlf_(1.0f), hash_(hash), equal_(equal) {
            init(tinySTL::max(bucket_count, size_));
        }

        /*
         * 拷贝构造函数
         */
        hashtable(const hashtable &rhs) : hash_(rhs.hash_), equal_(rhs.equal_) {
            copy_init(rhs);
        }

        /*
         * 移动构造函数
         */
        hashtable(hashtable &&rhs) noexcept
                : buckets_(tinySTL::move(rhs.buckets_)),
                  bucket_size_(rhs.bucket_size_),
                  size_(rhs.size_),
                  mlf_(rhs.mlf_),
                  hash_(rhs.hash_),
                  equal_(rhs.equal_) {
            rhs.bucket_size_ = 0;
            rhs.size_ = 0;
            rhs.mlf_ = 0.0f;
        }

        /*
         * 拷贝赋值函数
         */
        hashtable &operator=(const hashtable &rhs);

        /*
         * 移动赋值函数
         */
        hashtable &operator=(hashtable &&rhs);

        /*
         * 析构函数
         */
        ~hashtable() {
            clear();
        }

    public:
        /*
         * 成员函数
         */

        /*
         * 迭代器相关操作
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            return M_begin();
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            return M_begin();
        }

        /*
         * 获取头部迭代器
         */
        iterator end() noexcept {
            return iterator(nullptr, this);
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator end() const noexcept {
            return M_cit(nullptr);
        }

        /*
         * 返回常量头部迭代器
         */
        const_iterator cbegin() const noexcept {
            return begin();
        }

        /*
         * 返回常量头部迭代器
         */
        const_iterator cend() const noexcept {
            return end();
        }

        /*
         * 容量相关操作
         */

        /*
         * hashtable是否为空
         */
        bool empty() const noexcept {
            return size_ == 0;
        }

        /*
         * 返回hashtable中元素的个数
         */
        size_type size() const noexcept {
            return size_;
        }

        /*
         * hashtable最多存放元素
         */
        size_type max_size() const noexcept {
            return static_cast<size_type>(-1);
        }

        /*
         * 向容器增加元素相关操作
         */

        /*
         * 在hashtable中构建一个新的元素，可以重复
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_multi(Args &&...args);

        /*
         * 在hashtable中构建一个新的元素，不可以重复
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        tinySTL::pair<iterator, bool> emplace_unique(Args &&...args);

        /*
         * [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
         * 来确保 hash_table 的性质，所以选择忽略它
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_multi_use_hint(const_iterator /* hint */, Args &&...args) {
            return emplace_multi(tinySTL::forward<Args>(args)...);
        }

        /*
         * 同上
         */
        template<typename ...Args>
        iterator emplace_unique_use_hint(const_iterator /* hint */, Args &&...args) {
            return emplace_unique(tinySTL::forward<Args>(args)...).first;
        }

        /*
         * 在不需要重建表格的情况下插入新节点，键值允许重复
         */
        iterator insert_multi_noresize(const value_type &value);

        /*
         * 在不需要重建表格的情况下插入新节点，键值不允许重复
         */
        tinySTL::pair<iterator, bool> insert_unique_noresize(const value_type &value);

        /*
         * 插入一个节点，键值允许重复，可能会重建表格
         */
        iterator insert_multi(const value_type &value) {
            rehash_if_need(1);
            return insert_multi_noresize(value);
        }

        /*
         * 插入一个节点，键值允许重复，可能会重建表格 移动语义
         */
        iterator insert_multi(value_type &&value) {
            return emplace_multi(tinySTL::move(value));
        }

        /*
         * 插入一个节点，键值不允许重复，可能会重建表格
         */
        tinySTL::pair<iterator, bool> insert_unique(const value_type &value) {
            rehash_if_need(1);
            return insert_unique_noresize(value);
        }

        /*
         * 插入一个节点，键值不允许重复，可能会重建表格 移动语义
         */
        tinySTL::pair<iterator, bool> insert_unique(value_type &&value) {
            return emplace_unique(tinySTL::move(value));
        }

        /*
         * [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
         * 来确保 hash_table 的性质，所以选择忽略它
         */
        iterator insert_multi_use_hint(const_iterator /* hint */, const value_type &value) {
            return insert_multi(value);
        }

        /*
         * [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
         * 来确保 hash_table 的性质，所以选择忽略它
         * 移动语义
         */
        iterator insert_multi_use_hint(const_iterator /* hint */, value_type &&value) {
            return insert_multi(tinySTL::move(value));
        }

        /*
         * [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
         * 来确保 hash_table 的性质，所以选择忽略它
         */
        iterator insert_unique_use_hint(const_iterator /* hint */, const value_type &value) {
            return insert_unique(value).first;
        }

        /*
         * [note]: hint 对于 hash_table 其实没有意义，因为即使提供了 hint，也要做一次 hash，
         * 来确保 hash_table 的性质，所以选择忽略它
         * 移动语义
         */
        iterator insert_unique_use_hint(const_iterator /* hint */, value_type &&value) {
            return emplace_unique(tinySTL::move(value)).first;
        }

        /*
         * 将迭代器间的数据插入到hashtable中,键值可以重复
         * 类成员模板
         */
        template<typename InputIter>
        void insert_multi(InputIter first, InputIter last) {
            copy_insert_multi(first, last, tinySTL::iterator_category(first));
        }

        /*
         * 将迭代器间的数据插入到hashtable中,键值不可以重复
         * 类成员模板
         */
        template<typename InputIter>
        void insert_unique(InputIter first, InputIter last) {
            copy_insert_unique(first, last, tinySTL::iterator_category(first));
        }

        /*
         * 删除或清空节点
         */

        /*
         * 删除指定位置的节点
         */
        void erase(const_iterator position);

        /*
         * 删除指定区间的节点
         */
        void erase(const_iterator first, const_iterator last);

        /*
         * 删除所有指定的键对应节点
         */
        size_type erase_multi(const key_type &key);

        /*
         * 删除指定键的节点
         */
        size_type erase_unique(const key_type &key);

        /*
         * 清空hashtable所有节点
         */
        void clear();

        /*
         * 交换hashtable对象
         */
        void swap(hashtable &rhs) noexcept;

        /*
         * 查找相关操作
         */

        /*
         * 拆寻指定key有多少个元素
         */
        size_type count(const key_type &key) const;

        /*
         * 查找指定key的位置
         */
        iterator find(const key_type &key);

        /*
         * 查找指定key的位置 const重载
         */
        const_iterator find(const key_type &key) const;

        /*
         * 找到键为key的区间
         */
        tinySTL::pair<iterator, iterator> equal_range_multi(const key_type &key);

        /*
         * 找到键为key的区间 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range_multi(const key_type &key) const;

        /*
         * 找到键为key的区间 不允许重复版本
         */
        tinySTL::pair<iterator, iterator> equal_range_unique(const key_type &key);

        /*
         * 找到键为key的区间 不允许重复版本 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range_unique(const key_type &key) const;

        /*
         * bucket相关接口函数
         */

        /*
         * 获取指定bucket的链表头部迭代器
         */
        local_iterator begin(size_type n) noexcept {
            TINYSTL_DEBUG(n < size_);
            return buckets_[n];
        }

        /*
         * 获取指定bucket的链表头部迭代器 const重载
         */
        const_local_iterator begin(size_type n) const noexcept {
            TINYSTL_DEBUG(n < size_);
            return buckets_[n];
        }

        /*
         * 获取指定bucket的链表尾部迭代器
         */
        local_iterator end(size_type n) noexcept {
            TINYSTL_DEBUG(n < size_);
            return nullptr;
        }

        /*
         * 获取指定bucket的链表尾迭代器 const重载
         */
        const_local_iterator end(size_type n) const noexcept {
            TINYSTL_DEBUG(n < size_);
            return nullptr;
        }

        /*
         * 获取指定bucket的链表常量头部迭代器
         */
        const_local_iterator cbegin(size_type n) const noexcept {
            TINYSTL_DEBUG(n < size_);
            return buckets_[n];
        }

        /*
         * 获取指定bucket的链表常量头部迭代器
         */
        const_local_iterator cend(size_type n) const noexcept {
            TINYSTL_DEBUG(n < size_);
            return nullptr;
        }

        /*
         * 返回 bucket 数量
         */
        size_type bucket_count() const noexcept {
            return bucket_size_;
        }

        /*
         * 返回最大bucket的数量
         */
        size_type max_bucket_count() const noexcept {
            return ht_prime_list[PRIME_NUM - 1];
        }

        /*
         * 在指定bucket上的节点个数
         */
        size_type bucket_size(size_type n) const noexcept;

        /*
         * 根据键key找到对应的bucket
         */
        size_type bucket(const key_type &key) const {
            return hash(key);
        }

        /*
         * hash相关函数
         */

        /*
         * 获取当前的装载比例
         */
        float load_factor() const noexcept {
            return bucket_size_ != 0 ? (float) size_ / bucket_size_ : 0.0f;
        }

        /*
         * 获取最大装载比例
         */
        float max_load_factor() const noexcept {
            return mlf_;
        }

        /*
         * 设置最大装载比例
         */
        void max_load_factor(float ml) {
            THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor");
            mlf_ = ml;
        }

        /*
         * 重新对元素进行一遍哈希，插入到新的位置
         */
        void rehash(size_type count);

        /*
         * 分配能够装下count个元素的bucket空间
         */
        void reserve(size_type count) {
            rehash(static_cast<size_type>((float) count / max_load_factor() + 0.5f));
        }

        /*
         * 返回hash函数对象
         */
        hasher hash_fcn() const {
            return hash_;
        }

        /*
         * 返回键比较函数对象
         */
        key_equal key_eq() const {
            return equal_;
        }

        /*
         * 比较hashtable对象是否相等
         */

        /*
         * 比较hashtable对象是否相等 可以重复
         */
        bool equal_to_multi(const hashtable &other);

        /*
         * 比较hashtable对象是否相等 不可以重复
         */
        bool equal_to_unique(const hashtable &other);

    private:
        /*
         * 私有的辅助函数
         */

        /*
         * 初始化相关函数
         */

        /*
         * 初始化一个buckets大小为n的hashtable
         */
        void init(size_type n);

        /*
         * 根据另一个hashtable的数据初始化此对象
         */
        void copy_init(const hashtable &ht);

        /*
         * 创建或者销毁hashtable节点
         */

        /*
         * 创建hashtable节点
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        node_ptr create_node(Args &&...args);

        /*
         * 摧毁hashtable节点
         */
        void destroy_node(node_ptr node);

        /*
         * hash操作相关函数
         */

        /*
         * 根据n值获取合适的hashtable大小
         */
        size_type next_size(size_type n) const;

        /*
         * hash函数，根据key与n计算最终的位置
         */
        size_type hash(const key_type &key, size_type n) const;

        /*
         * hash函数，根据key与buckets大小计算最终的位置
         */
        size_type hash(const key_type &key) const;

        /*
         * 计算是否需要重新建立hashtable，若需要就重新建立
         */
        void rehash_if_need(size_type n);

        /*
         * 插入节点相关函数
         */

        /*
         * 将迭代器间的数据插入到hashtable 键值允许重复
         * 类成员模板 input_iterator_tag版本
         */
        template<typename InputIter>
        void copy_insert_multi(InputIter first, InputIter last, tinySTL::input_iterator_tag);

        /*
         * 将迭代器间的数据插入到hashtable 键值允许重复
         * 类成员模板 forward_iterator_tag版本
         */
        template<typename ForwardIter>
        void copy_insert_multi(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag);

        /*
         * 将迭代器间的数据插入到hashtable 键值不允许重复
         * 类成员模板 input_iterator_tag版本
         */
        template<typename InputIter>
        void copy_insert_unique(InputIter first, InputIter last, tinySTL::input_iterator_tag);

        /*
         * 将迭代器间的数据插入到hashtable 键值不允许重复
         * 类成员模板 forward_iterator_tag版本
         */
        template<typename ForwardIter>
        void copy_insert_unique(ForwardIter first, ForwardIter last, tinySTL::forward_iterator_tag);

        /*
         * 向hashtable中插入一个节点，不允许重复
         * bool值表示插入是否成功
         */
        tinySTL::pair<iterator, bool> insert_node_unique(node_ptr np);

        /*
         * 向hashtable中插入一个节点，允许重复
         */
        iterator insert_node_multi(node_ptr np);

        /*
         * bucket相关操作
         */

        /*
         * 更换新的bucket
         */
        void replace_bucket(size_type bucket_count);

        /*
         * 将指定的bucket中first->last间的元素珊瑚虫
         */
        void erase_bucket(size_type n, node_ptr first, node_ptr last);

        /*
         * 将指定bucket中头部到last的节点删除
         */
        void erase_bucket(size_type n, node_ptr last);

    };

    // ====================================================================================

    /*
     * 成员函数
     */

    /*
     * 拷贝赋值函数
     */
    template<typename T, typename HashFun, typename KeyEqual>
    hashtable<T, HashFun, KeyEqual> &  // 返回值
    hashtable<T, HashFun, KeyEqual>::operator=(const hashtable &rhs) {
        if (this != &rhs) {
            /* swap使用小技巧，使用局部变量交换，自动释放 */
            hashtable temp(rhs);
            swap(temp);
        }
        return *this;
    }

    /*
     * 移动赋值函数
     */
    template<typename T, typename HashFun, typename KeyEqual>
    hashtable<T, HashFun, KeyEqual> &  // 返回值
    hashtable<T, HashFun, KeyEqual>::operator=(hashtable &&rhs) {
        hashtable temp(tinySTL::move(rhs));
        swap(temp);
        return *this;
    }

    /*
     * 在hashtable中构建一个新的元素，可以重复
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename ...Args>
    typename hashtable<T, HashFun, KeyEqual>::iterator  // 返回值
    hashtable<T, HashFun, KeyEqual>::emplace_multi(Args &&...args) {
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        try {
            /* 查看是否达到了最大装载，若达到了就扩容重新hash一下 */
            if ((float) (size_ + 1) > (float) bucket_size_ * max_load_factor()) {
                rehash(size_ + 1);
            }
        }
        catch (...) {
            destroy_node(np);
            throw;
        }
        return insert_node_multi(np);
    }

    /*
     * 在hashtable中构建一个新的元素，不可以重复
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename ...Args>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::iterator, bool>  // 返回值
    hashtable<T, HashFun, KeyEqual>::emplace_unique(Args &&...args) {
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        try {
            /* 查看是否达到了最大装载，若达到了就扩容重新hash一下 */
            if ((float) (size_ + 1) > (float) bucket_size_ * max_load_factor()) {
                rehash(size_ + 1);
            }
        }
        catch (...) {
            destroy_node(np);
            throw;
        }
        return insert_node_unique(np);
    }

    /*
     * 在不需要重建表格的情况下插入新节点，键值允许重复
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::iterator  // 返回值
    hashtable<T, HashFun, KeyEqual>::insert_multi_noresize(const value_type &value) {
        const size_type n = hash(value_traits::get_key(value));
        node_ptr first = buckets_[n];
        node_ptr temp = create_node(value);
        for (node_ptr cur = first; cur; cur = cur->next) {
            if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(temp->value))) {
                /* 若有相等值，插在相等值后边 */
                temp->next = cur->next;
                cur->next = temp;
                ++size_;
                return iterator(temp, this);
            }
        }
        /* 否则插入在链表头部 */
        temp->next = first;
        buckets_[n] = temp;
        ++size_;
        return iterator(temp, this);
    }

    /*
     * 在不需要重建表格的情况下插入新节点，键值不允许重复
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::iterator, bool>
    hashtable<T, HashFun, KeyEqual>::insert_unique_noresize(const value_type &value) {
        const size_type n = hash(value_traits::get_key(value));
        node_ptr first = buckets_[n];
        for (node_ptr cur = first; cur; cur = cur->next) {
            if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value))) {
                /* 已经存在相同数据，返回false */
                return tinySTL::make_pair(iterator(cur, this), false);
            }
        }
        /* 创建新节点，并成为链表的第一个节点 */
        node_ptr temp = create_node(value);
        temp->next = first;
        buckets_[n] = temp;
        ++size_;
        return tinySTL::make_pair(iterator(temp, this), true);
    }

    /*
     * 删除指定位置的节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::erase(const_iterator position) {
        node_ptr p = position.node;
        if (p) {
            const size_type n = hash(value_traits::get_key(p->value));
            node_ptr cur = buckets_[n];
            if (cur == p) {
                /* 位于链表头部 */
                buckets_[n] = cur->next;
                destroy_node(p);
                --size_;
            } else {
                node_ptr next = cur->next;
                while (next) {
                    if (next == p) {
                        cur->next = next->next;
                        destroy_node(next);
                        --size_;
                        break;
                    } else {
                        cur = next;
                        next = cur->next;
                    }
                }
            }
        }
    }

    /*
     * 删除指定区间的节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::erase(const_iterator first, const_iterator last) {
        if (first.node == last.node) {
            return;
        }
        size_type first_bucket = first.node
                                 ? hash(value_traits::get_key(first.node->value))
                                 : bucket_size_;
        size_type last_bucket = last.node
                                ? hash(value_traits::get_key(last.node->value))
                                : bucket_size_;
        if (first_bucket == last_bucket) {
            /* 如果bucket在同一个位置 */
            erase_bucket(first_bucket, first.node, last.node);
        } else {
            /* 将这个bucket中的节点完全删除 */
            erase_bucket(first_bucket, first.node, nullptr);
            for (size_type n = first_bucket + 1; n < last_bucket; ++n) {
                if (buckets_[n] != nullptr) {
                    erase_bucket(n, nullptr);
                }
            }
            if (last_bucket != bucket_size_) {
                erase_bucket(last_bucket, last.node);
            }
        }
    }

    /*
     * 删除所有指定的键对应节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::erase_multi(const key_type &key) {
        auto p = equal_range_multi(key);
        if (p.first.node != nullptr) {
            erase(p.first, p.second);
            return tinySTL::distance(p.first, p.second);
        }
        return 0;
    }

    /*
     * 删除指定键的节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::erase_unique(const key_type &key) {
        const size_type n = hash(key);
        node_ptr first = buckets_[n];
        if (first) {
            if (is_equal(value_traits::get_key(first->value), key)) {
                /* 头部节点为要删除节点 */
                buckets_[n] = first->next;
                destroy_node(first);
                --size_;
                return 1;
            } else {
                node_ptr next = first->next;
                while (next) {
                    if (is_equal(value_traits::get_key(next->value), key)) {
                        /* 找到节点，删除节点 */
                        first->next = next->next;
                        destroy_node(next);
                        --size_;
                        return 1;
                    }
                    first = next;
                    next = first->next;
                }
            }
        }
        return 0;
    }

    /*
     * 清空hashtable所有节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::clear() {
        if (size_ != 0) {
            for (size_type i = 0; i < bucket_size_; ++i) {
                node_ptr cur = buckets_[i];
                while (cur != nullptr) {
                    node_ptr next = cur->next;
                    destroy_node(cur);
                    cur = next;
                }
                buckets_[i] = nullptr;
            }
            size_ = 0;
        }
    }

    /*
     * 交换hashtable对象
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::swap(hashtable &rhs) noexcept {
        if (this != &rhs) {
            buckets_.swap(rhs.buckets_);
            tinySTL::swap(bucket_size_, rhs.bucket_size_);
            tinySTL::swap(size_, rhs.size_);
            tinySTL::swap(mlf_, rhs.mlf_);
            tinySTL::swap(hash_, rhs.hash_);
            tinySTL::swap(equal_, rhs.equal_);
        }
    }

    /*
     * 拆寻指定key有多少个元素
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type
    hashtable<T, HashFun, KeyEqual>::count(const key_type &key) const {
        const size_type n = hash(key);
        size_type result = 0;
        for (node_ptr cur = buckets_[n]; cur; cur = cur->next) {
            if (is_equal(value_traits::get_key(cur->value), key)) {
                ++result;
            }
        }
        return result;
    }

    /*
     * 查找指定key的位置
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::iterator
    hashtable<T, HashFun, KeyEqual>::find(const key_type &key) {
        const size_type n = hash(key);
        node_ptr first = buckets_[n];
        for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {
            /* 找到第一个相等的位置 */
        }
        return iterator(first, this);
    }

    /*
     * 查找指定key的位置 const重载
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::const_iterator
    hashtable<T, HashFun, KeyEqual>::find(const key_type &key) const {
        const size_type n = hash(key);
        node_ptr first = buckets_[n];
        for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {
            /* 找到第一个相等的位置 */
        }
        return M_cit(first);
    }

    /*
     * 找到键为key的区间
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::iterator,
            typename hashtable<T, HashFun, KeyEqual>::iterator>  // 返回值
    hashtable<T, HashFun, KeyEqual>::equal_range_multi(const key_type &key) {
        const size_type n = hash(key);
        for (node_ptr first = buckets_[n]; first; first = first->next) {
            if (is_equal(value_traits::get_key(first->value), key)) {
                /* 出现等于key的节点,从此节点开始找到所有相等的节点，直到不等 */
                for (node_ptr second = first->next; second; second = second->next) {
                    if (!is_equal(value_traits::get_key(second->value), key)) {
                        /* 找到不相等的位置 */
                        return tinySTL::make_pair(iterator(first, this), iterator(second, this));
                    }
                }
                /* 直到链表最后一直相等，于是找到下一个链表出现的位置作为第二个迭代器的值 */
                for (size_type m = n + 1; m < bucket_size_; ++m) {
                    if (buckets_[m]) {
                        return tinySTL::make_pair(iterator(first, this), iterator(buckets_[m], this));
                    }
                }
                return tinySTL::make_pair(iterator(first, this), end());
            }
        }
        return tinySTL::make_pair(end(), end());
    }

    /*
     * 找到键为key的区间 const重载
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::const_iterator,
            typename hashtable<T, HashFun, KeyEqual>::const_iterator>  // 返回值
    hashtable<T, HashFun, KeyEqual>::equal_range_multi(const key_type &key) const {
        const size_type n = hash(key);
        for (node_ptr first = buckets_[n]; first; first = first->next) {
            if (is_equal(value_traits::get_key(first->value), key)) {
                /* 出现等于key的节点,从此节点开始找到所有相等的节点，直到不等 */
                for (node_ptr second = first->next; second; second = second->next) {
                    if (!is_equal(value_traits::get_key(second->value), key)) {
                        /* 找到不相等的位置 */
                        return tinySTL::make_pair(M_cit(first), M_cit(second));
                    }
                }
                /* 直到链表最后一直相等，于是找到下一个链表出现的位置作为第二个迭代器的值 */
                for (size_type m = n + 1; m < bucket_size_; ++m) {
                    if (buckets_[m]) {
                        return tinySTL::make_pair(M_cit(first), M_cit(buckets_[m]));
                    }
                }
                return tinySTL::make_pair(M_cit(first), cend());
            }
        }
        return tinySTL::make_pair(cend(), cend());
    }

    /*
     * 找到键为key的区间 不允许重复版本
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::iterator,
            typename hashtable<T, HashFun, KeyEqual>::iterator>  // 返回值
    hashtable<T, HashFun, KeyEqual>::equal_range_unique(const key_type &key) {
        const size_type n = hash(key);
        for (node_ptr first = buckets_[n]; first; first = first->next) {
            if (is_equal(value_traits::get_key(first->value), key)) {
                if (first->next) {
                    /* 若下一个节点不为空 */
                    return tinySTL::make_pair(iterator(first, this), iterator(first->next, this));
                }
                /* 直到链表最后一直相等，于是找到下一个链表出现的位置作为第二个迭代器的值 */
                for (size_type m = n + 1; m < bucket_size_; ++m) {
                    if (buckets_[m]) {
                        return tinySTL::make_pair(iterator(first, this), iterator(buckets_[m], this));
                    }
                }
                return tinySTL::make_pair(iterator(first, this), end());
            }
        }
        return tinySTL::make_pair(end(), end());
    }

    /*
     * 找到键为key的区间 不允许重复版本 const重载
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::const_iterator,
            typename hashtable<T, HashFun, KeyEqual>::const_iterator>  // 返回值
    hashtable<T, HashFun, KeyEqual>::equal_range_unique(const key_type &key) const {
        const size_type n = hash(key);
        for (node_ptr first = buckets_[n]; first; first = first->next) {
            if (is_equal(value_traits::get_key(first->value), key)) {
                if (first->next) {
                    /* 若下一个节点不为空 */
                    return tinySTL::make_pair(M_cit(first), M_cit(first->next));
                }
                /* 直到链表最后一直相等，于是找到下一个链表出现的位置作为第二个迭代器的值 */
                for (size_type m = n + 1; m < bucket_size_; ++m) {
                    if (buckets_[m]) {
                        return tinySTL::make_pair(M_cit(first), M_cit(buckets_[m]));
                    }
                }
                return tinySTL::make_pair(M_cit(first), cend());
            }
        }
        return tinySTL::make_pair(cend(), cend());
    }

    /*
     * 在直嘀咕bucket上的节点个数
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::bucket_size(size_type n) const noexcept {
        size_type result = 0;
        for (node_ptr cur = buckets_[n]; cur; cur = cur->next) {
            ++result;
        }
        return result;
    }

    /*
     * 重新对元素进行一遍哈希，插入到新的位置
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::rehash(size_type count) {
        size_type n = next_size(count);
        if (n > bucket_size_) {
            /* n大于bucket_size_当然要重新hash */
            replace_bucket(n);
        } else {
            if ((float) size_ / (float) n < max_load_factor() - 0.25f &&
                (float) n < (float) bucket_size_ * 0.75) {
                /* 满足以上条件也需要hash */
                replace_bucket(n);
            }
        }
    }

    /*
    * 比较hashtable对象是否相等 可以重复
    */
    template<typename T, typename HashFun, typename KeyEqual>
    bool hashtable<T, HashFun, KeyEqual>::equal_to_multi(const hashtable &other) {
        if (size_ != other.size_) {
            return false;
        }
        for (node_ptr f = begin(), l = end(); f != l;) {
            auto p1 = equal_range_multi(value_traits::get_key(*f));
            auto p2 = other.equal_range_multi(value_traits::get_key(*f));
            if (tinySTL::distance(p1.first, p1.second) != tinySTL::distance(p2.first, p2.second)
                || tinySTL::is_permutation(p1.first, p1.second, p2.first, p2.second)) {
                return false;
            }
            f = p1.second;
        }
        return true;
    }

    /*
     * 比较hashtable对象是否相等 不可以重复
     */
    template<typename T, typename HashFun, typename KeyEqual>
    bool hashtable<T, HashFun, KeyEqual>::equal_to_unique(const hashtable &other) {
        if (size_ != other.size_) {
            return false;
        }
        for (node_ptr f = begin(), l = end(); f != l; ++f) {
            iterator res = other.find(value_traits::get_key(*f));
            if (res.node == nullptr || *res != *f) {
                return false;
            }
        }
        return true;
    }

    // =====================================================================================

    /*
     * 私有的辅助函数
     */

    /*
     * 初始化一个buckets大小为n的hashtable
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::init(size_type n) {
        const size_type bucket_nums = next_size(n);
        try {
            buckets_.reserve(bucket_nums);
            buckets_.assign(bucket_nums, nullptr);
        }
        catch (...) {
            bucket_size_ = 0;
            size_ = 0;
            throw;
        }
        bucket_size_ = buckets_.size();
    }

    /*
     * 根据另一个hashtable的数据初始化此对象
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::copy_init(const hashtable &ht) {
        bucket_size_ = 0;
        buckets_.reserve(ht.bucket_size_);
        buckets_.assign(ht.bucket_size_, nullptr);
        try {
            for (size_type i = 0; i < ht.bucket_size_; ++i) {
                node_ptr cur = ht.buckets_[i];
                if (cur) {
                    /* cur不为空，表示当前节点存在链表 */
                    node_ptr copy = create_node(cur->value);
                    /* 将第一个节点的指针赋值给对应的hashtable节点 */
                    buckets_[i] = copy;
                    /* 再复制这一个节点对应的链表 */
                    for (node_ptr node = cur->next; node; node = node->next) {
                        copy->next = create_node(node->value);
                        copy = copy->next;
                    }
                    copy->next = nullptr;
                }
            }
            bucket_size_ = ht.bucket_size_;
            mlf_ = ht.mlf_;
            size_ = ht.size_;
        }
        catch (...) {
            clear();
        }
    }

    /*
     * 创建hashtable节点
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename ...Args>
    typename hashtable<T, HashFun, KeyEqual>::node_ptr  // 返回值
    hashtable<T, HashFun, KeyEqual>::create_node(Args &&...args) {
        node_ptr temp = node_allocator::allocate(1);
        try {
            data_allocator::construct(tinySTL::address_of(temp->value), tinySTL::forward<Args>(args)...);
            temp->next = nullptr;
        }
        catch (...) {
            node_allocator::deallocate(temp);
            throw;
        }
        return temp;
    }

    /*
     * 摧毁hashtable节点
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::destroy_node(node_ptr node) {
        data_allocator::destroy(tinySTL::address_of(node->value));
        node_allocator::deallocate(node);
    }

    /*
     * 根据n值获取合适的hashtable大小
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::next_size(size_type n) const {
        return ht_next_prime(n);
    }

    /*
     * hash函数，根据key与n计算最终的位置
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::hash(const key_type &key, size_type n) const {
        return hash_(key) % n;
    }

    /*
     * hash函数，根据key与buckets大小计算最终的位置
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::size_type  // 返回值
    hashtable<T, HashFun, KeyEqual>::hash(const key_type &key) const {
        return hash_(key) % bucket_size_;
    }

    /*
     * 计算是否需要重新建立hashtable，若需要就重新建立
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::rehash_if_need(size_type n) {
        if (static_cast<float>(size_ + n) > (float) bucket_size_ * max_load_factor()) {
            rehash(size_ + n);
        }
    }

    /*
     * 将迭代器间的数据插入到hashtable 键值允许重复
     * 类成员模板 input_iterator_tag版本
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename InputIter>
    void
    hashtable<T, HashFun, KeyEqual>::copy_insert_multi(InputIter first, InputIter last, tinySTL::input_iterator_tag) {
        rehash_if_need(tinySTL::distance(first, last));
        for (; first != last; ++first) {
            insert_multi_noresize(*first);
        }
    }

    /*
     * 将迭代器间的数据插入到hashtable 键值允许重复
     * 类成员模板 forward_iterator_tag版本
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename ForwardIter>
    void hashtable<T, HashFun, KeyEqual>::copy_insert_multi(ForwardIter first, ForwardIter last,
                                                            tinySTL::forward_iterator_tag) {
        size_type n = tinySTL::distance(first, last);
        rehash_if_need(n);
        for (; n > 0; --n, ++first) {
            insert_multi_noresize(*first);
        }
    }

    /*
     * 将迭代器间的数据插入到hashtable 键值不允许重复
     * 类成员模板 input_iterator_tag版本
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename InputIter>
    void
    hashtable<T, HashFun, KeyEqual>::copy_insert_unique(InputIter first, InputIter last, tinySTL::input_iterator_tag) {
        rehash_if_need(tinySTL::distance(first, last));
        for (; first != last; ++first) {
            insert_unique_noresize(*first);
        }
    }

    /*
     * 将迭代器间的数据插入到hashtable 键值不允许重复
     * 类成员模板 forward_iterator_tag版本
     */
    template<typename T, typename HashFun, typename KeyEqual>
    template<typename ForwardIter>
    void hashtable<T, HashFun, KeyEqual>::copy_insert_unique(ForwardIter first, ForwardIter last,
                                                             tinySTL::forward_iterator_tag) {
        size_type n = tinySTL::distance(first, last);
        rehash_if_need(n);
        for (; n > 0; --n, ++first) {
            insert_unique_noresize(*first);
        }
    }


    /*
     * 向hashtable中插入一个节点，不允许重复
     * bool值表示插入是否成功
     */
    template<typename T, typename HashFun, typename KeyEqual>
    tinySTL::pair<typename hashtable<T, HashFun, KeyEqual>::iterator, bool>
    hashtable<T, HashFun, KeyEqual>::insert_node_unique(node_ptr np) {
        const size_type n = hash(value_traits::get_key(np->value));
        node_ptr cur = buckets_[n];
        if (cur == nullptr) {
            /* 此节点还没有数据 */
            buckets_[n] = np;
            ++size_;
            return tinySTL::make_pair(iterator(np, this), true);
        }
        for (; cur; cur = cur->next) {
            if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) {
                /* 存在相等的值 */
                return tinySTL::make_pair(iterator(cur, this), false);
            }
        }
        /* 此节点没有相同的值，直接插入到头部 */
        np->next = buckets_[n];
        buckets_[n] = np;
        ++size_;
        return tinySTL::make_pair(iterator(np, this), true);
    }

    /*
     * 向hashtable中插入一个节点，允许重复
     */
    template<typename T, typename HashFun, typename KeyEqual>
    typename hashtable<T, HashFun, KeyEqual>::iterator  // 返回值
    hashtable<T, HashFun, KeyEqual>::insert_node_multi(node_ptr np) {
        const size_type n = hash(value_traits::get_key(np->value));
        node_ptr cur = buckets_[n];
        if (cur == nullptr) {
            /* 此节点还没有数据 */
            buckets_[n] = np;
            ++size_;
            return iterator(np, this);
        }
        for (; cur; cur = cur->next) {
            if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) {
                /* 存在相等的值,将其插入到该值后 */
                np->next = cur->next;
                cur->next = np;
                ++size_;
                return iterator(np, this);
            }
        }
        /* 此节点没有相同的值，直接插入到头部 */
        np->next = buckets_[n];
        buckets_[n] = np;
        ++size_;
        return iterator(np, this);
    }

    /*
     * 更换新的bucket
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::replace_bucket(size_type bucket_count) {
        /* 首先根据大小初始化一个新的bucket */
        bucket_type bucket(bucket_count);
        /* 开始迁移，迁移后的节点就算还是在同一个hashtable位置上，但相对位置还是有可能改变 */
        if (size_ != 0) {
            for (size_type i = 0; i < bucket_size_; ++i) {
                for (node_ptr first = buckets_[i]; first; first = first->next) {
                    node_ptr temp = create_node(first->value);
                    /* 计算新的位置 */
                    const size_type n = hash(value_traits::get_key(first->value), bucket_count);
                    node_ptr f = bucket[n];
                    bool is_inserted = false;
                    for (node_ptr cur = f; cur; cur = cur->next) {
                        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value))) {
                            /* 找到相等的节点就插入在相等节点的后边 */
                            temp->next = cur->next;
                            cur->next = temp;
                            is_inserted = true;
                            break;
                        }
                    }
                    if (!is_inserted) {
                        /* 第一次插入此节点时会进入此分支，没有与当前节点相等的节点时也会进入此分支 */
                        temp->next = f;
                        bucket[n] = temp;
                    }
                }
            }
        }
        /* 和一个局部变量交换，自动释放原内存 */
        buckets_.swap(bucket);
        bucket_size_ = buckets_.size();
    }

    /*
     * 将指定的bucket中first->last间的元素珊瑚虫
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::erase_bucket(size_type n, node_ptr first, node_ptr last) {
        node_ptr cur = buckets_[n];
        if (cur == first) {
            erase_bucket(n, last);
        } else {
            node_ptr next = cur->next;
            for (; next != first; cur = next, next = cur->next) {
                /* 移动next到first位置 */
            }
            /* cur在需要删除节点的前一个节点 */
            while (next != last) {
                cur->next = next->next;
                destroy_node(next);
                next = cur->next;
                --size_;
            }
        }
    }

    /*
     * 将指定bucket中头部到last的节点删除
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void hashtable<T, HashFun, KeyEqual>::erase_bucket(size_type n, node_ptr last) {
        node_ptr cur = buckets_[n];
        while (cur != last) {
            node_ptr next = cur->next;
            destroy_node(cur);
            cur = next;
            --size_;
        }
        buckets_[n] = last;
    }

    // ========================================================================================

    /*
     * 全局区域重载操作符,hashtable没有重载操作符
     */

    /*
     * 重载tinySTL的swap
     */
    template<typename T, typename HashFun, typename KeyEqual>
    void swap(hashtable<T, HashFun, KeyEqual> &lhs, hashtable<T, HashFun, KeyEqual> &rhs) noexcept {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_HASHTABLE_H
