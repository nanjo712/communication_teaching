#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/types.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/serial_port.hpp>
#include <cstring>
#include <filesystem>
#include <iostream>

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

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

void show_usage()
{
    std::cout << "Usage:\n";
    std::cout << "  help                - Display this help message\n";
    std::cout << "  encrypt <content>   - Encrypt the provided content\n";
    std::cout << "  decrypt <content>   - Decrypt the provided content\n";
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

int main(int argc, char *argv[])
{
    if (argc < 2 || argv[1] == NULL)
    {
        std::cout << "Please give a seria name\n";
    }
    std::string port_name(argv[1]);

    auto private_key_file = fopen(private_key_file_name.c_str(), "r");
    auto public_key_file = fopen(public_key_file_name.c_str(), "r");

    if (!std::filesystem::exists(private_key_file_name) ||
        !std::filesystem::exists(public_key_file_name))
    {
        std::cout << "Cannot find RSA key pair files. Generating new ones...\n";
        generate_rsa_keypair(2048, private_key_file_name.c_str(),
                             public_key_file_name.c_str());
    }

    EVP_PKEY *private_key = NULL;
    EVP_PKEY *public_key = NULL;

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

    std::string buffer;
    while (1)
    {
        serial.read_some(boost::asio::buffer(buffer));
        if (buffer == "help")
        {
            show_usage();
        }
        else if (buffer.substr(0, 7) == "encrypt")
        {
            std::string content = buffer.substr(8);
            if (content.empty())
            {
                std::cout << "Error: No content provided for encryption.\n";
            }
            else
            {
                std::cout << encrypt(public_key, content) << std::endl;
            }
        }
        else if (buffer.substr(0, 7) == "decrypt")
        {
            std::string content = buffer.substr(8);
            if (content.empty())
            {
                std::cout << "Error: No content provided for decryption.\n";
            }
            else
            {
                std::cout << decrypt(private_key, content) << std::endl;
            }
        }
        else
        {
            std::cout << "Unknown command. Type 'help' for usage.\n";
        }

        return 0;
    }
}