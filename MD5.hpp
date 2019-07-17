#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cmath>

const int N = 64;

class MD5
{
public:
    MD5()
        :chunk_byte_(N)
    {
        a_ = 0x67452301;
        b_ = 0xefcdab89;
        c_ = 0x98badcfe;
        d_ = 0x10325476; 

        size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,  
                     5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,  
                     4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,  
                     6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21  };
        memcpy(shf_, s, sizeof(s));

        for (size_t i = 0; i < 64; i++)
        {
            k_[i] = pow(2.0, 32) * abs(sin(i + 1.0));
        }

        memset(chunk_, 0, chunk_byte_);
        lastByte_ = totalByte_ = 0;
    }


    std::string getFileMD5(const char* filename)
    {
        std::ifstream fin(filename, std::ifstream::binary);
        if (fin.is_open())
        {
            while (!fin.eof())
            {
                fin.read((char *)chunk_, chunk_byte_);
                if (chunk_byte_ != (const size_t)fin.gcount())
                {
                    // 已经读到最后一块数据了
                    break;
                }

                totalByte_ += chunk_byte_;
                calculateMD5((size_t *)chunk_);
            }

            // 循环结束说明读到最后一块数据
            lastByte_ = fin.gcount();
            totalByte_ += lastByte_;
            calculateMD5Final();
        }

        return changeHex(a_) + changeHex(b_) + changeHex(c_) + changeHex(d_);
    }

    std::string getStringMD5(const std::string& str)
    {
        if (str.empty())
        {
            return "";
        }
        
        unsigned char *pstr = (unsigned char*)str.c_str();
        size_t numChunk = str.size() / chunk_byte_;
        for (size_t i = 0; i < numChunk; i++)
        {
            totalByte_ += chunk_byte_;
            calculateMD5((size_t *)pstr + i * chunk_byte_);
        }

        lastByte_ = str.size() % chunk_byte_;
        memcpy(chunk_, pstr + totalByte_, lastByte_);
        calculateMD5Final();

        return changeHex(a_) +changeHex(b_) + changeHex(c_) + changeHex(d_);
    }
private:
    inline size_t F(size_t b, size_t c, size_t d)
    {
        return (b & c) | ((~b) & d);
    }
    
    inline size_t G(size_t b, size_t c, size_t d)
    {
        return (b & d) | ( c & (~d));
    }
    
    inline size_t H(size_t b, size_t c, size_t d)
    {
        return b ^ c ^ d;
    }

    inline size_t I(size_t b, size_t c, size_t d)
    {
        return c ^ (b | (~d));
    }

    inline size_t shiftLeftRotate(size_t num, size_t n)
    {
        return (num << n) | (num >> (32 - n));
    }


    // 最后一块数据拼接
    void calculateMD5Final()
    {
        // lastByte_ < 64Byte
        // 表示最后一块数据的大小
        unsigned char *p = chunk_ + lastByte_;
        // 填充位前 8 位：1000 0000  0x80
        *p++ = 0x80;
        
        // 剩余多少个字节需要填充
        size_t remainFillByte = chunk_byte_ - lastByte_ - 1;
        if (remainFillByte < 8)
        {
            memset(p, 0, remainFillByte);
            calculateMD5((size_t *)chunk_);
            memset(chunk_, 0, chunk_byte_);
        }
        else
        {
            memset(p, 0, remainFillByte);

        }
        // 最后 64 bit 存放原始文档的大小
        ((unsigned long long *)chunk_)[7] = totalByte_ * 8;
    }

    void calculateMD5(size_t *chunk)
    {
        // 获取 A B C D 当前值
        size_t a = a_;
        size_t b = b_;
        size_t c = c_;
        size_t d = d_;

        //TODO 感觉这个要放在构造函数里

        // 哈希函数的返回值
        size_t f;
        // chunk[g]
        size_t g;

        // 64 次变换： 4 轮操作； 每一轮操作分为 16 次子操作
        for (size_t i = 0; i < 64; i++)
        {
            if (i >= 0  && i < 16)
            {
                f = F(b, c, d);
                g = i;
            }
            else if (i < 32)
            {
                f = G(b, c, d);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = H(b, c, d);
                g = (3 * i + 5) % 16;
            }
            else if (i < 64)
            {
                f = I(b, c, d);
                g = (7 * i) % 16;
            }

            size_t dtmp = d;
            d = c;
            c = b;
            b = b + shiftLeftRotate(a + f + k_[i] + chunk[g], shf_[i]);
            a = dtmp;
        }

        // 最后更新 A B C D
        a_ += a;
        b_ += b;
        c_ += c;
        d_ += d;
    }


    // 把整型转换成16进制字符串
    std::string changeHex(size_t num)
    {
        static std::string strMap = "0123456789abcdef";
        std::string ret;
        std::string byteStr;
        
        for (int i = 0; i < 4; i++)
        {
            size_t b = num >> (i * 8) & 0xff;
            for (int j = 0; j < 2; j++)
            {
                byteStr.insert(0, 1, strMap[b % 16]);
                b /= 16;
            }
            ret += byteStr;
        }

        return ret;
    }
private:
    size_t a_;
    size_t b_;
    size_t c_;
    size_t d_;

    size_t k_[N];

    // 循环移位
    size_t shf_[N];  


    // 这是一个定值
    // 表示每个块的大小-> 512
    const size_t chunk_byte_;

    // 这个数组分成四个部分，16 个一组。对一个 512 比特的数据块进行 4 字节为单位的划分
    // 每组都保存着 某一块的编号
    unsigned char chunk_[N];

    size_t lastByte_;

    // 原文档的长度
    unsigned long long totalByte_;
};
