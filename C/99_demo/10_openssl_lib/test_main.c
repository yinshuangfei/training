/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-10-01
 *
 * @copyright Copyright (c) 2025
 *
 * @node: openssl base64
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "../../common/utils.h"

int base64_encrypt(char *input, char *buff, int buff_size)
{
	BIO *bio_mem, *bio64;
	int len;

	/** 1. 创建 Base64 filter BIO */
	bio64 = BIO_new(BIO_f_base64());
	// 设置解码模式：忽略换行
	BIO_set_flags(bio64, BIO_FLAGS_BASE64_NO_NL);

	/** 2. 创建内存 BIO（作为底层） */
	bio_mem = BIO_new(BIO_s_mem());
	/** 3. 将 Base64 BIO 连接到内存 BIO（形成链） */
	bio64 = BIO_push(bio64, bio_mem);

	/** 4. 写入数据（自动被 Base64 编码并写入内存） */
	BIO_write(bio64, input, strlen(input));
	/** 必须 flush，否则 Base64 不会输出完整数据（特别是结尾的 =） */
	BIO_flush(bio64);

	/** 5. 从内存 BIO 中读取编码结果 */
	len = BIO_read(bio_mem, buff, buff_size);
	buff[len] = '\0';

	/** 6. 清理, 会自动释放整个链 */
	BIO_free_all(bio64);

	// 注意：BIO_f_base64 默认加换行
	printf("Base64 Encoded: len:%2d, %s\n", len, buff);
	// printf("%d -> %d\n", strlen(input), len);
	return 0;
}

int base64_decrypt(char *input, char *buff, int buff_size)
{
	BIO *bio_mem, *bio64;
	int len;

	/** 1. 创建 Base64 filter BIO（用于解码） */
	bio64 = BIO_new(BIO_f_base64());
	// 设置解码模式：忽略换行
	BIO_set_flags(bio64, BIO_FLAGS_BASE64_NO_NL);

	/** 2. 创建内存 BIO 并写入 Base64 数据 */
	bio_mem = BIO_new(BIO_s_mem());
	BIO_write(bio_mem, input, strlen(input));

	/** 3. 将 Base64 BIO 连接到内存 BIO（数据从 mem 流向 base64 解码） */
	bio64 = BIO_push(bio64, bio_mem);

	/** 4. 读取解码后的数据 */
	len = BIO_read(bio64, buff, buff_size);
	buff[len] = '\0';

	BIO_free_all(bio64);

	printf("Decoded: %s\n", buff);
	return 0;
}

void base64_test()
{
	char enc_name[NAME_MAX + 1] = {0};
	char dec_name[NAME_MAX + 1] = {0};
	char str[NAME_MAX + 1] = {0};

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j <= i; j++) {
			str[j] = 'a';
		}

		base64_encrypt(str, enc_name, sizeof(enc_name) - 1);
		base64_decrypt(enc_name, dec_name, sizeof(enc_name) - 1);
	}
}

// 处理 OpenSSL 错误
void handle_errors()
{
	ERR_print_errors_fp(stderr);
	exit(1);
}

/**
 * @brief SM4-ECB 加密 (PKCS7 填充)
 *
 * @param [in] key
 * @param [in] plaintext
 * @param [in] plaintext_len
 * @param [in out] ciphertext
 * @return int 0: success; <0: failed, posix errno; >0: other success, value;
 */
int sm4_ecb_encrypt(const unsigned char *key,
		    const unsigned char *plaintext,
		    int plaintext_len,
		    unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int ciphertext_len;

	// 创建上下文
	if (!(ctx = EVP_CIPHER_CTX_new()))
		handle_errors();

	// 初始化加密操作
	if (1 != EVP_EncryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, NULL))
		handle_errors();

	// 执行加密
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handle_errors();

	ciphertext_len = len;

	// 结束加密（处理填充）
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		handle_errors();

	ciphertext_len += len;

	// 释放上下文
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

/**
 * @brief SM4-ECB 解密 (PKCS7 填充)
 *
 * @param [in] key
 * @param [in] ciphertext
 * @param [in] ciphertext_len
 * @param [in out] plaintext
 * @return int 0: success; <0: failed, posix errno; >0: other success, value;
 */
int sm4_ecb_decrypt(const unsigned char *key,
		    const unsigned char *ciphertext,
		    int ciphertext_len,
		    unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;

	if (!(ctx = EVP_CIPHER_CTX_new()))
		handle_errors();

	if (1 != EVP_DecryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, NULL))
		handle_errors();

	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext,
				   ciphertext_len))
		handle_errors();

	plaintext_len = len;

	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		handle_errors();

	plaintext_len += len;

	EVP_CIPHER_CTX_free(ctx);
	return plaintext_len;
}

void ecb_test()
{
	if (EVP_sm4_ecb()) {
		printf("✅ SM4-ECB supported\n");
	} else {
		printf("❌ SM4 not available\n");
		return;
	}

	// 16 字节密钥
	unsigned char key[] = "2021112520211125";
	// 明文
	unsigned char plaintext[] = "a";
	// 密文存储
	unsigned char ciphertext[1024];
	// 解密后的文本存储
	unsigned char decryptedtext[1024];

	int ciphertext_len, decryptedtext_len;

	// 加密
	ciphertext_len = sm4_ecb_encrypt(key, plaintext,
					 strlen((char *)plaintext),
					 ciphertext);
	printf("Ciphertext: ");
	for (int i = 0; i < ciphertext_len; i++)
		printf("%02x ", ciphertext[i]);
	printf("\n");

	// 解密
	decryptedtext_len = sm4_ecb_decrypt(key, ciphertext,
					    ciphertext_len, decryptedtext);
	decryptedtext[decryptedtext_len] = '\0';
	printf("Decrypted text: %s\n", decryptedtext);
}

int main(int argc, char *argv[])
{
	base64_test();
	ecb_test();

	return 0;
}