//
// Created by Josh Sloggett on 31/05/2025.
//

#include <gtest/gtest.h>
#include <tuple>
#include <iostream>

#include "kek_manager.h"
#include "NewRatchet.h"
#include "utils.h"
#include "database/schema.h"
#include "keys/secure_memory_buffer.h"
#include "algorithms/algorithms.h"
#include "database/database.h"
#include "sessions/KeyBundle.h"
#include "sessions/RatchetSessionManager.h"
#include "key_exchange/MessageStructs.h"

class RatchetSessionManagerTest : public ::testing::Test {
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
        std::shared_ptr<SecureMemoryBuffer> shared_alice_to_bob_eph_priv = std::shared_ptr<SecureMemoryBuffer>(std::move(alice_to_bob_eph_priv_copy));

        auto alice_to_charlie_eph_priv_copy = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        memcpy(alice_to_charlie_eph_priv_copy->data(), alice_to_charlie_eph_priv->data(), crypto_sign_SECRETKEYBYTES);
        std::shared_ptr<SecureMemoryBuffer> shared_alice_to_charlie_eph_priv = std::shared_ptr<SecureMemoryBuffer>(std::move(alice_to_charlie_eph_priv_copy));

        auto charlie_eph_priv_copy = SecureMemoryBuffer::create(crypto_sign_SECRETKEYBYTES);
        memcpy(charlie_eph_priv_copy->data(), charlie_eph_priv->data(), crypto_sign_SECRETKEYBYTES);
        std::shared_ptr<SecureMemoryBuffer> shared_charlie_eph_priv = std::shared_ptr<SecureMemoryBuffer>(std::move(charlie_eph_priv_copy));

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

        // Initialize RatchetSessionManager
        session_manager = std::make_unique<RatchetSessionManager>();
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

    // Helper function to create a MessageHeader for testing
    MessageHeader* create_test_header(const unsigned char* dh_public, const unsigned char* device_id, int message_index, int prev_chain_length) {
        auto header = new MessageHeader();
        memcpy(header->dh_public, dh_public, crypto_kx_PUBLICKEYBYTES);
        memcpy(header->device_id, device_id, crypto_box_PUBLICKEYBYTES);
        header->message_index = message_index;
        header->prev_chain_length = prev_chain_length;
        return header;
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

    // Test data
    std::unique_ptr<RatchetSessionManager> session_manager;

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

TEST_F(RatchetSessionManagerTest, RatchetCreationTest) {
    switch_to_alice_db();
    // Test that ratchets are created when bundles are provided
    std::vector<KeyBundle*> alice_bundles = {alice_to_bob_bundle};
    session_manager->create_ratchets_if_needed("bob", alice_bundles);
    
    // Verify ratchet was created by attempting to get keys
    auto keys = session_manager->get_keys_for_identity("bob");
    EXPECT_EQ(keys.size(), 1);
    
    // Check that the device ID matches bob's device public key
    std::array<unsigned char, 32> bob_device_id;
    memcpy(bob_device_id.data(), bob_device_pub, 32);
    EXPECT_NE(keys.find(bob_device_id), keys.end());
}

TEST_F(RatchetSessionManagerTest, MultipleDeviceRatchetCreationTest) {
    // Test creating ratchets for multiple devices
    switch_to_alice_db();
    std::vector<KeyBundle*> alice_bundles = {alice_to_charlie_bundle, alice_to_bob_bundle};
    session_manager->create_ratchets_if_needed("twobundles", alice_bundles);
    
    auto keys = session_manager->get_keys_for_identity("twobundles");
    EXPECT_EQ(keys.size(), 2);
    
    // Check that both Alice and Bob device IDs are present
    std::array<unsigned char, 32> charlie_device_id;
    std::array<unsigned char, 32> bob_device_id;
    memcpy(charlie_device_id.data(), charlie_device_pub, 32);
    memcpy(bob_device_id.data(), bob_device_pub, 32);
    
    EXPECT_NE(keys.find(charlie_device_id), keys.end());
    EXPECT_NE(keys.find(bob_device_id), keys.end());
}

TEST_F(RatchetSessionManagerTest, DuplicateRatchetPreventionTest) {
    // Test that duplicate ratchets are not created
    switch_to_alice_db();
    std::vector<KeyBundle*>alice_bundles = {alice_to_bob_bundle};
    
    // Create ratchet first time
    session_manager->create_ratchets_if_needed("bob", alice_bundles);
    auto keys1 = session_manager->get_keys_for_identity("bob");
    
    // Attempt to create again - should not create duplicates
    session_manager->create_ratchets_if_needed("bob", alice_bundles);
    auto keys2 = session_manager->get_keys_for_identity("bob");
    
    EXPECT_EQ(keys1.size(), keys2.size());
    EXPECT_EQ(keys1.size(), 1);
}

TEST_F(RatchetSessionManagerTest, KeyGenerationForSendingTest) {
    // Test that keys are properly generated for sending
    switch_to_alice_db();
    std::vector<KeyBundle*> alice_bundles = {alice_to_bob_bundle};
    session_manager->create_ratchets_if_needed("bob", alice_bundles);
    
    auto keys = session_manager->get_keys_for_identity("bob");
    EXPECT_EQ(keys.size(), 1);
    
    std::array<unsigned char, 32> bob_device_id;
    memcpy(bob_device_id.data(), bob_device_pub, 32);
    
    auto it = keys.find(bob_device_id);
    ASSERT_NE(it, keys.end());
    
    auto [message_key, header] = it->second;
    
    // Verify header is properly initialized
    EXPECT_NE(header, nullptr);
    EXPECT_EQ(header->message_index, 0); // First message should have index 0
    EXPECT_EQ(header->prev_chain_length, 0);
    
    // Verify message key is not all zeros
    bool all_zeros = true;
    for (int i = 0; i < 32; i++) {
        if (message_key[i] != 0) {
            all_zeros = false;
            break;
        }
    }
    EXPECT_FALSE(all_zeros);
    
    // Clean up
    delete header;
}

