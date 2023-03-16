//
// Created by cqupt1811 on 2022/5/17.
//

/*
 * 这个头文件包含一个模板类 rb_tree
 * rb_tree : 红黑树
 */

#ifndef TINYSTL_RB_TREE_H
#define TINYSTL_RB_TREE_H

#include <initializer_list>  /* std::initializer_list<T> 类型对象是一个访问 const T 类型对象数组的轻量代理对象 */
#include <cassert>  /* assert函数 */
#include "functional.h"  /* 这个头文件包含了 tinySTL 的函数对象与哈希函数 */
#include "iterator.h"  /* 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数 */
#include "memory.h"  /* 这个头文件负责更高级的动态内存管理,包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr */
#include "exceptdef.h"  /* 此文件中定义了异常相关的宏 */

/* 首先定义自己的命名空间 */
namespace tinySTL {

    /*
     * rb_tree节点颜色的类型
     */
    typedef bool rb_tree_color_type;

    static constexpr rb_tree_color_type rb_tree_red = false;
    static constexpr rb_tree_color_type rb_tree_black = true;

    /*
     * 红黑树节点前向声明
     */
    template<typename T>
    struct rb_tree_node_base;
    template<typename T>
    struct rb_tree_node;

    /*
     * 迭代器前向声明
     */
    template<typename T>
    struct rb_tree_iterator;
    template<typename T>
    struct rb_tree_const_iterator;

    // =============================================================================

    /*
     * 用以萃取红黑树值的萃取机,set会进入此萃取机
     */
    template<typename T, bool>
    struct rb_tree_value_traits_imp {
        typedef T key_type;
        typedef T mapped_type;
        typedef T value_type;

        /*
         * 获取key_type类型的值
         * 类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value;
        }

        /*
         * 获取value_type类型的值
         * 类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value;
        }
    };

    /*
     * 模板偏特化，针对第二模板参数为true的类型,map会进入此萃取机
     */
    template<typename T>
    struct rb_tree_value_traits_imp<T, true> {
        /* remove_cv去除类型带有的const与volatile属性 */
        typedef typename std::remove_cv<typename T::first_type>::type key_type;
        typedef typename T::second_type mapped_type;
        typedef T value_type;

        /*
         * 获取key值
         * 类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value.first;
        }

        /*
         * 获取value值
         * 类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value;
        }
    };

    /*
     * 红黑树值萃取机
     * 根据节点类型是set的还是map的可以萃取出对应的信息
     */
    template<typename T>
    struct rb_tree_value_traits {
        /* 判断是否为map类型，因为map的元素为pair */
        static constexpr bool is_map = tinySTL::is_pair<T>::value;

        /*
         * 类型别名定义
         */

        /* 根据是否是map采用不同的实现 */
        typedef rb_tree_value_traits_imp<T, is_map> value_traits_type;
        typedef typename value_traits_type::key_type key_type;
        typedef typename value_traits_type::mapped_type mapped_type;
        typedef typename value_traits_type::value_type value_type;

        /*
         * 获取key值
         * 类成员模板
         */
        template<typename Ty>
        static const key_type &get_key(const Ty &value) {
            return value_traits_type::get_key(value);
        }

        /*
         * 获取value值
         * 类成员模板
         */
        template<typename Ty>
        static const value_type &get_value(const Ty &value) {
            return value_traits_type::get_value(value);
        }
    };

    // =========================================================================

    /*
     * 红黑树节点萃取机
     */
    template<typename T>
    struct rb_tree_node_traits {
        /*
         * 别名定义
         */
        typedef rb_tree_color_type color_type;
        typedef rb_tree_value_traits<T> value_traits;
        typedef typename value_traits::key_type key_type;
        typedef typename value_traits::mapped_type mapped_type;
        typedef typename value_traits::value_type value_type;

        /*
         * 定义节点类型
         */
        typedef rb_tree_node_base<T> base_type;
        typedef rb_tree_node<T> node_type;

        /*
         * 定义节点指针
         */
        typedef base_type *base_ptr;
        typedef node_type *node_ptr;
    };

    // ==============================================================================

    /*
     * 红黑树的节点设计
     */
    template<typename T>
    struct rb_tree_node_base {
        /*
         * 别名定义
         */
        typedef typename rb_tree_node_traits<T>::color_type color_type;
        typedef typename rb_tree_node_traits<T>::base_ptr base_ptr;
        typedef typename rb_tree_node_traits<T>::node_ptr node_ptr;

        /*
         * 定义类变量
         * parent: 父节点
         * left: 左子节点
         * right: 右子节点
         * color: 节点颜色
         */
        base_ptr parent;
        base_ptr left;
        base_ptr right;
        color_type color;

        /*
         * 获取当前节点的base_ptr类型指针
         */
        base_ptr get_base_ptr() {
            return &*this;
        }

        /*
         * 获取当前节点的node_ptr类型指针
         */
        node_ptr get_node_ptr() {
            return static_cast<node_ptr>(&*this);
        }
    };

    /*
     * 红黑树的节点设计
     */
    template<typename T>
    struct rb_tree_node : public rb_tree_node_base<T> {
        /*
         * 定义类型别名
         */
        typedef typename rb_tree_node_traits<T>::base_ptr base_ptr;
        typedef typename rb_tree_node_traits<T>::node_ptr node_ptr;

        /*
         * 节点值变量
         */
        T value;

        /*
         * 获取当前节点的base_ptr类型指针
         */
        base_ptr get_base_ptr() {
            return static_cast<base_ptr>(&*this);
        }

        /*
         * 获取当前节点的node_ptr类型指针
         */
        node_ptr get_node_ptr() {
            return &*this;
        }
    };

    // ==========================================================================================

    /*
     * 红黑树萃取机
     */
    template<typename T>
    struct rb_tree_traits {
        /*
         * 类型别名定义
         */
        typedef typename rb_tree_node_traits<T>::value_traits value_traits;
        typedef typename rb_tree_node_traits<T>::key_type key_type;
        typedef typename rb_tree_node_traits<T>::mapped_type mapped_type;
        typedef typename rb_tree_node_traits<T>::value_type value_type;
        typedef typename rb_tree_node_traits<T>::base_type base_type;
        typedef typename rb_tree_node_traits<T>::node_type node_type;
        typedef typename rb_tree_node_traits<T>::base_ptr base_ptr;
        typedef typename rb_tree_node_traits<T>::node_ptr node_ptr;

        typedef value_type *pointer;
        typedef value_type &reference;
        typedef const value_type *const_pointer;
        typedef const value_type &const_reference;
    };

    // ======================================================================================

    /*
     * 红黑树相关算法
     */

    /*
     * 找到以x为根节点的最小值
     */
    template<typename NodePtr>
    NodePtr rb_tree_min(NodePtr x) noexcept {
        while (x->left != nullptr) {
            x = x->left;
        }
        return x;
    }

