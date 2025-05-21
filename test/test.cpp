#include <iostream>
#include <string>
#include "../include/HTTPSClient.h"

int main() {
    webwood::HTTPSClient https_client;
    std::string res = https_client.get("untrusted-root.badssl.com", "/");
    std::cout << res << std::endl;
    return 0;
}