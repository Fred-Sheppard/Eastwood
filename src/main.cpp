#include <QApplication>
#include "ui/windows/login/login.h"
#include "../include/HTTPSClient.h"

int main(int argc, char *argv[]) {
    webwood::HTTPSClient httpclient;
    std::string res = httpclient.get("badssl.com", "/");
    QApplication app(argc, argv);
    Login login;
    login.show();
    return app.exec();
}
