#include "utils/base64.h"

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

std::string base64_encode(const std::string &input)
{
    // 创建 BIO
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // 设置不换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // 编写数据
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);

    // 获取编码结果
    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    // 返回编码字符串
    return std::string(bufferPtr->data, bufferPtr->length);
}

std::string base64_decode(const std::string &input)
{
    // 创建 BIO
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new_mem_buf(input.data(), input.size());
    bio = BIO_push(b64, bio);

    // 设置不换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // 解码数据
    std::string output;
    char buffer[512];  // 临时缓冲区
    int decoded_length;

    while ((decoded_length = BIO_read(bio, buffer, sizeof(buffer))) > 0)
    {
        output.append(buffer, decoded_length);
    }

    BIO_free_all(bio);  // 释放 BIO

    return output;  // 返回解码后的字符串
}