#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

std::string flag = {"R29vZCBIZWF2ZW4hIFlvdSBoYXZlIGZpbmlzaGVkIGl0IQ=="};

EVP_PKEY *load_public_key(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        std::cerr << "Unable to open public key file.\n";
        return nullptr;
    }
    EVP_PKEY *pkey = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    if (!pkey)
    {
        handleErrors();
    }
    return pkey;
}

EVP_PKEY *load_private_key(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        std::cerr << "Unable to open private key file.\n";
        return nullptr;
    }
    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    if (!pkey)
    {
        handleErrors();
    }
    return pkey;
}

void generate_rsa_keypair(int key_length, const char *private_key_file,
                          const char *public_key_file)
{
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;

    // 创建上下文
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx)
    {
        fprintf(stderr, "Error creating context: %s\n",
                ERR_reason_error_string(ERR_get_error()));
        return;
    }

    // 生成密钥对
    if (EVP_PKEY_keygen_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_length) <= 0 ||
        EVP_PKEY_keygen(ctx, &pkey) <= 0)
    {
        fprintf(stderr, "Error generating RSA key pair: %s\n",
                ERR_reason_error_string(ERR_get_error()));
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    // 输出私钥到文件
    FILE *private_key_fp = fopen(private_key_file, "wb");
    if (!private_key_fp)
    {
        fprintf(stderr, "Error opening private key file for writing.\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return;
    }
    if (!PEM_write_PrivateKey(private_key_fp, pkey, NULL, NULL, 0, NULL, NULL))
    {
        fprintf(stderr, "Error writing private key to file: %s\n",
                ERR_reason_error_string(ERR_get_error()));
        fclose(private_key_fp);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return;
    }
    fclose(private_key_fp);

    // 输出公钥到文件
    FILE *public_key_fp = fopen(public_key_file, "wb");
    if (!public_key_fp)
    {
        fprintf(stderr, "Error opening public key file for writing.\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return;
    }
    if (!PEM_write_PUBKEY(public_key_fp, pkey))
    {
        fprintf(stderr, "Error writing public key to file: %s\n",
                ERR_reason_error_string(ERR_get_error()));
        fclose(public_key_fp);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return;
    }
    fclose(public_key_fp);

    // 清理
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    printf("RSA key pair generated and saved to files successfully.\n");
}

std::string private_key_file_name = "private.pem";
std::string public_key_file_name = "public.pem";

std::string show_usage()
{
    std::stringstream ss;
    ss << "Usage:\n";
    ss << "  help                - Display this help message\n";
    ss << "  encrypt <content>   - Encrypt the provided content\n";
    ss << "  decrypt <content>   - Decrypt the provided content\n";
    return ss.str();
}

std::string encrypt(EVP_PKEY *public_key, const std::string &content)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(public_key, nullptr);
    if (!ctx) handleErrors();

    if (EVP_PKEY_encrypt_init(ctx) <= 0) handleErrors();

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen,
                         (const unsigned char *)content.c_str(),
                         content.size()) <= 0)
        handleErrors();

    std::string encrypted(outlen, '\0');
    if (EVP_PKEY_encrypt(ctx, (unsigned char *)encrypted.data(), &outlen,
                         (const unsigned char *)content.c_str(),
                         content.size()) <= 0)
        handleErrors();

    EVP_PKEY_CTX_free(ctx);
    encrypted.resize(outlen);  // Resize to actual length
    return encrypted;
}

std::string decrypt(EVP_PKEY *private_key, const std::string &encrypted)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(private_key, nullptr);
    if (!ctx) handleErrors();

    if (EVP_PKEY_decrypt_init(ctx) <= 0) handleErrors();

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen,
                         (const unsigned char *)encrypted.c_str(),
                         encrypted.size()) <= 0)
        handleErrors();

    std::string decrypted(outlen, '\0');
    if (EVP_PKEY_decrypt(ctx, (unsigned char *)decrypted.data(), &outlen,
                         (const unsigned char *)encrypted.c_str(),
                         encrypted.size()) <= 0)
        handleErrors();

    EVP_PKEY_CTX_free(ctx);
    decrypted.resize(outlen);  // Resize to actual length
    return decrypted;
}

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

static EVP_PKEY *private_key = NULL;
static EVP_PKEY *public_key = NULL;

std::string command_handler(const std::string &command)
{
    std::cout << "Received: " << command << std::endl;
    if (command == "help")
    {
        return show_usage();
    }
    else if (command.substr(0, 7) == "encrypt")
    {
        std::string content = command.substr(8);
        if (content.empty())
        {
            return "Error: No content provided for encryption.\n";
        }
        else
        {
            return base64_encode(encrypt(public_key,
                                         content + " " + base64_decode(flag))) +
                   "\n";
        }
    }
    else if (command.substr(0, 7) == "decrypt")
    {
        std::string content = command.substr(8);
        if (content.empty())
        {
            return "Error: No content provided for decryption.\n";
        }
        else
        {
            return decrypt(private_key, base64_decode(content)) + "\n";
        }
    }
    else
    {
        return "Error: Unknown command.\n";
    }
    return "";
}

int main(int argc, char *argv[])
{
    std::string port_name = "/dev/ttyUSB0";

    auto private_key_file = fopen(private_key_file_name.c_str(), "r");
    auto public_key_file = fopen(public_key_file_name.c_str(), "r");

    if (!std::filesystem::exists(private_key_file_name) ||
        !std::filesystem::exists(public_key_file_name))
    {
        std::cout << "Cannot find RSA key pair files. Generating new ones...\n";
        generate_rsa_keypair(2048, private_key_file_name.c_str(),
                             public_key_file_name.c_str());
    }

    private_key = load_private_key(private_key_file_name.c_str());
    public_key = load_public_key(public_key_file_name.c_str());

    boost::asio::io_context io_context;
    boost::asio::serial_port serial(io_context, port_name);

    serial.set_option(boost::asio::serial_port::baud_rate(115200));
    serial.set_option(boost::asio::serial_port::character_size(8));
    serial.set_option(boost::asio::serial_port::stop_bits(
        boost::asio::serial_port::stop_bits::one));
    serial.set_option(boost::asio::serial_port::parity(
        boost::asio::serial_port::parity::none));
    serial.set_option(boost::asio::serial_port::flow_control(
        boost::asio::serial_port::flow_control::none));

    if (!serial.is_open()) return 1;

    std::array<char, 1024> buffer;
    while (1)
    {
        auto size = serial.read_some(boost::asio::buffer(buffer));
        std::string command(buffer.data(), size);
        std::string response = command_handler(command);
        serial.write_some(boost::asio::buffer(response));
    }
}