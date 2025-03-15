/**
 * @file lockfree_queue.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

/**
 * https://mp.weixin.qq.com/s/-9RJgcpamtXZOFflOkkt5w
 * https://linuxcpp.0voice.com/?id=128594
 * https://coolshell.cn/articles/8239.html
 * https://standards.wiki/c/c_library_stdatomic_enumeration_constant_memory_order_release.html
 *
 * CAS (Compare and Swap) 操作：
 * 在实际的C/C++程序中，CAS的各种实现版本如下：
 * 1）GCC 的 CAS
 * 2）Windows 的 CAS
 * 3) C++11 中的 CAS
 *
 * 需要的元素：
 * - 存储结构：链表或队列（如循环队列）
 * - 头尾指针：head, tail
*/

#ifndef FREE_LOCK_QUEUE_H
#define FREE_LOCK_QUEUE_H



#endif /** FREE_LOCK_QUEUE_H */