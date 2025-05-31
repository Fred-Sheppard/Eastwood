//
// Created by Josh Sloggett on 31/05/2025.
//
//
// Created by Josh Sloggett on 30/05/2025.
//

#include <gtest/gtest.h>
#include <tuple>

#include "kek_manager.h"
#include "NewRatchet.h"
#include "utils.h"
#include "database/schema.h"
#include "client_api_interactions/MakeAuthReq.h"
#include "libraries/BaseClient.h"
#include "libraries/HTTPSClient.h"
#include "endpoints/endpoints.h"
#include "keys/secure_memory_buffer.h"
#include "algorithms/algorithms.h"
#include "database/database.h"
#include "sessions/KeyBundle.h"
#include "sessions/IdentityManager.h"

class IdentitySessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (sodium_init() < 0) {
            throw std::runtime_error("Libsodium initialization failed");
        }

        auto kek = SecureMemoryBuffer::create(SYM_KEY_LEN);
        crypto_secretbox_keygen(kek->data());

        KekManager::instance().setKEK(std::move(kek));

        bool encrypted = false;

        const auto master_key = SecureMemoryBuffer::create(MASTER_KEY_LEN);
        randombytes_buf(master_key->data(), MASTER_KEY_LEN);
        Database::get().initialize("username", master_key, encrypted);

        auto master_password = std::make_unique<std::string>("correct horse battery stapler");
        drop_all_tables();
        init_schema();

        // Initialize SecureMemoryBuffer objects with correct sizes for all users
        // Alice's keys
        alice_device_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        alice_to_bob_eph_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        alice_to_charlie_eph_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        
        // Bob's keys
        bob_device_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        bob_presign_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        bob_onetime_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        
        // Charlie's keys
        charlie_device_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        charlie_eph_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        charlie_presign_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        charlie_onetime_priv = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);

        // Generate keypairs for all users
        // Alice's keypairs
        crypto_sign_keypair(alice_device_pub, alice_device_priv->data());
        crypto_box_keypair(alice_to_bob_eph_pub, alice_to_bob_eph_priv->data());
        crypto_box_keypair(alice_to_charlie_eph_pub, alice_to_charlie_eph_priv->data());

        // Bob's keypairs
        crypto_sign_keypair(bob_device_pub, bob_device_priv->data());
        crypto_box_keypair(bob_presign_pub, bob_presign_priv->data());
        crypto_box_keypair(bob_onetime_pub, bob_onetime_priv->data());

        // Charlie's keypairs
        crypto_sign_keypair(charlie_device_pub, charlie_device_priv->data());
        crypto_box_keypair(charlie_eph_pub, charlie_eph_priv->data());
        crypto_box_keypair(charlie_presign_pub, charlie_presign_priv->data());
        crypto_box_keypair(charlie_onetime_pub, charlie_onetime_priv->data());

        // Generate signatures
        crypto_sign_detached(bob_presign_signature,
            nullptr,
            bob_presign_pub,
            crypto_box_PUBLICKEYBYTES,
            bob_device_priv->data()
        );

        crypto_sign_detached(charlie_presign_signature,
            nullptr,
            charlie_presign_pub,
            crypto_box_PUBLICKEYBYTES,
            charlie_device_priv->data()
        );

        // Create shared pointers for ephemeral private keys
        auto alice_to_bob_eph_priv_copy = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        memcpy(alice_to_bob_eph_priv_copy->data(), alice_to_bob_eph_priv->data(), crypto_sign_SECRETKEYBYTES);
        std::shared_ptr<SecureMemoryBuffer> shared_alice_to_bob_eph_priv = std::shared_ptr<SecureMemoryBuffer>(alice_to_bob_eph_priv_copy.release());

        auto alice_to_charlie_eph_priv_copy = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        memcpy(alice_to_charlie_eph_priv_copy->data(), alice_to_charlie_eph_priv->data(), crypto_sign_SECRETKEYBYTES);
        std::shared_ptr<SecureMemoryBuffer> shared_alice_to_charlie_eph_priv = std::shared_ptr<SecureMemoryBuffer>(alice_to_charlie_eph_priv_copy.release());

        auto charlie_eph_priv_copy = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        memcpy(charlie_eph_priv_copy->data(), charlie_eph_priv->data(), crypto_sign_SECRETKEYBYTES);
        std::shared_ptr<SecureMemoryBuffer> shared_charlie_eph_priv = std::shared_ptr<SecureMemoryBuffer>(charlie_eph_priv_copy.release());

        // Create key bundles for Alice -> Bob
        alice_to_bob_bundle = new SendingKeyBundle(
            alice_device_pub,
            alice_to_bob_eph_pub,
            std::move(shared_alice_to_bob_eph_priv),
            bob_device_pub,
            bob_presign_pub,
            bob_onetime_pub,
            bob_presign_signature
        );

        bob_from_alice_bundle = new ReceivingKeyBundle(
            alice_device_pub,
            alice_to_bob_eph_pub,
            bob_device_pub,
            bob_onetime_pub
        );

        // Create key bundles for Alice -> Charlie
        alice_to_charlie_bundle = new SendingKeyBundle(
            alice_device_pub,
            alice_to_charlie_eph_pub,
            std::move(shared_alice_to_charlie_eph_priv),
            charlie_device_pub,
            charlie_presign_pub,
            charlie_onetime_pub,
            charlie_presign_signature
        );

        charlie_from_alice_bundle = new ReceivingKeyBundle(
            alice_device_pub,
            alice_to_charlie_eph_pub,
            charlie_device_pub,
            charlie_onetime_pub
        );

        // Create key bundles for Bob -> Charlie
        bob_to_charlie_bundle = new SendingKeyBundle(
            bob_device_pub,
            bob_presign_pub,
            std::move(shared_charlie_eph_priv),
            charlie_device_pub,
            charlie_presign_pub,
            charlie_onetime_pub,
            charlie_presign_signature
        );

        charlie_from_bob_bundle = new ReceivingKeyBundle(
            bob_device_pub,
            bob_presign_pub,
            charlie_device_pub,
            charlie_onetime_pub
        );

        // Initialize databases for all users
        switch_to_alice_db();
        switch_to_bob_db();
        switch_to_charlie_db();
    }

    void TearDown() override {
        // Clean up all key bundles
        if (alice_to_bob_bundle) delete alice_to_bob_bundle;
        if (bob_from_alice_bundle) delete bob_from_alice_bundle;
        if (alice_to_charlie_bundle) delete alice_to_charlie_bundle;
        if (charlie_from_alice_bundle) delete charlie_from_alice_bundle;
        if (bob_to_charlie_bundle) delete bob_to_charlie_bundle;
        if (charlie_from_bob_bundle) delete charlie_from_bob_bundle;

        // Reset database state
        drop_all_tables();
    }

    void switch_to_alice_db() {
        drop_all_tables();
        init_schema();

        auto nonce = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_alice_device_priv = encrypt_secret_key(std::move(alice_device_priv), nonce);
        save_encrypted_keypair("device", alice_device_pub, encrypted_alice_device_priv, nonce);

        delete[] nonce;
    }

    void switch_to_bob_db() {
        drop_all_tables();
        init_schema();

        auto nonce = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_bob_device_priv = encrypt_secret_key(std::move(bob_device_priv), nonce);
        save_encrypted_keypair("device", bob_device_pub, encrypted_bob_device_priv, nonce);

        auto nonce_2 = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce_2, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_bob_presign_priv = encrypt_secret_key(std::move(bob_presign_priv), nonce_2);
        save_encrypted_keypair("signed", bob_presign_pub, encrypted_bob_presign_priv, nonce_2);

        auto nonce_3 = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce_3, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_bob_onetime_priv = encrypt_secret_key(std::move(bob_onetime_priv), nonce_3);

        unsigned char* onetime_pub_copy = new unsigned char[crypto_box_PUBLICKEYBYTES];
        memcpy(onetime_pub_copy, bob_onetime_pub, crypto_box_PUBLICKEYBYTES);

        std::vector<std::tuple<unsigned char*, std::unique_ptr<SecureMemoryBuffer>, unsigned char*>> onetime_keys;
        onetime_keys.emplace_back(onetime_pub_copy, std::move(encrypted_bob_onetime_priv), nonce_3);

        save_encrypted_onetime_keys(std::move(onetime_keys));

        // Clean up the copy
        delete[] onetime_pub_copy;
        delete[] nonce;
    }

    void switch_to_charlie_db() {
        drop_all_tables();
        init_schema();

        auto nonce = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_charlie_device_priv = encrypt_secret_key(std::move(charlie_device_priv), nonce);
        save_encrypted_keypair("device", charlie_device_pub, encrypted_charlie_device_priv, nonce);

        auto nonce_2 = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce_2, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_charlie_presign_priv = encrypt_secret_key(std::move(charlie_presign_priv), nonce_2);
        save_encrypted_keypair("signed", charlie_presign_pub, encrypted_charlie_presign_priv, nonce_2);

        auto nonce_3 = new unsigned char[CHA_CHA_NONCE_LEN];
        randombytes_buf(nonce_3, CHA_CHA_NONCE_LEN);

        std::unique_ptr<SecureMemoryBuffer> encrypted_charlie_onetime_priv = encrypt_secret_key(std::move(charlie_onetime_priv), nonce_3);

        unsigned char* onetime_pub_copy = new unsigned char[crypto_box_PUBLICKEYBYTES];
        memcpy(onetime_pub_copy, charlie_onetime_pub, crypto_box_PUBLICKEYBYTES);

        std::vector<std::tuple<unsigned char*, std::unique_ptr<SecureMemoryBuffer>, unsigned char*>> onetime_keys;
        onetime_keys.emplace_back(onetime_pub_copy, std::move(encrypted_charlie_onetime_priv), nonce_3);

        save_encrypted_onetime_keys(std::move(onetime_keys));

        delete[] onetime_pub_copy;
        delete[] nonce;
        delete[] nonce_2;
        delete[] nonce_3;
    }

    // Key bundles for Alice -> Bob
    SendingKeyBundle *alice_to_bob_bundle = nullptr;
    ReceivingKeyBundle *bob_from_alice_bundle = nullptr;

    // Key bundles for Alice -> Charlie
    SendingKeyBundle *alice_to_charlie_bundle = nullptr;
    ReceivingKeyBundle *charlie_from_alice_bundle = nullptr;

    // Key bundles for Bob -> Charlie
    SendingKeyBundle *bob_to_charlie_bundle = nullptr;
    ReceivingKeyBundle *charlie_from_bob_bundle = nullptr;

    // Alice's keys
    unsigned char alice_device_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> alice_device_priv;
    unsigned char alice_to_bob_eph_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> alice_to_bob_eph_priv;
    unsigned char alice_to_charlie_eph_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> alice_to_charlie_eph_priv;

    // Bob's keys
    unsigned char bob_device_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> bob_device_priv;
    unsigned char bob_presign_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> bob_presign_priv;
    unsigned char bob_onetime_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> bob_onetime_priv;
    unsigned char bob_presign_signature[crypto_sign_BYTES] = {};

    // Charlie's keys
    unsigned char charlie_device_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> charlie_device_priv;
    unsigned char charlie_eph_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> charlie_eph_priv;
    unsigned char charlie_presign_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> charlie_presign_priv;
    unsigned char charlie_onetime_pub[crypto_box_PUBLICKEYBYTES] = {};
    std::unique_ptr<SecureMemoryBuffer> charlie_onetime_priv;
    unsigned char charlie_presign_signature[crypto_sign_BYTES] = {};
};

