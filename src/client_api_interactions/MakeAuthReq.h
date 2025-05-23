#ifndef MAKE_AUTH_REQ_H
#define MAKE_AUTH_REQ_H

#include <string>

std::string post_auth(const std::string& data, const std::string& endpoint = "/");
std::string get_auth(const std::string& endpoint = "/");

#endif // MAKE_AUTH_REQ_H