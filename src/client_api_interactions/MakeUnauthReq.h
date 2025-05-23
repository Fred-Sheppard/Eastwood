#ifndef MAKE_UNAUTH_REQ_H
#define MAKE_UNAUTH_REQ_H

#include <string>

std::string post_unauthenticated(const std::string& data, const std::string& endpoint = "/");
std::string get_unauthenticated(const std::string& endpoint = "/");

#endif // MAKE_UNAUTH_REQ_H