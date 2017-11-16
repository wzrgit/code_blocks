
#include "openssl/evp.h"
#include <iostream>
#include <string>
#include <cassert>

#include <fstream>

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

unsigned char key[16] = {};
const char* content = "一文中，使用了树莓派来做直播服务器。通过安装在树莓派上的摄像头采集实时视频数据流，推送至 RTMP 监听服务器。同时，其他的客户端访问这个 RTMP 服务器就可以观看视频了。下面我们来详细介绍这个模块如何搭建，文末还将给出将直播视频推送至斗鱼直播平台的方法。";

std::string Enc2(std::string content)
{	std::string str;
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	int ret = EVP_EncryptInit(&ctx, EVP_aes_256_ecb(), key, NULL);

	int sz = content.size() + EVP_CIPHER_CTX_block_size(&ctx);
	unsigned char* out_buf = new unsigned char[sz];
	int out_sz = 0;
	ret = EVP_EncryptUpdate(&ctx, out_buf, &out_sz, reinterpret_cast<const unsigned char*>(content.c_str()), content.length());
	int out_sz2 = 0;
	ret = EVP_EncryptFinal(&ctx, out_buf + out_sz, &out_sz2);
	
	str.insert(str.end(), out_buf, out_buf + out_sz + out_sz2);
	delete[] out_buf;

	return str;
}


std::string Dec2(std::string content)
{
	std::string str;
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	int ret = EVP_DecryptInit(&ctx, EVP_aes_256_ecb(), key, NULL);
	int sz = content.size() + EVP_CIPHER_CTX_block_size(&ctx);
	unsigned char* out_buf = new unsigned char[sz];

	const unsigned char* in_buf = reinterpret_cast<const unsigned char*>(content.c_str());

	int flag = 0;
	int flag_out = 0;
	int bksz = 53;
	do
	{
		int out_sz = 0;
		if (flag + bksz > content.length())
			bksz = content.length() - flag;
		ret = EVP_DecryptUpdate(&ctx, out_buf + flag_out, &out_sz, in_buf + flag,  bksz);
		flag += bksz;
		flag_out += out_sz;
	} while (flag < content.length());

	int out_sz2 = 0;
	ret = EVP_DecryptFinal(&ctx, out_buf + flag_out, &out_sz2);

	str.insert(str.end(), reinterpret_cast<char*>(out_buf), reinterpret_cast<char*>(out_buf) + flag_out + out_sz2);
	delete[] out_buf;

	return str;
}



std::string Enc(std::string content)
{
	std::string str;

	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-ecb");
	if (!cipher)
	{
		cipher = EVP_aes_256_ecb();
	}
	int ret = EVP_CipherInit(&ctx, cipher, key, NULL, 1);
	int sz = EVP_CIPHER_CTX_block_size(&ctx);
	int in_size = content.length();
	sz += in_size;

	unsigned char* outbuf = new unsigned char[sz];


	ret = EVP_CipherUpdate(&ctx, outbuf, &sz, reinterpret_cast<const unsigned char*>(content.c_str()), in_size);
	str.insert(str.end(), outbuf, outbuf + sz);

	sz = EVP_CIPHER_CTX_block_size(&ctx);
	unsigned char* outbuf_fianl = new unsigned char[sz];
	ret = EVP_CipherFinal(&ctx, outbuf_fianl, &sz);

	str.insert(str.end(), outbuf, outbuf + sz);

	return str;
}

std::string Dec(std::string content)
{
	std::string str;

	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	const EVP_CIPHER* cipher = EVP_get_cipherbyname("aes-256-ecb");
	if (!cipher)
	{
		cipher = EVP_aes_256_ecb();
	}
	int ret = EVP_CipherInit(&ctx, cipher, key, NULL, 0);
	int sz = EVP_CIPHER_CTX_block_size(&ctx);	
	int in_size = content.length();
	sz += in_size;

	unsigned char* outbuf = new unsigned char[sz];

	sz = 0;
	ret = EVP_CipherUpdate(&ctx, outbuf, &sz, reinterpret_cast<const unsigned char*>(content.c_str()), in_size);
	str.insert(str.end(), outbuf, outbuf + sz);

	int sz2 = 0;

	//TODO  something error here
	ret = EVP_CipherFinal_ex(&ctx, outbuf + sz, &sz);

	str.insert(str.end(), outbuf, outbuf + sz);

	return str;
}

int main()
{
	for (int i = 0; i < 16; i++)
	{
		key[i] = i;
	}

	std::cout << content << std::endl;
	std::string str = Enc2(content);
	std::string str2 = Dec2(str);
	std::cout << str2 << std::endl;
	assert(strcmp(str2.c_str(), content) == 0);

	std::string txt = Enc(content);
	//assert(strcmp(str.c_str(), txt.c_str()) == 0);

	std::string txt2 = Dec(str);
	//assert(strcmp(txt2.c_str(), content) == 0);



	return 0;
}