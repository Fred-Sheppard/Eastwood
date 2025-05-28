#include "JsonParser.h"

namespace webwood {

int parse_status_code(const std::string& first_line) {
    size_t first_space = first_line.find(' ');
    if (first_space == std::string::npos) {
        return 0;
    }
    
    size_t second_space = first_line.find(' ', first_space + 1);
    if (second_space == std::string::npos) {
        return 0;
    }
    
    try {
        return std::stoi(first_line.substr(first_space + 1, second_space - first_space - 1));
    } catch (const std::exception&) {
        return 0;
    }
}

nlohmann::json parse_json_response(const std::string& response) {
    if (response.empty()) {
        return {{"data", ""}, {"content_type", "text"}, {"status_code", 0}};
    }

    // Extract status code from the first line
    size_t first_line_end = response.find("\r\n");
    if (first_line_end == std::string::npos) {
        first_line_end = response.find("\n");
    }
    if (first_line_end == std::string::npos) {
        return {{"data", response}, {"content_type", "text"}, {"status_code", 0}};
    }

    std::string first_line = response.substr(0, first_line_end);
    int status_code = parse_status_code(first_line);

    size_t json_start = response.find("\r\n\r\n");
    if (json_start == std::string::npos) {
        json_start = response.find("\n\n");
        if (json_start == std::string::npos) {
            return {{"data", response}, {"content_type", "text"}, {"status_code", status_code}};
        }
        json_start += 2;
    } else {
        json_start += 4;
    }

    // Extract and parse the JSON body
    try {
        nlohmann::json body_json = nlohmann::json::parse(response.substr(json_start));
        return {{"data", body_json}, {"content_type", "json"}, {"status_code", status_code}};
    } catch (const nlohmann::json::parse_error&) {
        return {{"data", response.substr(json_start)}, {"content_type", "text"}, {"status_code", status_code}};
    }
}

}