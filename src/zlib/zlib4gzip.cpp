#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#include "zlib.h"
#include "zconf.h"

#pragma comment(lib, "zlib.lib")

#define CHUNK  16384

bool UnGzip(const std::string& source, std::string& dst)
{
	int ret = Z_OK;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];
	memset(out, 0, CHUNK);

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit2(&strm, 47);
	if (ret != Z_OK)
		return false;

	Bytef* bt_source = reinterpret_cast<Bytef*>(const_cast<char*>(source.c_str()));
	uLong tsz = source.size() / sizeof(char) * sizeof(Bytef);
	uLong flag = 0;
	do 
	{
		strm.avail_in = tsz - flag > CHUNK ? CHUNK : tsz - flag;
		
		memset(in, 0, strm.avail_in);
		memcpy(in, bt_source + flag, strm.avail_in);
		flag += strm.avail_in;
		strm.next_in = in;
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;

			ret = inflate(&strm, Z_NO_FLUSH);
			if (ret == Z_STREAM_ERROR)
				return false;

			if (ret != Z_OK && ret != Z_STREAM_END)
			{
				return false;
			}
			have = CHUNK - strm.avail_out;
			char* buf = reinterpret_cast<char*>(out);
			dst.insert(dst.end(), buf, buf + have);
			memset(out, 0, CHUNK);

		} while (strm.avail_out == 0);
		if(flag == tsz)
			break;
	} while (ret != Z_STREAM_END);

	(void)inflateEnd(&strm);

	return true;
}


bool GZip(const std::string& content, std::string& str)
{
	int ret, flh;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION,Z_DEFLATED, 31,8,Z_DEFAULT_STRATEGY);
	if (ret != Z_OK)
		return false;

	Bytef* buf = reinterpret_cast<Bytef*>(const_cast<char*>(content.c_str()));
	uLong tsz = content.size();
	uLong flag = 0;

	do {		
		strm.avail_in = tsz - flag > CHUNK ? CHUNK : tsz - flag;
		flh = strm.avail_in == CHUNK ? Z_NO_FLUSH : Z_FINISH;
		memset(in, 0, strm.avail_in);
		memcpy(in, buf + flag, strm.avail_in);
		flag += strm.avail_in;
		strm.next_in = in;

		do 
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;

			ret = deflate(&strm, flh);
			if (ret == Z_STREAM_ERROR)
			{
				return false;
			}
			if (ret != Z_OK && ret != Z_STREAM_END)
			{
				return false;
			}

			have = CHUNK - strm.avail_out;

			char* dst = reinterpret_cast<char*>(out);
			str.insert(str.end(), dst, dst + have);

		} while (strm.avail_out == 0);
		if (flag == tsz)
		{
			break;
		}

	} while (ret != Z_STREAM_END);

	(void)deflateEnd(&strm);

	return true;
}


int main()
{
	std::string stm, dst;
	//std::string str_path = "e:/1.txt.gz";
	//std::string str_path = "e:/1.zip";
	//std::string str_path = "e:/4.gz";

	std::string str_path = "e:/1.jpg.gz";
	FILE* f = fopen(str_path.c_str(), "rb");
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* pbuf = new char[sz];
	fread(pbuf, 1, sz, f);
	fclose(f);

	std::string s(pbuf, pbuf + sz);
	

	bool b_ok = false;
	b_ok = UnGzip(s, dst);

	if (b_ok)
	{
		fstream fs;
		fs.open("e:/fdd2.jpg", std::ios::out | std::ios::binary);
		fs.write(dst.c_str(), dst.size());
		fs.close();
	}


	ifstream fs("e:/11.jpg", std::ios::binary);
	std::istreambuf_iterator<char> beg(fs), end;
	std::string str_content(beg, end);
	fs.close();
	
	std::string str_zip;
	b_ok = GZip(str_content, str_zip);

	if (b_ok)
	{
		fstream ff;
		ff.open("e:/11_.jpg.gz", std::ios::binary | std::ios::out);
		ff.write(str_zip.c_str(), str_zip.size());
		ff.close();
	}

	return 0;
}