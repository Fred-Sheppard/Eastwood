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
#include "key_exchange/utils.h"
#include "sessions/IdentityManager.h"
#include "sessions/IdentitySession.h"
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
    // Test generate_unique_id_pair
    std::string input1 = "test1";
    std::string input2 = "test2";
    unsigned char *result = generate_unique_id_pair(&input1, &input2);
    
    std::cout << "Testing generate_unique_id_pair:" << std::endl;
    std::cout << "Input 1: " << input1 << std::endl;
    std::cout << "Input 2: " << input2 << std::endl;
    std::cout << "Result hex: ";
    for (size_t i = 0; i < crypto_hash_sha256_BYTES; i++) {
        printf("%02x", result[i]);
    }
    std::cout << std::endl;
    std::cout << "Result length: " << crypto_hash_sha256_BYTES << " bytes" << std::endl;
    delete[] result;  // Clean up the result buffer
    
    std::cout << "\nStarting main application...\n" << std::endl;

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

    register_user("sloggotesting35", std::make_unique<std::string>("1234"));
    register_first_device();
    login_user("sloggotesting35");
    post_new_keybundles(
        get_decrypted_keypair("device"),
        generate_signed_prekey(),
        generate_onetime_keys(100)
    );

    std::cout << "Press Enter to continue...";
    std::cin.get();

    auto backlog = get_handshake_backlog();
    IdentityManager::getInstance().update_or_create_identity_sessions(backlog);

    std::cout << "Press Enter to continue...";
    std::cin.get();

    auto msg = new unsigned char[5];
    randombytes_buf(msg, 5);
    std::cout << "message" << bin2hex(msg, 5) << std::endl;

    IdentityManager::getInstance().send_to_user("nialltesting35", msg);
    delete[] msg;  // Clean up the message buffer after sending
    // WindowManager::instance().showLogin();
    return app.exec();
}