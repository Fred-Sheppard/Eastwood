#include <sodium.h>
#include <iostream>
#include "src/libraries/HTTPSClient.h"
#include "../utils/ConversionUtils.h"
#include "../utils/JsonParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string post_unauth(const json& data, const std::string& endpoint = "/") {

    std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        return "";
    }

    std::string headers = "Content-Type: application/json\n";

    webwood::HTTPSClient httpsclient;
    std::string request_body = data.dump();
    std::string response = httpsclient.post(API_HOST, endpoint, headers, request_body);

    try {
        nlohmann::json parsed_response = webwood::parse_json_response(response);
        return parsed_response.dump();
    } catch (const webwood::HttpError& e) {
        std::cerr << "HTTP Error: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing response: " << e.what() << std::endl;
        throw;
    }
}

std::string get_unauth(const std::string& endpoint = "/") {

    std::string API_HOST = load_env_variable("API_HOST");
    if (API_HOST.empty()) {
        std::cerr << "API_HOST not found in .env file" << std::endl;
        return "";
    }

    std::string headers;
    
    webwood::HTTPSClient httpsclient;
    std::string response = httpsclient.get(API_HOST, endpoint, headers);

    try {
        nlohmann::json parsed_response = webwood::parse_json_response(response);
        return parsed_response.dump();
    } catch (const webwood::HttpError& e) {
        std::cerr << "HTTP Error: " << e.what() << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing response: " << e.what() << std::endl;
        throw;
    }
}