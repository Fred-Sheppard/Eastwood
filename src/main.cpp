#include "ui/windows/login/login.h"
#include "ui/windows/received_dashboard/received_dash.h"
#include "ui/windows/sent_dashboard/sent_dash.h"
#include "./libraries/HTTPSClient.h"
#include "ui/utils/window_manager/window_manager.h"
#include <iostream>
#define SQLITE_HAS_CODEC 1
#include <QFile>
#include <QApplication>
#include <random>

#include "auth/login/login.h"
#include "auth/register_device/register_device.h"
#include "auth/register_user/register_user.h"
#include "auth/login/login.h"
#include "client_api_interactions/MakeAuthReq.h"
#include "src/auth/login/login.h"
#include "database/database.h"
#include "database/schema.h"
#include "endpoints/endpoints.h"
#include "keys/session_token_manager.h"
#include "sql/queries.h"

std::string generateRandomString(int length) {
    // Define the characters you want to include in your random string.
    // This example uses uppercase letters, lowercase letters, and digits.
    const std::string characters =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789";

    // Seed the random number generator.
    // Using std::chrono::system_clock::now().time_since_epoch().count()
    // provides a good, time-based seed.
    std::mt19937 generator(static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    ));

    // Create a distribution to pick random indices from the 'characters' string.
    std::uniform_int_distribution<> distribution(0, characters.length() - 1);

    std::string randomString;
    randomString.reserve(length); // Pre-allocate memory for efficiency

    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    constexpr bool encrypted = false;
    constexpr bool refresh_database = true;

    auto &db = Database::get();
    if (db.initialize("master key", encrypted)) {
        qDebug() << "Database initialized successfully.";
    } else {
        qDebug() << "Failed to initialize database.";
        return 1;
    }

    auto master_password = std::make_unique<std::string>("correct horse battery stapler");

    // TODO: Debugging only
    if (refresh_database) drop_all_tables();

    init_schema();

    const std::string username = generateRandomString(8);
    register_user(username, std::make_unique<std::string>("1234"));
    register_first_device();
    // login_user(username);
    const auto [pk_device, esk_device, nonce_sk] = get_keypair("device");
    const auto sk_device = decrypt_secret_key(esk_device, nonce_sk);

    const auto nonce = post_request_login(username, q_byte_array_to_chars(pk_device));
    unsigned char signature[crypto_sign_BYTES];
    crypto_sign_detached(signature, nullptr, nonce.data(), nonce.size(), sk_device->data());
    const std::string token = post_authenticate(username, q_byte_array_to_chars(pk_device), signature);
    SessionTokenManager::instance().setToken(token);

    get("/keybundle/13891E8B36D170680E0BAB77A3DC2A23BA9A381A1E309C8383D27768D82A4251");
    std::string jsonString =
            "{\n"
            "    \"signedpre_key\": \"830432F90957752ECCD8B9C4708B813E26C9AEAB2D24ADDCF1A44255D5C1E9E8\",\n"
            "    \"signedpk_signature\": \"CD5D645E93CDAEC619F105ACAD980904116558D699346B716594F5638FB840550E3AC1BED1F5CEAF3D98ED6A92B010000E668AFA991640C4F45FAA133D4CE303\",\n"
            "    \"one_time_keys\": []\n"
            "}";
    post(json::parse(jsonString), "/updateKeybundle");

    // TODO - This will go in login_user
    // auto kek = decrypt_kek(
    //     reinterpret_cast<unsigned char *>(encrypted_kek.data()),
    //     reinterpret_cast<unsigned char *>(nonce.data()),
    //     std::move(master_key)
    // );
    // KekManager::instance().setKEK(std::move(kek));

    // WindowManager::instance().showLogin();
    return app.exec();
}
