//
// Created by cqupt1811 on 2022/5/5.
//

/*
 * 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数
 */

#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

#include <cstddef>  /* 头文件cstddef与其C对应版本兼容，它是C头文件<stddef.h>较新版本，定义了常用的常量、宏、类型和函数 */
#include "type_traits.h"  /* type_traits.h文件钟引入了标准库钟的type_traits文件，同时定义了一些自己的模板方法 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    // =========================================================================================

    /*
     * 首先定义五种迭代器类型
     */
    struct input_iterator_tag {
        // 只能输入
    };

    struct output_iterator_tag {
        // 只能输出
    };

    struct forward_iterator_tag : public input_iterator_tag {
        // 继承自input_iterator,并且可以单向移动
    };

    struct bidirectional_iterator_tag : public forward_iterator_tag {
        // 继承自forward_iterator，并且可以双向移动
    };

    struct random_access_iterator_tag : public bidirectional_iterator_tag {
        // 继承自bidirectional_iterator，并且具有随机读写的能力
    };


    /*
     * 定义最基础的 iterator 模板类型
     */
    template<typename Category, typename T, typename Distance = ptrdiff_t,
            typename Pointer = T *, typename Reference=T &>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Distance difference_type;
    };


    // ==========================================================================================
    /*
     * 以下代码通过模板的偏特化实现 iterator 的萃取机制
     */

    /*
     * 检查类型 T 是否含有 iterator_category
     * 如果有 iterator_category，则 value 为true，否则为 false
     */
    template<typename T>
    struct has_iterator_cat {
    private:
        struct two {
            char a;
            char b;
        };

        template<class U>
        static two test(...);

        template<class U>
        static char test(typename U::iterator_category * = 0);

    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    /*
     * iterator traits 模板实现，在其中定义 iterator 的五种必要类型
     * 此处为空实现，利用偏特化 特化一个版本给的哥泛型参数为 true 的实现
     */
    template<typename Iterator, bool>
    struct iterator_traits_impl {

    };

    /*
     * 偏特化版本的 iterator traits ，第二个参数为true
     * 在其中定义五种类型
     */
    template<typename Iterator>
    struct iterator_traits_impl<Iterator, true> {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::difference_type difference_type;
    };

    /*
     * 利用偏特化实现的辅助模板
     */
    template<typename Iterator, bool>
    struct iterator_traits_helper {

    };

    /*
     * 利用偏特化实现的辅助模板,偏特化版本
     * 第二个参数为true进入此分支
     * 该分支继承了 iterator_traits_impl，可以获得定义的五个类型
     */
    template<typename Iterator>
    struct iterator_traits_helper<Iterator, true>
            : public iterator_traits_impl<Iterator,
                    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
                    // 是否可以由Iterator::iterator_category类型隐式转换到input_iterator_tag类型
                    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {

    };

    /*
     * 定义 迭代器萃取器 模板
     * 该类型针对自己定义的 iterator 对象
     */
    template<typename Iterator>
    struct iterator_traits
            : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {

    };

    /*
     * 针对原始指针定义的萃取器，偏特化
     */
    template<typename T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag iterator_category;  // 原始指针是可以随机访问的，所以是属于随机访问类的
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef ptrdiff_t difference_type;
    };

    /*
     * 针对常量原始指针的偏特化
     */
    template<typename T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag iterator_category;  // 原始指针是可以随机访问的，所以是属于随机访问类的
        typedef T value_type;  // 值类型不需要加const
        typedef const T *pointer;
        typedef const T &reference;
        typedef ptrdiff_t difference_type;
    };

    // ==============================================================================================

    /*
     * iterator相关的一系列辅助函数
     * 检查类型 T 的iterator_category类型是否能隐式转换为迭代器类型 U
     */
    template<typename T, typename U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
            : public tinySTL::m_bool_constant<std::is_convertible<
                    typename iterator_traits<T>::iterator_category, U>::value> {

    };

    /*
     * 上面模板的偏特化版本，第三个参数为false时进入此模板
     */
    template<typename T, typename U>
    struct has_iterator_cat_of<T, U, false> : public tinySTL::m_false_type {

    };

    /*
     * 判断迭代器是否属于 input_iterator_tag 类型
     */
    template<typename Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {

    };

    /*
     * 判断迭代器是否属于 is_output_iterator 类型
     */
    template<typename Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {

    };

    /*
     * 判断迭代器是否属于 forward_iterator_tag 类型
     */
    template<typename Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {

    };

    /*
     * 判断迭代器是否属于 bidirectional_iterator_tag 类型
     */
    template<typename Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {

    };

    /*
     * 判断迭代器是否属于 random_access_iterator_tag 类型
     */
    template<typename Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {

    };

    /*
     * 判断类型Iterator是否是迭代器，因为迭代器要么能向上转型到input_iterator，要么就是output_iterator
     * 所以只需要判断是否属于这两类就可以确定该类是否是迭代器了
     */
    template<typename Iterator>
    struct is_iterator :
            public tinySTL::m_bool_constant<is_input_iterator<Iterator>::value ||
                                            is_output_iterator<Iterator>::value> {

    };

    /*
     * 获取迭代器对应的 category
     */
    template<typename Iterator>
    typename iterator_traits<Iterator>::iterator_category  // 返回值为iterator_category类型
    iterator_category(const Iterator &) {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    /*
     * 获取迭代器对应的 difference_type
     * 注意：是 difference_type *，指针类型
     */
    template<typename Iterator>
    typename iterator_traits<Iterator>::difference_type *  // 返回值为iterator_category指针类型
    distance_type(const Iterator &) {
        /* 注意：这里返回的是对应类型的指针类型，而不是对应类型 */
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    /*
     * 获取迭代器对应的 value_type
     * 注意：是 value_type *，指针类型
     */
    template<typename Iterator>
    typename iterator_traits<Iterator>::value_type *  // 返回值为value_type指针类型
    value_type(const Iterator &) {
        /* 注意：这里返回的是对应类型的指针类型，而不是对应类型 */
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

    /*
     * 下列函数用于计算迭代器之间的距离
     */

    /*
     * 计算距离的 input_iterator_tag 版本
     */
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type  // 返回值
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {  // 第三个参数表明接受类型
        typename iterator_traits<InputIterator>::difference_type n = 0;
        /* 因为只能单向访问，于是采用这种方式计算距离 */
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    /*
     * 计算距离的 random_access_iterator_tag 版本
     */
    template<typename RandomIter>
    typename iterator_traits<RandomIter>::difference_type  // 返回值
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        return last - first;
    }

    /*
     * 在此函数中根据迭代器类型分发任务，调用对应的函数
     */
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type  // 返回值
    distance(InputIterator first, InputIterator last) {
        /* 第三个参数中提取对应类型，可以指定分发到对应函数 */
        return distance_dispatch(first, last, iterator_category(first));
    }

    /*
     * 下列函数用于让迭代器前进n个距离
     */

    /*
     * 前进n个距离的 input_iterator_tag 版本
     */
    template<typename InputIterator, typename Distance>
    void  // 返回值为 void
    advance_dispatch(InputIterator &i, Distance n, input_iterator_tag) {
        /* 因为只能单向访问，于是采用这种方式计算前进 */
        while (n--) {
            ++i;
        }
    }

    /*
     * 前进n个距离的 bidirectional_iterator_tag 版本
     */
    template<typename BidirectionalIterator, typename Distance>
    void  // 返回值
    advance_dispatch(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
        /* 因为可以双向访问，所以根据n的取值决定前进方向 */
        if (n >= 0) {
            while (n--) {
                ++i;
            }
        } else {
            while (n++) {
                --i;
            }
        }
    }

    /*
     * 前进n个距离的 random_acess_iterator_tag 版本
     */
    template<typename RandomIter, typename Distance>
    void  // 返回值
    advance_dispatch(RandomIter &i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    /*
     * 在此函数中根据迭代器类型分发任务，调用对应的函数
     */
    template<typename InputIterator, typename Distance>
    void  // 返回值
    advance(InputIterator &i, Distance n) {
        /* 第三个参数中提取对应类型，可以指定分发到对应函数 */
        advance_dispatch(i, n, iterator_category(i));
    }

    // reverse_iterator=============================================================================================

    /*
     * 反向迭代器的模板类 reverse_iterator ，将原有迭代器的操作调转
     * 代表反向迭代器，使前进为后退，后退为前进
     * 拥有一个模板参数 Iterator，为传入的迭代器的类型
     */
    template<typename Iterator>
    class reverse_iterator {
    private:
        Iterator current;  // 包装的正向迭代器

    public:
        /* 反向迭代器同样需要指定以下五种类型 */
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::pointer pointer;
        typedef typename iterator_traits<Iterator>::reference reference;

        /* 在类中使用的别名 */
        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        /*
         * 默认构造函数
         */
        reverse_iterator() {

        }

        /*
         * 有参构造函数，接受一个正向迭代器
         * 必须显式调用
         */
        explicit reverse_iterator(iterator_type i) : current(i) {

        }

        /*
         * 拷贝构造函数
         */
        reverse_iterator(const self &rhs) : current(rhs.current) {

        }

    public:
        /*
         * 返回对应的正向迭代器
         */
        iterator_type base() const {
            return current;
        }

        /*
         * 重载 * 运算符，使得该类模拟指针的操作
         * 实际对应正向迭代器的前一个位置,所以取值前需要做自减操作
         */
        reference operator*() const {
            auto temp = current;
            return *--temp;
        }

        /*
         * 重载 -> 运算符，使得该类模拟指针的操作
         */
        pointer operator->() const {
            /* 返回当前元素的对应指针，调用operator*()获取当前元素 */
            return &(operator*());
        }

        /*
         * 重载前 ++ 运算符，使得该类模拟指针的操作
         * 反向迭代器的 ++ 操作等于正向迭代器的 -- 操作
         */
        self &operator++() {
            --current;
            return *this;
        }

        /*
         * 重载后 ++ 运算符，使得该类模拟指针的操作
         * 反向迭代器的 ++ 操作等于正向迭代器的 -- 操作
         */
        self operator++(int) {
            /* 保存一个副本，以备返回 */
            self temp = *this;
            --current;
            return temp;
        }

        /*
         * 重载前 -- 运算符，使得该类模拟指针的操作
         * 反向迭代器的 -- 操作等于正向迭代器的 ++ 操作
         */
        self &operator--() {
            ++current;
            return *this;
        }

        /*
         * 重载后 -- 运算符，使得该类模拟指针的操作
         * 反向迭代器的 -- 操作等于正向迭代器的 ++ 操作
         */
        self operator--(int) {
            /* 保存一个副本，以备返回 */
            self temp = *this;
            ++current;
            return temp;
        }

        /*
         * 重载 += 运算符，使得该类模拟指针的操作
         * 反向迭代器的 += 操作等于正向迭代器的 -= 操作
         */
        self &operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        /*
         * 重载 -= 运算符，使得该类模拟指针的操作
         * 反向迭代器的 -= 操作等于正向迭代器的 += 操作
         */
        self &operator-=(difference_type n) {
            current += n;
            return *this;
        }

        /*
         * 重载 + 运算符，使得该类模拟指针的操作
         * 反向迭代器的 + 操作等于正向迭代器的 - 操作
         */
        self operator+(difference_type n) const {
            return self(current - n);
        }

        /*
         * 重载 - 运算符，使得该类模拟指针的操作
         * 反向迭代器的 - 操作等于正向迭代器的 + 操作
         */
        self operator-(difference_type n) const {
            return self(current + n);
        }

        /*
         * 重载 [] 运算符，使得该类模拟指针的操作
         */
        reference operator[](difference_type n) const {
            return *(*this + n);
        }
    };

    // ============================================================================================

    /*
     * 重载reverse iterator的全局操作符
     * 每一个操作符只是对反向迭代器中的正向迭代器操作符的包装
     */

    /*
     * 重载减法操作符
     */
    template<class Iterator>
    typename reverse_iterator<Iterator>::difference_type  // 返回值
    operator-(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用正向迭代器的减法即可 */
        return rhs.base() - lhs.base();
    }

    /*
     * 重载等于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator==(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用正向迭代器的等于比较操作符即可 */
        return lhs.base() == rhs.base();
    }

    /*
     * 重载不等于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator!=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用等于比较操作符即可 */
        return !(lhs == rhs);
    }

    /*
     * 重载小于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator<(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用正向迭代器的等于比较操作符即可 */
        return lhs.base() < rhs.base();
    }

    /*
     * 重载大于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator>(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用小于比较操作符即可 */
        return rhs < lhs;
    }

    /*
     * 重载小于等于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator<=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用小于比较操作符即可 */
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于比较操作符
     */
    template<typename Iterator>
    bool  // 返回值
    operator>=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs) {
        /* 调用小于比较操作符即可 */
        return !(lhs < rhs);
    }

}  // namespace tinySTL


#endif //TINYSTL_ITERATOR_H
