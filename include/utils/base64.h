#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <string>

std::string base64_encode(const std::string &input);

std::string base64_decode(const std::string &input);

#endif /* CRYPTO_H_ */