    /*
     * 找到以x为根节点的最大值
     */
    template<typename NodePtr>
    NodePtr rb_tree_max(NodePtr x) noexcept {
        while (x->right != nullptr) {
            x = x->right;
        }
        return x;
    }

    /*
     * 判断当前节点是否为左子节点
     */
    template<typename NodePtr>
    bool rb_tree_is_lchild(NodePtr node) noexcept {
        return node == node->parent->left;
    }

    /*
     * 判断当前节点颜色是否为红色
     */
    template<typename NodePtr>
    bool rb_tree_is_red(NodePtr node) noexcept {
        return node->color == rb_tree_red;
    }

    /*
     * 将当前节点颜色设置为黑色
     */
    template<typename NodePtr>
    void rb_tree_set_black(NodePtr &node) noexcept {
        node->color = rb_tree_black;
    }

    /*
     * 将当前节点颜色设置为红色
     */
    template<typename NodePtr>
    void rb_tree_set_red(NodePtr &node) noexcept {
        node->color = rb_tree_red;
    }

    /*
     * 获取当前节点的下一个节点
     */
    template<typename NodePtr>
    NodePtr rb_tree_next(NodePtr node) noexcept {
        if (node->right != nullptr) {
            /* 如果当前节点的右节点不为空，那么下一个节点为右节点的最小节点 */
            return rb_tree_min(node->right);
        }
        while (!rb_tree_is_lchild(node)) {
            /* 一直向上，直到当前节点为左节点，此时下一个节点为当前节点的父节点 */
            node = node->parent;
        }
        return node->parent;
    }

    /*
     * 旋转左子树操作
     * 左旋，参数一为左旋点，参数二为根节点
     * 旋转过程如下所示
     * /-----------------------------------------\
     * |       p                         p       |
     * |      / \                       / \      |
     * |     x   d    rotate left      y   d     |
     * |    / \       ===========>    / \        |
     * |   a   y                     x   c       |
     * |      / \                   / \          |
     * |     b   c                 a   b         |
     * \-----------------------------------------/
     */
    template<class NodePtr>
    void rb_tree_rotate_left(NodePtr x, NodePtr &root) noexcept {
        /* y 为 x 的右子节点 */
        NodePtr y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;

        if (x == root) {
            /* 如果 x 为根节点，让 y 顶替 x 成为根节点 */
            root = y;
        } else if (rb_tree_is_lchild(x)) {
            /* 如果 x 是左子节点 */
            x->parent->left = y;
        } else {
            /* 如果 x 是右子节点 */
            x->parent->right = y;
        }
        /* 调整 x 与 y 的关系 */
        y->left = x;
        x->parent = y;
    }

    /*
     * 旋转右子树操作
     * 右旋，参数一为右旋点，参数二为根节点
     * 旋转过程如下所示
     * /------------------------------------------\
     * |     p                         p          |
     * |    / \                       / \         |
     * |   d   x      rotate right   d   y        |
     * |      / \     ===========>      / \       |
     * |     y   a                     b   x      |
     * |    / \                           / \     |
     * |   b   c                         c   a    |
     * \------------------------------------------/
     */
    template<class NodePtr>
    void rb_tree_rotate_right(NodePtr x, NodePtr &root) noexcept {
        /* y 为 x 的左子节点 */
        NodePtr y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;

        if (x == root) {
            /* 如果 x 为根节点，让 y 顶替 x 成为根节点 */
            root = y;
        } else if (rb_tree_is_lchild(x)) {
            /* 如果 x 是右子节点 */
            x->parent->left = y;
        } else {
            /* 如果 x 是左子节点 */
            x->parent->right = y;
        }
        /* 调整 x 与 y 的关系 */
        y->right = x;
        x->parent = y;
    }

    /*
     * 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
     * case 1: 新增节点位于根节点，令新增节点为黑
     * case 2: 新增节点的父节点为黑，没有破坏平衡，直接返回
     * case 3: 父节点和叔叔节点都为红，令父节点和叔叔节点为黑，祖父节点为红，然后令祖父节点为当前节点，继续处理
     * case 4: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为右（左）孩子，
     *          让父节点成为当前节点，再以当前节点为支点左（右）旋
     * case 5: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
     *          让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
     *  参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
     *          http://blog.csdn.net/v_JULY_v/article/details/6109153
     */
    template<class NodePtr>
    void rb_tree_insert_rebalance(NodePtr x, NodePtr &root) noexcept {
        /* 新增节点为红色 */
        rb_tree_set_red(x);
        while (x != root && rb_tree_is_red(x->parent)) {
            if (rb_tree_is_lchild(x->parent)) {
                /* 如果父节点是左子节点 */
                NodePtr uncle = x->parent->parent->right;
                if (uncle != nullptr && rb_tree_is_red(uncle)) {
                    /* case 3: 父节点和叔叔节点都为红 */
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(uncle);
                    x = x->parent->parent;
                    rb_tree_set_red(x);
                } else {
                    /* 无叔叔节点或叔叔节点为黑 */
                    if (!rb_tree_is_lchild(x)) {
                        /* case 4: 当前节点 x 为右子节点 */
                        x = x->parent;
                        rb_tree_rotate_left(x, root);
                    }
                    /* 都转换成 case 5： 当前节点为左子节点 */
                    rb_tree_set_black(x->parent);
                    rb_tree_set_red(x->parent->parent);
                    rb_tree_rotate_right(x->parent->parent, root);
                    break;
                }
            } else {
                /* 如果父节点是右子节点，对称处理 */
                NodePtr uncle = x->parent->parent->left;
                if (uncle != nullptr && rb_tree_is_red(uncle)) {
                    /* case 3: 父节点和叔叔节点都为红 */
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(uncle);
                    x = x->parent->parent;
                    rb_tree_set_red(x);
                    /* 此时祖父节点为红，可能会破坏红黑树的性质，令当前节点为祖父节点，继续处理 */
                } else {
                    /* 无叔叔节点或叔叔节点为黑 */
                    if (rb_tree_is_lchild(x)) {
                        /* case 4: 当前节点 x 为左子节点 */
                        x = x->parent;
                        rb_tree_rotate_right(x, root);
                    }
                    /* 都转换成 case 5： 当前节点为左子节点 */
                    rb_tree_set_black(x->parent);
                    rb_tree_set_red(x->parent->parent);
                    rb_tree_rotate_left(x->parent->parent, root);
                    break;
                }
            }
        }
        /* 根节点永远为黑 */
        rb_tree_set_black(root);
    }

