#ifndef MAKE_UNAUTH_REQ_H
#define MAKE_UNAUTH_REQ_H

#include <string>

std::string post_unauth(const std::string& data, const std::string& endpoint = "/");
std::string get_unauth(const std::string& endpoint = "/");

#endif // MAKE_UNAUTH_REQ_H