TEST_F(IdentitySessionTest, MultipleRatchetInitialisationTest) {
    // Initialise identity session
    std::vector<KeyBundle*> bundles;
    bundles.push_back(alice_to_bob_bundle);
    bundles.push_back(alice_to_charlie_bundle);

    std::string alice_username = "alice";
    std::string bob_username = "bob";

    const auto session = std::make_unique<IdentitySession>(
        bundles, generate_unique_id_pair(&alice_username, &bob_username)
    );

    ASSERT_EQ(session->get_ratchets().size(), 2);
    EXPECT_FALSE(session->get_ratchets().empty());

    // Search manually using memcmp
    bool found_first = false;
    bool found_second = false;
    for (const auto& [key, _] : session->get_ratchets()) {
        if (std::memcmp(key.data(), bundles[0]->get_ratchet_id(), 32) == 0)
            found_first = true;
        if (std::memcmp(key.data(), bundles[1]->get_ratchet_id(), 32) == 0)
            found_second = true;
    }

    ASSERT_TRUE(found_first);
    ASSERT_TRUE(found_second);
}

TEST_F(IdentitySessionTest, RatchetGenerationAndDuplicationTest) {
    // Initialize identity session with initial bundles
    std::vector<KeyBundle*> initial_bundles;
    initial_bundles.push_back(alice_to_bob_bundle);
    initial_bundles.push_back(alice_to_charlie_bundle);

    std::string alice_username = "alice";
    std::string bob_username = "bob";

    const auto session = std::make_unique<IdentitySession>(
        initial_bundles, generate_unique_id_pair(&alice_username, &bob_username)
    );

    // Verify initial ratchets were created
    ASSERT_EQ(session->get_ratchets().size(), 2);

    // Try to add the same bundles again
    session->updateFromBundles(initial_bundles);

    // Verify no new ratchets were created
    ASSERT_EQ(session->get_ratchets().size(), 2);

    // Add a new bundle
    std::vector<KeyBundle*> new_bundles;
    new_bundles.push_back(bob_to_charlie_bundle);
    session->updateFromBundles(new_bundles);

    // Verify new ratchet was created
    ASSERT_EQ(session->get_ratchets().size(), 3);
}

