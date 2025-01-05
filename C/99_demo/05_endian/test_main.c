#include <stdio.h>
#include <arpa/inet.h>

#include "../../common/utils.h"

static union {
	char c[4];
	unsigned long l;
} endian_test = {
	/** 以第一个元素的宽度赋值，即 char */
	{ 'l', '?', '?', 'b' }
};

#define ENDIANNESS ((char)endian_test.l)

union uc {
	int  val;
	char arr[4];
};

void print(union uc data)
{
	pr_info("[0]:%x [1]:%x [2]:%x [3]:%x",
		data.arr[0],
		data.arr[1],
		data.arr[2],
		data.arr[3]);
}

int get_endian1()
{
	/**
	 * 小端系统‌：在小端系统中，低位字节存储在低地址，高位字节存储在高地址。发送数
	 *          据前，需要将数据从小端转换为大端，以确保网络传输的正确性。接收端收
	 *          到数据后，也会按照大端字节序存储。
	 *          小端模式下，低位放低地址，高位放高地址，比较适合人的思维习惯。
	 * 大端系统‌：在大端系统中，多字节数据的高位字节存储在低地址，低位字节存储在高
	 *          地址。发送数据时，按照内存中的顺序发送，接收端也按照这个顺序接收
	 *          并存储数据。
	 *
	 * 发送一个32位整数0x12345678时，在大端系统中，发送的顺序为0x12, 0x34, 0x56,
	 * 0x78；在小端系统中，发送的顺序为0x78, 0x56, 0x34, 0x12。
	 * 接收端必须按照大端字节序来解释这些数据，以确保数据的正确性。
	 *
	 * unsigned int value = 0x12345678
	 *
	 *     Little-Endian: 低地址存放低位，如下：
	 *     低地址
	 *     ---------------
	 *     buf[0] (0x78) -- 低位字节
	 *     buf[1] (0x56)
	 *     buf[2] (0x34)
	 *     buf[3] (0x12) -- 高位字节
	 *     --------------
	 *     高地址
	 *
	 *     Big-Endian: 低地址存放高位，如下：
	 *     低地址
	 *     ---------------
	 *     buf[0] (0x12) -- 高位字节
	 *     buf[1] (0x34)
	 *     buf[2] (0x56)
	 *     buf[3] (0x78) -- 低位字节
	 *     ---------------
	 *     高地址
	 **/
	union uc t1, t2;
	t1.val = 0x12345678;
	pr_info("raw value: 0x%x", t1.val);
	print(t1);

	if (t1.arr[0] == 0x78)
		pr_info("little endian");
	else
		pr_info("big endian");

	/**
	 * convert from host to network byte order
	 * 网络字节顺序: 大端字节序
	*/
	t2.val = htonl(t1.val);
	pr_info("network  : 0x%x", t2.val);
	print(t2);
}

void get_endian2()
{
	pr_info("[0]:%c [1]:%c [2]:%c [3]:%c",
		endian_test.c[0],
		endian_test.c[1],
		endian_test.c[2],
		endian_test.c[3]);

	pr_info("%s endian", ENDIANNESS == 'l' ? "little" : "big");
}

int main(void)
{
	get_endian1();
	get_endian2();
}