    /*
     * 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，
     * 参数二为根节点，参数三为最小节点，参数四为最大节点
     * 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
     *          http://blog.csdn.net/v_JULY_v/article/details/6109153
     */
    template<class NodePtr>
    NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr &root, NodePtr &leftmost, NodePtr &rightmost) {
        /* y 是可能的替换节点，指向最终要删除的节点 */
        NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
        /* x 是 y 的一个独子节点或 NIL 节点 */
        NodePtr x = y->left != nullptr ? y->left : y->right;
        /* xp 为 x 的父节点 */
        NodePtr xp = nullptr;

        /* y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。*/
        /* 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z */
        if (y != z) {
            z->left->parent = y;
            y->left = z->left;
            /* 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子 */
            if (y != z->right) {
                /* x 替换 y 的位置 */
                xp = y->parent;
                if (x != nullptr) {
                    x->parent = y->parent;
                }
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            } else {
                xp = y;
            }
            /* 连接 y 与 z 的父节点 */
            if (root == z) {
                root = y;
            } else if (rb_tree_is_lchild(z)) {
                z->parent->left = y;
            } else {
                z->parent->right = y;
            }
            y->parent = z->parent;
            tinySTL::swap(y->color, z->color);
            y = z;
        } else {
            /* y == z 说明 z 至多只有一个孩子 */
            xp = y->parent;
            if (x) {
                x->parent = y->parent;
            }
            /* 连接 x 与 z 的父节点 */
            if (root == z) {
                root = x;
            } else if (rb_tree_is_lchild(z)) {
                z->parent->left = x;
            } else {
                z->parent->right = x;
            }
            /* 此时 z 有可能是最左节点或最右节点，更新数据 */
            if (leftmost == z) {
                leftmost = x == nullptr ? xp : rb_tree_min(x);
            }
            if (rightmost == z) {
                rightmost = x == nullptr ? xp : rb_tree_max(x);
            }
        }
        /*
         * 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
         * 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
         * case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
         * case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
         * case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
         *          令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
         * case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
         *          为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
         */
        if (!rb_tree_is_red(y)) {
            /* x 为黑色时，调整，否则直接将 x 变为黑色即可 */
            while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
                if (x == xp->left) {
                    /* 如果 x 为左子节点 */
                    NodePtr brother = xp->right;
                    if (rb_tree_is_red(brother)) {
                        /* case 1 */
                        rb_tree_set_black(brother);
                        rb_tree_set_red(xp);
                        rb_tree_rotate_left(xp, root);
                        brother = xp->right;
                    }
                    /* case 1 转为为了 case 2、3、4 中的一种 */
                    if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                        (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                        /* case 2 */
                        rb_tree_set_red(brother);
                        x = xp;
                        xp = xp->parent;
                    } else {
                        if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
                            /* case 3 */
                            if (brother->left != nullptr)
                                rb_tree_set_black(brother->left);
                            rb_tree_set_red(brother);
                            rb_tree_rotate_right(brother, root);
                            brother = xp->right;
                        }
                        /* 转为 case 4 */
                        brother->color = xp->color;
                        rb_tree_set_black(xp);
                        if (brother->right != nullptr)
                            rb_tree_set_black(brother->right);
                        rb_tree_rotate_left(xp, root);
                        break;
                    }
                } else {
                    /* x 为右子节点，对称处理 */
                    NodePtr brother = xp->left;
                    if (rb_tree_is_red(brother)) {
                        /* case 1 */
                        rb_tree_set_black(brother);
                        rb_tree_set_red(xp);
                        rb_tree_rotate_right(xp, root);
                        brother = xp->left;
                    }
                    if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
                        (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                        /* case 2 */
                        rb_tree_set_red(brother);
                        x = xp;
                        xp = xp->parent;
                    } else {
                        if (brother->left == nullptr || !rb_tree_is_red(brother->left)) {
                            /* case 3 */
                            if (brother->right != nullptr) {
                                rb_tree_set_black(brother->right);
                            }
                            rb_tree_set_red(brother);
                            rb_tree_rotate_left(brother, root);
                            brother = xp->left;
                        }
                        /* 转为 case 4 */
                        brother->color = xp->color;
                        rb_tree_set_black(xp);
                        if (brother->left != nullptr) {
                            rb_tree_set_black(brother->left);
                        }
                        rb_tree_rotate_right(xp, root);
                        break;
                    }
                }
            }
            if (x != nullptr) {
                rb_tree_set_black(x);
            }
        }
        return y;
    }


    // ================================================================================

    /*
     * 红黑树的迭代器设计
     * 迭代器类的基类
     * 继承
     * template<class Category, class T, class Distance = ptrdiff_t,
     * class Pointer = T *, class Reference = T &>
     * struct iterator{ ... }
     * 可以自动定义迭代器所需的五种类型
     * 红黑树 的迭代器属于双向迭代器类型
     */
    template<typename T>
    struct rb_tree_iterator_base : public tinySTL::iterator<tinySTL::bidirectional_iterator_tag, T> {
        /*
         * 类型别名定义
         */
        typedef typename rb_tree_traits<T>::base_ptr base_ptr;

        /*
         * 指向节点本身
         */
        base_ptr node;

        /*
         * 默认构造函数
         */
        rb_tree_iterator_base() : node(nullptr) {

        }

        /*
         * 迭代器前进一步，解析见侯捷的STL源码剖析
         */
        void inc() {
            if (node->right != nullptr) {
                node = rb_tree_min(node->right);
            } else {
                /* 如果没有右子节点 */
                base_ptr y = node->parent;
                while (y->right == node) {
                    node = y;
                    y = y->parent;
                }
                if (node->right != y) {
                    /* 应对“寻找根节点的下一节点，而根节点没有右子节点”的特殊情况 */
                    node = y;
                }
            }
        }

        /*
         * 迭代器前进一步，解析见侯捷的STL源码剖析
         */
        void dec() {
            if (node->parent->parent == node && rb_tree_is_red(node)) { // 如果 node 为 header
                /* 指向整棵树的 max 节点 */
                node = node->right;
            } else if (node->left != nullptr) {
                node = rb_tree_max(node->left);
            } else {
                /* 非 header 节点，也无左子节点 */
                base_ptr y = node->parent;
                while (node == y->left) {
                    node = y;
                    y = y->parent;
                }
                node = y;
            }
        }

        /*
         * 操作符相等于重载
         */
        bool operator==(const rb_tree_iterator_base &rhs) {
            return node == rhs.node;
        }

        /*
         * 操作符不等于重载
         */
        bool operator!=(const rb_tree_iterator_base &rhs) {
            return node != rhs.node;
        }
    };

    /*
     * 红黑树的迭代器类设计
     */
    template<typename T>
    struct rb_tree_iterator : public rb_tree_iterator_base<T> {
        /*
         * 类中使用类型别名定义
         */
        typedef rb_tree_traits<T> tree_traits;
        typedef typename tree_traits::value_type value_type;
        typedef typename tree_traits::pointer pointer;
        typedef typename tree_traits::reference reference;
        typedef typename tree_traits::base_ptr base_ptr;
        typedef typename tree_traits::node_ptr node_ptr;

        /*
         * 定义迭代器别名
         */
        typedef rb_tree_iterator<T> iterator;
        typedef rb_tree_const_iterator<T> const_iterator;
        typedef iterator self;

        /*
         * 使用父类的node变量
         */
        using rb_tree_iterator_base<T>::node;

        /*
         * 默认构造函数
         */
        rb_tree_iterator() {

        }

        /*
         * 有参构造函数，接收一个base_ptr
         */
        rb_tree_iterator(base_ptr x) {
            node = x;
        }

        /*
         * 有参构造函数，接收一个node_ptr
         */
        rb_tree_iterator(node_ptr x) {
            node = x->get_base_ptr();
        }

        /*
         * 拷贝构造函数
         */
        rb_tree_iterator(const iterator &rhs) {
            node = rhs.node;
        }

        /*
         * 构造函数，接收一个rb_tree_const_iterator<T>对象
         */
        rb_tree_iterator(const const_iterator &rhs) {
            node = rhs.node;
        }

        /*
         * 重载此类的 * 操作符
         */
        reference operator*() const {
            return node->get_node_ptr()->value;
        }

        /*
         * 重载此类的 -> 操作符
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载此类的前置 ++ 操作符
         */
        self &operator++() {
            this->inc();
            return *this;
        }

        /*
         * 重载此类的后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }

        /*
         * 重载此类的前置 -- 操作符
         */
        self &operator--() {
            this->dec();
            return *this;
        }

        /*
         * 重载此类的后置 -- 操作符
         */
        self operator--(int) {
            self temp(*this);
            --*this;
            return temp;
        }
    };

    /*
     * 红黑树的迭代器类设计 const类型迭代器
     */
    template<typename T>
    struct rb_tree_const_iterator : public rb_tree_iterator_base<T> {
        /*
         * 定义类中的类型别名
         */
        typedef rb_tree_traits<T> tree_traits;
        typedef typename tree_traits::value_type value_type;
        typedef typename tree_traits::const_pointer pointer;
        typedef typename tree_traits::const_reference reference;
        typedef typename tree_traits::base_ptr base_ptr;
        typedef typename tree_traits::node_ptr node_ptr;

        /*
         * 定义迭代器别名
         */
        typedef rb_tree_iterator<T> iterator;
        typedef rb_tree_const_iterator<T> const_iterator;
        typedef const_iterator self;

        /*
         * 使用父类的node变量
         */
        using rb_tree_iterator_base<T>::node;

        /*
         * 默认构造函数
         */
        rb_tree_const_iterator() {

        }

        /*
         * 有参构造函数，接收一个base_ptr
         */
        rb_tree_const_iterator(base_ptr x) {
            node = x;
        }

        /*
         * 有参构造函数，接收一个node_ptr
         */
        rb_tree_const_iterator(node_ptr x) {
            node = x->get_base_ptr();
        }

        /*
         * 构造函数，接收一个rb_tree_iterator<T>对象
         */
        rb_tree_const_iterator(const iterator &rhs) {
            node = rhs.node;
        }

        /*
         * 拷贝构造函数
         */
        rb_tree_const_iterator(const const_iterator &rhs) {
            node = rhs.node;
        }

        /*
         * 重载此类的 * 操作符
         */
        reference operator*() const {
            return node->get_node_ptr()->value;
        }

        /*
         * 重载此类的 -> 操作符
         */
        pointer operator->() const {
            return &(operator*());
        }

        /*
         * 重载此类的前置 ++ 操作符
         */
        self &operator++() {
            this->inc();
            return *this;
        }

        /*
         * 重载此类的后置 ++ 操作符
         */
        self operator++(int) {
            self temp(*this);
            ++*this;
            return temp;
        }

        /*
         * 重载此类的前置 -- 操作符
         */
        self &operator--() {
            this->dec();
            return *this;
        }

        /*
         * 重载此类的后置 -- 操作符
         */
        self operator--(int) {
            self temp(*this);
            --*this;
            return temp;
        }
    };

    // ==========================================================================================

    /*
     * 模板类 rb_tree
     * 参数 T 代表数据类型，参数 Compare 代表键值比较类型
     */
    template<typename T, typename Compare>
    class rb_tree {
    public:
        /*
         * 定义类中使用相关别名
         */
        typedef rb_tree_traits<T> tree_traits;
        typedef typename tree_traits::value_traits value_traits;
        typedef typename tree_traits::base_type base_type;
        typedef typename tree_traits::base_ptr base_ptr;
        typedef typename tree_traits::node_type node_type;
        typedef typename tree_traits::node_ptr node_ptr;
        typedef typename tree_traits::key_type key_type;
        typedef typename tree_traits::mapped_type mapped_type;
        typedef typename tree_traits::value_type value_type;

        typedef Compare key_compare;

        /*
         * 定义空间配置器相关的别名
         */
        /* 定义一下两个别名的目的是使用其中的construct方法与destroy方法创建或销毁 节点 中的T类型元素 */
        typedef tinySTL::allocator<T> allocator_type;
        typedef tinySTL::allocator<T> data_allocator;

        typedef tinySTL::allocator<base_type> base_allocator;
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
         * 迭代器别名定义
         */
        typedef rb_tree_iterator<T> iterator;
        typedef rb_tree_const_iterator<T> const_iterator;
        typedef tinySTL::reverse_iterator<iterator> reverse_iterator;
        typedef tinySTL::reverse_iterator<const_iterator> const_reverse_iterator;

        /*
         * 获取空间配置器实例
         */
        allocator_type get_allocator() const {
            return data_allocator();
        }

        /*
         * 返回比较函数对象
         */
        key_compare key_comp() const {
            return key_comp_;
        }

    private:
        /*
         * 用以下三个变量描述红黑树
         * header_: 特殊节点，与根节点互为对方的父节点
         * node_count_: 节点数
         * key_comp_: 节点键值比较的准则
         */
        base_ptr header_;
        size_type node_count_;
        key_compare key_comp_;

    private:
        /*
         * 获取根节点
         */
        base_ptr &root() const {
            return header_->parent;
        }

        /*
         * 获取最左节点，header_节点的左指针指向最左的节点（即最小的节点）
         */
        base_ptr &leftmost() const {
            return header_->left;
        }

        /*
         * 获取最左节点，header_节点的右指针指向最右的节点（即最大的节点）
         */
        base_ptr &rightmost() const {
            return header_->right;
        }

    public:
        /*
         * 默认构造函数
         */
        rb_tree() {
            rb_tree_init();
        }

        /*
         * 拷贝构造函数
         */
        rb_tree(const rb_tree &rhs);

        /*
         * 移动构造函数
         */
        rb_tree(rb_tree &&rhs) noexcept;

        /*
         * 拷贝复制函数
         */
        rb_tree &operator=(const rb_tree &rhs);

        /*
         * 移动复制函数
         */
        rb_tree &operator=(rb_tree &&rhs);

        /*
         * 析构函数
         */
        ~rb_tree() {
            clear();
        }

    public:
        /*
         * 类成员函数
         */

        /*
         * 迭代器相关操作
         */

        /*
         * 获取头部迭代器
         */
        iterator begin() noexcept {
            /* 返回具有最小值节点的指针，这是迭代器开始的地方 */
            return leftmost();
        }

        /*
         * 获取头部迭代器 const重载
         */
        const_iterator begin() const noexcept {
            /* 返回具有最小值节点的指针，这是迭代器开始的地方 */
            return leftmost();
        }

        /*
         * 获取尾部迭代器
         */
        iterator end() noexcept {
            /* 返回header_指针，这是迭代器结束的地方 */
            return header_;
        }

        /*
         * 获取尾部迭代器 const重载
         */
        const_iterator end() const noexcept {
            /* 返回header_指针，这是迭代器结束的地方 */
            return header_;
        }

        /*
         * 获取反向头部迭代器
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        /*
         * 获取反向头部迭代器 const重载
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(end());
        }

        /*
         * 获取反向尾部迭代器
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        /*
         * 获取反向尾部迭代器 const重载
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
         * 容量相关操作
         */

        /*
         * 查询红黑树是或否为空
         */
        bool empty() const noexcept {
            return node_count_ == 0;
        }

        /*
         * 查询红黑树大小
         */
        size_type size() const noexcept {
            return node_count_;
        }

        /*
         * 查询红黑树最大容量
         */
        size_type max_size() const noexcept {
            return static_cast<size_type>(-1);
        }

        /*
         * 插入删除相关操作
         */

        /*
         * 在红黑树中新构建一个节点,该节点可以重复,返回指向该节点的迭代器
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_multi(Args &&...args);

        /*
         * 在红黑树中新构建一个节点,该节点不可以重复
         * 返回一个pair对象，指示是否插入成功
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        tinySTL::pair<iterator, bool> emplace_unique(Args &&...args);

        /*
         * 就地插入元素，键值允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_multi_use_hint(iterator hint, Args &&...args);

        /*
         * 就地插入元素，键值不允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        iterator emplace_unique_use_hint(iterator hint, Args &&...args);

        /*
         * 插入相关函数
         */

        /*
         * 插入元素，节点键值允许重复
         */
        iterator insert_multi(const value_type &value);

        /*
         * 插入元素，节点键值允许重复 移动语义
         */
        iterator insert_multi(value_type &&value) {
            return emplace_multi(tinySTL::move(value));
        }

        /*
         * 带有hint的元素插入，节点键值允许重复
         */
        iterator insert_multi(iterator hint, const value_type &value) {
            return emplace_multi_use_hint(hint, value);
        }

        /*
         * 带有hint的元素插入，节点键值允许重复  移动语义
         */
        iterator insert_multi(iterator hint, value_type &&value) {
            return emplace_multi_use_hint(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器区间的元素，节点键值允许重复
         * 类成员模板
         */
        template<typename InputIterator>
        void insert_multi(InputIterator first, InputIterator last) {
            size_type n = tinySTL::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
            for (; n > 0; --n, ++first) {
                insert_multi(end(), *first);
            }
        }

        /*
         * 插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
         */
        tinySTL::pair<iterator, bool> insert_unique(const value_type &value);

        /*
         * 移动语义
         * 插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
         */
        tinySTL::pair<iterator, bool> insert_unique(value_type &&value) {
            return emplace_unique(tinySTL::move(value));
        }

        /*
         * 带有hint的插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
         */
        iterator insert_unique(iterator hint, const value_type &value) {
            return emplace_unique_use_hint(hint, value);
        }

        /*
         * 移动语义
         * 带有hint的插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
         */
        iterator insert_unique(iterator hint, value_type &&value) {
            return emplace_unique_use_hint(hint, tinySTL::move(value));
        }

        /*
         * 插入迭代器区间的元素，节点键值不允许重复
         * 类成员模板
         */
        template<typename InputIterator>
        void insert_unique(InputIterator first, InputIterator last) {
            size_type n = tinySTL::distance(first, last);
            THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
            for (; n > 0; --n, ++first) {
                insert_unique(end(), *first);
            }
        }

        /*
         * 删除清空等操作
         */

        /*
         * 删除 hint 位置的节点
         */
        iterator erase(iterator hint);

        /*
         * 删除键值等于 key 的元素，返回删除的个数
         */
        size_type erase_multi(const key_type &key);

        /*
         * 删除键值等于 key 的元素，返回删除的个数
         */
        size_type erase_unique(const key_type &key);

        /*
         * 删除[first, last)区间内的元素
         */
        void erase(iterator first, iterator last);

        /*
         * 清空红黑树
         */
        void clear();

        /*
         * 红黑树相关操作
         */

        /*
         * 查找键值为 k 的节点，返回指向它的迭代器
         */
        iterator find(const key_type &key);

        /*
         * 查找键值为 k 的节点，返回指向它的迭代器 const重载
         */
        const_iterator find(const key_type &key) const;

        /*
         * 获取键为key的节点个数
         */
        size_type count_multi(const key_type &key) const {
            auto p = equal_range_multi(key);
            return static_cast<size_type>(tinySTL::distance(p.first, p.second));
        }

        /*
         * 获取是否存在键为key的节点
         */
        size_type count_unique(const key_type &key) const {
            return find(key) != end() ? 1 : 0;
        }

        /*
         * 键值不小于 key 的第一个位置
         */
        iterator lower_bound(const key_type &key);

        /*
         * 键值不小于 key 的第一个位置 const重载
         */
        const_iterator lower_bound(const key_type &key) const;

        /*
         * 键值大于 key 的第一个位置
         */
        iterator upper_bound(const key_type &key);

        /*
         * 键值大于 key 的第一个位置 const重载
         */
        const_iterator upper_bound(const key_type &key) const;

        /*
         * 获取键等于key的一段区间，返回一个pair指向头和尾
         */
        tinySTL::pair<iterator, iterator> equal_range_multi(const key_type &key) {
            return tinySTL::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
        }

        /*
         * 获取键等于key的一段区间，返回一个pair指向头和尾 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range_multi(const key_type &key) const {
            return tinySTL::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
        }

        /*
         * 获取键等于key的一段区间，区间大小为1，返回一个pair指向头和尾
         */
        tinySTL::pair<iterator, iterator> equal_range_unique(const key_type &key) {
            iterator it = find(key);
            iterator next = it;
            return it == end() ? tinySTL::make_pair(it, it) : tinySTL::make_pair(it, ++next);
        }

        /*
         * 获取键等于key的一段区间，区间大小为1，返回一个pair指向头和尾 const重载
         */
        tinySTL::pair<const_iterator, const_iterator> equal_range_unique(const key_type &key) const {
            const_iterator it = find(key);
            const_iterator next = it;
            return it == end() ? tinySTL::make_pair(it, it) : tinySTL::make_pair(it, ++next);
        }

        /*
         * 交换两个红黑树对象
         */
        void swap(rb_tree &rhs) noexcept;

    private:
        /*
         * 私有的辅助函数
         */

        /*
         * 节点相关函数
         */

        /*
         * 使用参数创建节点
         * 类成员模板，可变参数模板，完美转发
         */
        template<typename ...Args>
        node_ptr create_node(Args &&...args);

        /*
         * 复制一个红黑树节点
         */
        node_ptr clone_node(base_ptr x);

        /*
         * 销毁一个红黑树节点
         */
        void destroy_node(node_ptr p);

        /*
         * 初始化与重置相关函数
         */

        /*
         * 初始化红黑树
         */
        void rb_tree_init();

        /*
         * 重置红黑树状态
         */
        void reset();

        /*
         * 获取插入节点的位置
         */

        /*
         * 获取可重复的插入节点的位置
         */
        tinySTL::pair<base_ptr, bool> get_insert_multi_pos(const key_type &key);

        /*
         * 获取不可重复的插入节点的位置
         */
        tinySTL::pair<tinySTL::pair<base_ptr, bool>, bool>  // 返回值
        get_insert_unique_pos(const key_type &key);

        /*
         * 插入节点或值的相关函数
         */

        /*
         * x 为插入点的父节点， value 为要插入的值，add_to_left 表示是否在左边插入
         */
        iterator insert_value_at(base_ptr x, const value_type &value, bool add_to_left);

        /*
         * 在 x 节点处插入新的节点
         * x 为插入点的父节点， node 为要插入的节点，add_to_left 表示是否在左边插入
         */
        iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

        /*
         * 使用hint提示插入节点
         */

        /*
         * 插入元素，键值允许重复，使用 hint
         */
        iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);

        /*
         * 插入元素，键值不允许重复，使用 hint
         */
        iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

        /*
         * 拷贝或删除一颗红黑树
         */

        /*
         * 从另一颗红黑树复制数据
         * 递归复制一颗树，节点从 x 开始，p 为 x 的父节点
         */
        base_ptr copy_from(base_ptr x, base_ptr p);

        /*
         * 从 x 节点开始删除该节点及其子树
         */
        void erase_since(base_ptr x);
    };

    // ==========================================================================================

    /*
     * 类成员函数
     */

    /*
     * 构造函数以及赋值函数
     */

    /*
     * 拷贝构造函数
     */
    template<typename T, typename Compare>
    rb_tree<T, Compare>::rb_tree(const rb_tree &rhs) {
        rb_tree_init();
        if (rhs.node_count_ != 0) {
            root() = copy_from(rhs.root(), header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }

    /*
     * 移动构造函数
     */
    template<typename T, typename Compare>
    rb_tree<T, Compare>::rb_tree(rb_tree &&rhs) noexcept
            :header_(tinySTL::move(rhs.header_)), node_count_(rhs.node_count_), key_comp_(rhs.key_comp_) {
        rhs.reset();
    }

    /*
     * 拷贝复制函数
     */
    template<typename T, typename Compare>
    rb_tree<T, Compare> &rb_tree<T, Compare>::operator=(const rb_tree &rhs) {
        if (this != &rhs) {
            clear();
            if (rhs.node_count_ != 0) {
                root() = copy_from(rhs.root(), header_);
                leftmost() = rb_tree_min(root());
                rightmost() = rb_tree_max(root());
            }
            node_count_ = rhs.node_count_;
            key_comp_ = rhs.key_comp_;
        }
        return *this;
    }

    /*
     * 移动复制函数
     */
    template<typename T, typename Compare>
    rb_tree<T, Compare> &rb_tree<T, Compare>::operator=(rb_tree &&rhs) {
        clear();
        header_ = tinySTL::move(rhs.header_);
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
        rhs.reset();
        return *this;
    }

    /*
     * 在红黑树中新构建一个节点,该节点可以重复,返回指向该节点的迭代器
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename Compare>
    template<typename ...Args>
    typename rb_tree<T, Compare>::iterator rb_tree<T, Compare>::emplace_multi(Args &&...args) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        auto res = get_insert_multi_pos(value_traits::get_key(np->value));
        return insert_node_at(res.first, np, res.second);
    }

    /*
     * 在红黑树中新构建一个节点,该节点不可以重复
     * 返回一个pair对象，指示是否插入成功
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename Compare>
    template<typename ...Args>
    tinySTL::pair<typename rb_tree<T, Compare>::iterator, bool>  // 返回值
    rb_tree<T, Compare>::emplace_unique(Args &&...args) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        auto res = get_insert_unique_pos(value_traits::get_key(np->value));
        if (res.second) {
            /* 插入成功 */
            return tinySTL::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
        }
        destroy_node(np);
        return tinySTL::make_pair(iterator(res.first.first), false);
    }

    /*
     * 就地插入元素，键值允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename Compare>
    template<typename ...Args>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::emplace_multi_use_hint(iterator hint, Args &&...args) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        if (node_count_ == 0) {
            return insert_node_at(header_, np, true);
        }
        key_type key = value_traits::get_key(np->value);
        if (hint == begin()) {
            /* 位于 begin 处 */
            if (key_comp_(key, value_traits::get_key(*hint))) {
                return insert_node_at(hint.node, np, true);
            } else {
                auto pos = get_insert_multi_pos(key);
                return insert_node_at(pos.first, np, pos.second);
            }
        } else if (hint == end()) {
            /* 位于 end 处 */
            if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value))) {
                return insert_node_at(rightmost(), np, false);
            } else {
                auto pos = get_insert_multi_pos(key);
                return insert_node_at(pos.first, np, pos.second);
            }
        }
        return insert_multi_use_hint(hint, key, np);
    }

    /*
     * 就地插入元素，键值不允许重复，当 hint 位置与插入位置接近时，插入操作的时间复杂度可以降低
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename Compare>
    template<typename ...Args>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::emplace_unique_use_hint(iterator hint, Args &&...args) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        node_ptr np = create_node(tinySTL::forward<Args>(args)...);
        if (node_count_ == 0) {
            return insert_node_at(header_, np, true);
        }
        key_type key = value_traits::get_key(np->value);
        if (hint == begin()) {
            /* 位于 begin 处 */
            if (key_comp_(key, value_traits::get_key(*hint))) {
                return insert_node_at(hint.node, np, true);
            } else {
                auto pos = get_insert_unique_pos(key);
                if (!pos.second) {
                    destroy_node(np);
                    return pos.first.first;
                }
                return insert_node_at(pos.first.first, np, pos.first.second);
            }
        } else if (hint == end()) {
            /* 位于 end 处 */
            if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key)) {
                return insert_node_at(rightmost(), np, false);
            } else {
                auto pos = get_insert_unique_pos(key);
                if (!pos.second) {
                    destroy_node(np);
                    return pos.first.first;
                }
                return insert_node_at(pos.first.first, np, pos.first.second);
            }
        }
        return insert_unique_use_hint(hint, key, np);
    }

    /*
     * 插入元素，节点键值允许重复
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::insert_multi(const value_type &value) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        auto res = get_insert_multi_pos(value_traits::get_key(value));
        return insert_value_at(res.first, value, res.second);
    }

    /*
     * 插入新值，节点键值不允许重复，返回一个 pair，若插入成功，pair 的第二参数为 true，否则为 false
     */
    template<typename T, typename Compare>
    tinySTL::pair<typename rb_tree<T, Compare>::iterator, bool>   // 返回值
    rb_tree<T, Compare>::insert_unique(const value_type &value) {
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
        auto res = get_insert_unique_pos(value_traits::get_key(value));
        if (res.second) {
            /* 插入成功 */
            return tinySTL::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
        }
        return tinySTL::make_pair(res.first.first, false);
    }

    /*
     * 删除 hint 位置的节点
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator rb_tree<T, Compare>::erase(iterator hint) {
        node_ptr node = hint.node->get_node_ptr();
        iterator next(node);
        ++next;
        rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
        destroy_node(node);
        --node_count_;
        return next;
    }

    /*
     * 删除键值等于 key 的元素，返回删除的个数
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::size_type rb_tree<T, Compare>::erase_multi(const key_type &key) {
        auto p = equal_range_multi(key);
        size_type n = tinySTL::distance(p.first, p.second);
        erase(p.first, p.second);
        return n;
    }

    /*
     * 删除键值等于 key 的元素，返回删除的个数
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::size_type rb_tree<T, Compare>::erase_unique(const key_type &key) {
        iterator it = find(key);
        if (it != end()) {
            erase(it);
            return 1;
        }
        return 0;
    }

    /*
     * 删除[first, last)区间内的元素
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::erase(iterator first, iterator last) {
        if (first == begin() && last == end()) {
            clear();
        } else {
            while (first != last) {
                erase(first++);
            }
        }
    }

    /*
     * 清空红黑树
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::clear() {
        if (node_count_ != 0) {
            erase_since(root());
            leftmost() = header_;
            root() = nullptr;
            rightmost() = header_;
            node_count_ = 0;
        }
    }

    /*
     * 查找键值为 k 的节点，返回指向它的迭代器
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::find(const key_type &key) {
        /* y为最后一个不小于 key 的节点 */
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
                /* key 小于等于 x 键值，向左走 */
                y = x;
                x = x->left;
            } else {
                /* key 大于 x 键值，向右走 */
                x = x->right;
            }
        }
        iterator j = iterator(y);
        return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
    }

    /*
     * 查找键值为 k 的节点，返回指向它的迭代器 const重载
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::const_iterator  // 返回值
    rb_tree<T, Compare>::find(const key_type &key) const {
        /* y为最后一个不小于 key 的节点 */
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
                /* key 小于等于 x 键值，向左走 */
                y = x, x = x->left;
            } else {
                /* key 大于 x 键值，向右走 */
                x = x->right;
            }
        }
        const_iterator j = const_iterator(y);
        return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
    }

    /*
     * 键值不小于 key 的第一个位置
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::lower_bound(const key_type &key) {
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
                /* key <= x */
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return iterator(y);
    }

    /*
     * 键值不小于 key 的第一个位置 const重载
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::const_iterator  // 返回值
    rb_tree<T, Compare>::lower_bound(const key_type &key) const {
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
                /* key <= x */
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return const_iterator(y);
    }

    /*
     * 键值大于等于 key 的第一个位置
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::upper_bound(const key_type &key) {
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
                /* key < x */
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return iterator(y);
    }

    /*
     * 键值大于等于 key 的第一个位置 const重载
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::const_iterator  // 返回值
    rb_tree<T, Compare>::upper_bound(const key_type &key) const {
        base_ptr y = header_;
        base_ptr x = root();
        while (x != nullptr) {
            if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
                /* key < x */
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return const_iterator(y);
    }

    /*
     * 交换两个红黑树对象
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::swap(rb_tree &rhs) noexcept {
        if (this != &rhs) {
            tinySTL::swap(header_, rhs.header_);
            tinySTL::swap(node_count_, rhs.node_count_);
            tinySTL::swap(key_comp_, rhs.key_comp_);
        }
    }

    // ==========================================================================================

    /*
     * 私有的辅助函数
     */

    /*
     * 使用参数创建节点
     * 类成员模板，可变参数模板，完美转发
     */
    template<typename T, typename Compare>
    template<typename ...Args>
    typename rb_tree<T, Compare>::node_ptr
    rb_tree<T, Compare>::create_node(Args &&...args) {
        node_ptr temp = node_allocator::allocate(1);
        try {
            data_allocator::construct(tinySTL::address_of(temp->value), tinySTL::forward<Args>(args)...);
            temp->left = nullptr;
            temp->right = nullptr;
            temp->parent = nullptr;
        }
        catch (...) {
            node_allocator::deallocate(temp);
            throw;
        }
        return temp;
    }

    /*
     * 复制一个红黑树节点
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::node_ptr
    rb_tree<T, Compare>::clone_node(base_ptr x) {
        node_ptr temp = create_node(x->get_node_ptr()->value);
        temp->color = x->color;
        temp->left = nullptr;
        temp->right = nullptr;
        return temp;
    }

    /*
     * 销毁一个红黑树节点
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::destroy_node(node_ptr p) {
        data_allocator::destroy(tinySTL::address_of(p->value));
        node_allocator::deallocate(p);
    }

    /*
     * 初始化红黑树
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::rb_tree_init() {
        /* 给header节点分配空间， base_type 即可 */
        header_ = base_allocator::allocate(1);
        /* header_ 节点颜色为红，与 root 区分 */
        header_->color = rb_tree_red;
        root() = nullptr;
        /* 目前左右子树指针先指向自身 */
        leftmost() = header_;
        rightmost() = header_;
        node_count_ = 0;
    }

    /*
     * 重置红黑树状态
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::reset() {
        header_ = nullptr;
        node_count_ = 0;
    }

    /*
     * 获取可重复的插入节点的位置
     */
    template<typename T, typename Compare>
    tinySTL::pair<typename rb_tree<T, Compare>::base_ptr, bool>  // 返回值
    rb_tree<T, Compare>::get_insert_multi_pos(const key_type &key) {
        base_ptr x = root();
        base_ptr y = header_;
        bool add_to_left = true;
        while (x != nullptr) {
            y = x;
            add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
            x = add_to_left ? x->left : x->right;
        }
        return tinySTL::make_pair(y, add_to_left);
    }

    /*
     * 获取不可重复的插入节点的位置
     * 返回一个 pair，第一个值为一个 pair，包含插入点的父节点和一个 bool 表示是否在左边插入
     * 二个值为一个 bool，表示是否插入成功
     */
    template<typename T, typename Compare>
    tinySTL::pair<tinySTL::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>  // 返回值
    rb_tree<T, Compare>::get_insert_unique_pos(const key_type &key) {
        auto x = root();
        auto y = header_;
        /* 树为空时也在 header_ 左边插入 */
        bool add_to_left = true;
        while (x != nullptr) {
            y = x;
            add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
            x = add_to_left ? x->left : x->right;
        }
        /* 此时 y 为插入点的父节点 */
        iterator j = iterator(y);
        if (add_to_left) {
            if (y == header_ || j == begin()) {
                /* 如果树为空树或插入点在最左节点处，肯定可以插入新的节点 */
                return tinySTL::make_pair(tinySTL::make_pair(y, true), true);
            } else {
                /* 否则，如果存在重复节点，那么 --j 就是重复的值 */
                --j;
            }
        }
        if (key_comp_(value_traits::get_key(*j), key)) {
            /* 表明新节点没有重复 */
            return tinySTL::make_pair(tinySTL::make_pair(y, add_to_left), true);
        }
        /* 进行至此，表示新节点与现有节点键值重复 */
        return tinySTL::make_pair(tinySTL::make_pair(y, add_to_left), false);
    }

    /*
     * x 为插入点的父节点， value 为要插入的值，add_to_left 表示是否在左边插入
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::insert_value_at(base_ptr x, const value_type &value, bool add_to_left) {
        node_ptr node = create_node(value);
        node->parent = x;
        base_ptr base_node = node->get_base_ptr();
        if (x == header_) {
            root() = base_node;
            leftmost() = base_node;
            rightmost() = base_node;
        } else if (add_to_left) {
            x->left = base_node;
            if (leftmost() == x)
                leftmost() = base_node;
        } else {
            x->right = base_node;
            if (rightmost() == x)
                rightmost() = base_node;
        }
        rb_tree_insert_rebalance(base_node, root());
        ++node_count_;
        return iterator(node);
    }

    /*
     * 在 x 节点处插入新的节点
     * x 为插入点的父节点， node 为要插入的节点，add_to_left 表示是否在左边插入
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left) {
        node->parent = x;
        base_ptr base_node = node->get_base_ptr();
        if (x == header_) {
            root() = base_node;
            leftmost() = base_node;
            rightmost() = base_node;
        } else if (add_to_left) {
            x->left = base_node;
            if (leftmost() == x)
                leftmost() = base_node;
        } else {
            x->right = base_node;
            if (rightmost() == x)
                rightmost() = base_node;
        }
        rb_tree_insert_rebalance(base_node, root());
        ++node_count_;
        return iterator(node);
    }

    /*
     * 插入元素，键值允许重复，使用 hint
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
        /* 在 hint 附近寻找可插入的位置 */
        base_ptr np = hint.node;
        iterator before = hint;
        --before;
        base_ptr bnp = before.node;
        if (!key_comp_(key, value_traits::get_key(*before)) &&
            !key_comp_(value_traits::get_key(*hint), key)) {
            /* before <= node <= hint */
            if (bnp->right == nullptr) {
                return insert_node_at(bnp, node, false);
            } else if (np->left == nullptr) {
                return insert_node_at(np, node, true);
            }
        }
        auto pos = get_insert_multi_pos(key);
        return insert_node_at(pos.first, node, pos.second);
    }

    /*
     * 插入元素，键值不允许重复，使用 hint
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::iterator  // 返回值
    rb_tree<T, Compare>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
        /* 在 hint 附近寻找可插入的位置 */
        base_ptr np = hint.node;
        iterator before = hint;
        --before;
        auto bnp = before.node;
        if (key_comp_(value_traits::get_key(*before), key) &&
            key_comp_(key, value_traits::get_key(*hint))) {
            /* before < node < hint */
            if (bnp->right == nullptr) {
                return insert_node_at(bnp, node, false);
            } else if (np->left == nullptr) {
                return insert_node_at(np, node, true);
            }
        }
        auto pos = get_insert_unique_pos(key);
        if (!pos.second) {
            destroy_node(node);
            return pos.first.first;
        }
        return insert_node_at(pos.first.first, node, pos.first.second);
    }

    /*
     * 从另一颗红黑树复制数据
     * 递归复制一颗树，节点从 x 开始，p 为 x 的父节点
     */
    template<typename T, typename Compare>
    typename rb_tree<T, Compare>::base_ptr
    rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
        node_ptr top = clone_node(x);
        top->parent = p;
        try {
            if (x->right) {
                top->right = copy_from(x->right, top);
            }
            p = top;
            x = x->left;
            while (x != nullptr) {
                node_ptr y = clone_node(x);
                p->left = y;
                y->parent = p;
                if (x->right) {
                    y->right = copy_from(x->right, y);
                }
                p = y;
                x = x->left;
            }
        }
        catch (...) {
            erase_since(top);
            throw;
        }
        return top;
    }

    /*
     * 从 x 节点开始删除该节点及其子树
     */
    template<typename T, typename Compare>
    void rb_tree<T, Compare>::erase_since(base_ptr x) {
        while (x != nullptr) {
            erase_since(x->right);
            base_ptr y = x->left;
            destroy_node(x->get_node_ptr());
            x = y;
        }
    }

    // =============================================================================================

    /*
     * 全局区域重载操作符
     */

    /*
     * 重载相等于操作符
     */
    template<typename T, typename Compare>
    bool operator==(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return lhs.size() == rhs.size() && tinySTL::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    /*
     * 重载不相等于操作符
     */
    template<typename T, typename Compare>
    bool operator!=(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return !(lhs == rhs);
    }

    /*
     * 重载小于操作符
     */
    template<typename T, typename Compare>
    bool operator<(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return tinySTL::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    /*
     * 重载大于操作符
     */
    template<typename T, typename Compare>
    bool operator>(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return rhs < lhs;
    }

    /*
     * 重载小于等于操作符
     */
    template<typename T, typename Compare>
    bool operator<=(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return !(rhs < lhs);
    }

    /*
     * 重载大于等于操作符
     */
    template<typename T, typename Compare>
    bool operator>=(const rb_tree<T, Compare> &lhs, const rb_tree<T, Compare> &rhs) {
        return !(lhs < rhs);
    }

    /*
     * 重载tinySTL的swap
     */
    template<typename T, typename Compare>
    void swap(rb_tree<T, Compare> &lhs, rb_tree<T, Compare> &rhs) noexcept {
        lhs.swap(rhs);
    }

}  // namespace tinySTL

#endif //TINYSTL_RB_TREE_H