TEST_F(IdentitySessionTest, MessageRoutingTest) {
    // Initialize identity session with multiple bundles
    std::vector<KeyBundle*> bundles;
    bundles.push_back(alice_to_bob_bundle);
    bundles.push_back(alice_to_charlie_bundle);
    bundles.push_back(bob_to_charlie_bundle);

    std::string alice_username = "alice";
    std::string bob_username = "bob";

    const auto session = std::make_unique<IdentitySession>(
        bundles, generate_unique_id_pair(&alice_username, &bob_username)
    );

    // Create a test message
    unsigned char test_message[32] = "Test message for routing";
    
    // Send message and verify it creates messages for all ratchets
    auto messages = session->send_message(test_message, sizeof(test_message));
    ASSERT_EQ(messages.size(), 3); // One message per ratchet

    // Verify each message has correct header and ciphertext
    for (const auto& [session_id, message] : messages) {
        ASSERT_NE(message->header, nullptr);
        ASSERT_NE(message->ciphertext, nullptr);
        ASSERT_GT(message->length, 0);
    }
}

TEST_F(IdentitySessionTest, MessageSendingAndReceivingTest) {
    // Initialize identity session with a single bundle
    std::vector<KeyBundle*> bundles;
    bundles.push_back(alice_to_bob_bundle);

    std::string alice_username = "alice";
    std::string bob_username = "bob";

    const auto session = std::make_unique<IdentitySession>(
        bundles, generate_unique_id_pair(&alice_username, &bob_username)
    );

    // Create a test message
    unsigned char test_message[32] = "Test message for sending";
    
    // Send message
    auto messages = session->send_message(test_message, sizeof(test_message));
    ASSERT_EQ(messages.size(), 1);

    // Get the sent message
    auto [session_id, sent_message] = std::move(messages[0]);

    // Receive the message
    auto received_plaintext = session->receive_message(sent_message.get());

    // Verify the received message matches the original
    ASSERT_EQ(received_plaintext.size(), sizeof(test_message));
    ASSERT_EQ(memcmp(received_plaintext.data(), test_message, sizeof(test_message)), 0);
}

