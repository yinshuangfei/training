#include "rb_tree.h"


/* 
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y                
 *   /  \      --(左旋)-->           / \      
 *  lx   y                          x  ry     
 *     /   \                       /  \
 *    ly   ry                     lx  ly  
 *
 *
 */
// static void rbtree_left_rotate(RBRoot *root, Node *x)
// {
//     // 设置x的右孩子为y
//     Node *y = x->right;

//     // 将 “y的左孩子” 设为 “x的右孩子”；
//     // 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
//     x->right = y->left;
//     if (y->left != NULL)
//         y->left->parent = x;

//     // 将 “x的父亲” 设为 “y的父亲”
//     y->parent = x->parent;

//     if (x->parent == NULL)
//     {
//         //tree = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
//         root->node = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
//     }
//     else
//     {
//         if (x->parent->left == x)
//             x->parent->left = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
//         else
//             x->parent->right = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
//     }
    
//     // 将 “x” 设为 “y的左孩子”
//     y->left = x;
//     // 将 “x的父节点” 设为 “y”
//     x->parent = y;
// }



/* 
 * 对红黑树的节点(y)进行右旋转
 *
 * 右旋示意图(对节点y进行右旋)：
 *            py                               py
 *           /                                /
 *          y                                x                  
 *         /  \      --(右旋)-->            /  \                     #
 *        x   ry                           lx   y  
 *       / \                                   / \                   #
 *      lx  rx                                rx  ry
 * 
 */
// static void rbtree_right_rotate(RBRoot *root, Node *y)
// {
//     // 设置x是当前节点的左孩子。
//     Node *x = y->left;

//     // 将 “x的右孩子” 设为 “y的左孩子”；
//     // 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
//     y->left = x->right;
//     if (x->right != NULL)
//         x->right->parent = y;

//     // 将 “y的父亲” 设为 “x的父亲”
//     x->parent = y->parent;

//     if (y->parent == NULL) 
//     {
//         //tree = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
//         root->node = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
//     }
//     else
//     {
//         if (y == y->parent->right)
//             y->parent->right = x;    // 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
//         else
//             y->parent->left = x;    // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
//     }

//     // 将 “y” 设为 “x的右孩子”
//     x->right = y;

//     // 将 “y的父节点” 设为 “x”
//     y->parent = x;
// }




rb_root * create_rbtree()
{
    rb_root * root = (rb_root *) malloc(sizeof(rb_root));
    root->node = NULL;
    
    return root;
}