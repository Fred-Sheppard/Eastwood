#include <QApplication>
#include "ui/windows/login/login.h"
#include "./libraries/HTTPSClient.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "./client_api_interactions/MakeAuthReq.h"
#include "./client_api_interactions/MakeUnauthReq.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    
    webwood::HTTPSClient httpclient;
    // std::string headers = "User-Agent: NiallClient/1.0\nAuthorization: Bearer abc123";
    // std::string body = "bod: bod";
    // std::string res = httpclient.post("webhook.site", "/86b5bc32-daa9-4f09-88eb-c658b71ae426", headers, body );
    // std::cout << res << std::endl;
    // json test_data = {
    //     {"message", "Hi there"}
    // };
    // std::string res = post_auth(test_data, "/");
    // std::cout << "res: " << res << std::endl;
    // std::string res2 = post_unauth(test_data, "/86b5bc32-daa9-4f09-88eb-c658b71ae426");
    // std::cout << "res2: " << res2 << std::endl;
    // std::string res3 = get_auth("/");
    // std::cout << "res3: " << res3 << std::endl;
    // std::string res4 = get_unauth("/");
    // std::cout << "res4: " << res4 << std::endl;
    // std::cout << res << std::endl;
    QApplication app(argc, argv);
    Login login;
    login.show();
    return app.exec();
}