TEST_F(IdentitySessionTest, MultipleMessageExchangeTest) {
    // Initialize identity session with multiple bundles
    std::vector<KeyBundle*> bundles;
    bundles.push_back(alice_to_bob_bundle);
    bundles.push_back(alice_to_charlie_bundle);

    std::string alice_username = "alice";
    std::string bob_username = "bob";

    const auto session = std::make_unique<IdentitySession>(
        bundles, generate_unique_id_pair(&alice_username, &bob_username)
    );

    // Send multiple messages
    const int num_messages = 5;
    std::vector<std::vector<unsigned char>> sent_messages;
    std::vector<std::vector<std::tuple<IdentitySessionId, std::unique_ptr<DeviceMessage>>>> encrypted_messages;

    for (int i = 0; i < num_messages; i++) {
        // Create unique message for each send
        std::vector<unsigned char> message(32);
        snprintf(reinterpret_cast<char*>(message.data()), 32, "Test message %d", i);
        sent_messages.push_back(message);

        // Send message
        auto messages = session->send_message(message.data(), message.size());
        encrypted_messages.push_back(std::move(messages));
    }

    // Verify each message was sent to all ratchets
    for (const auto& messages : encrypted_messages) {
        ASSERT_EQ(messages.size(), 2); // One message per ratchet
    }

    // Receive and verify each message
    for (size_t i = 0; i < encrypted_messages.size(); i++) {
        for (const auto& [session_id, message] : encrypted_messages[i]) {
            auto received_plaintext = session->receive_message(message.get());
            ASSERT_EQ(received_plaintext.size(), sent_messages[i].size());
            ASSERT_EQ(memcmp(received_plaintext.data(), sent_messages[i].data(), sent_messages[i].size()), 0);
        }
    }
}

