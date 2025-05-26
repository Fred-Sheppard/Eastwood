#include <sodium.h>
#include <iostream>
#include "src/libraries/HTTPSClient.h"
#include "../utils/ConversionUtils.h"

std::string post_unauth(const std::string& data, const std::string& endpoint = "/") {

    std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        return "";
    }
    const std::string API_PATH = API_HOST + endpoint;

    std::string headers = "Content-Type: application/json\n";

    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.post(API_HOST, API_PATH, headers, data);

    return response;
}

std::string get_unauth(const std::string& endpoint = "/") {

    std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        return "";
    }
    const std::string API_PATH = API_HOST + endpoint;

    std::string headers = "";
    
    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.get(API_HOST, API_PATH, headers);

    return response;
}