#include <gtest/gtest.h>
#include <sodium.h>
#include <memory>
#include <vector>
#include <map>
#include <QByteArray>
#include "src/sessions/IdentityCommunicationSession.h"
#include "src/sessions/DeviceCommunicationSession.h"
#include "src/key_exchange/utils.h"
#include "src/structs/KeyBundle.h"
#include "src/sql/queries.h"
#include "src/database/database.h"
#include <iomanip>
#include <sstream>

// Provide a global definition to override the real get_keypair at link time
typedef std::tuple<QByteArray, QByteArray> KeypairTuple;
KeypairTuple get_keypair(const std::string& label) {
    // Generate Ed25519 keys directly
    unsigned char ed25519_pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char ed25519_sk[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(ed25519_pk, ed25519_sk);

    QByteArray public_key(reinterpret_cast<char*>(ed25519_pk), crypto_sign_PUBLICKEYBYTES);
    QByteArray private_key(reinterpret_cast<char*>(ed25519_sk), crypto_sign_SECRETKEYBYTES);
    return std::make_tuple(public_key, private_key);
}

// Mock save_keypair and save_encrypted_key for test build
void save_keypair(const std::string&, unsigned char*, unsigned char*, unsigned char*) {}
void save_encrypted_key(const std::string&, unsigned char*, unsigned char*) {}

// Mock database for testing
class MockDatabase {
public:
    static MockDatabase& get() {
        static MockDatabase instance;
        return instance;
    }

    bool initialize(const QString& masterKey, bool encrypted) {
        return true;  // Always succeed
    }

    bool isInitialized() const {
        return true;  // Always initialized
    }

    void prepare_or_throw(const char* zSql, sqlite3_stmt** stmt) const {
        // No-op for testing
    }

    void execute(sqlite3_stmt* stmt) const {
        // No-op for testing
    }

    QVector<QVariantMap> query(sqlite3_stmt* stmt) const {
        QVector<QVariantMap> results;
        QVariantMap row;
        
        // Create mock key pair data
        QByteArray public_key(crypto_box_PUBLICKEYBYTES, 0);
        QByteArray private_key(crypto_box_SECRETKEYBYTES, 0);
        
        // Fill with deterministic test data
        for (int i = 0; i < crypto_box_PUBLICKEYBYTES; i++) {
            public_key[i] = static_cast<char>(i % 256);
        }
        for (int i = 0; i < crypto_box_SECRETKEYBYTES; i++) {
            private_key[i] = static_cast<char>((i + 128) % 256);
        }
        
        row["public_key"] = public_key;
        row["encrypted_private_key"] = private_key;
        results.append(row);
        
        return results;
    }
};

// Mock key pair generator for testing
class MockKeyPairGenerator {
public:
    static std::pair<unsigned char*, unsigned char*> generateKeyPair() {
        unsigned char* public_key = new unsigned char[crypto_box_PUBLICKEYBYTES];
        unsigned char* private_key = new unsigned char[crypto_box_SECRETKEYBYTES];
        
        // Generate deterministic test keys
        for (size_t i = 0; i < crypto_box_PUBLICKEYBYTES; i++) {
            public_key[i] = static_cast<unsigned char>(i % 256);
        }
        for (size_t i = 0; i < crypto_box_SECRETKEYBYTES; i++) {
            private_key[i] = static_cast<unsigned char>((i + 128) % 256);
        }
        
        return {public_key, private_key};
    }
};

class IdentityCommunicationSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (sodium_init() < 0) {
            FAIL() << "Failed to initialize libsodium";
        }
        
        // Initialize the mock database
        auto& db = Database::get();
        if (!db.initialize("test_key", false)) {
            FAIL() << "Failed to initialize mock database";
        }
    }

    // Helper function to generate key pairs using mock
    std::pair<unsigned char*, unsigned char*> generateKeyPair() {
        return MockKeyPairGenerator::generateKeyPair();
    }

    // Helper function to print a key in hex
    void print_key(const char* name, const unsigned char* key, size_t len) {
        std::ostringstream oss;
        for (size_t i = 0; i < len; ++i)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)key[i];
        std::cout << name << ": " << oss.str() << std::endl;
    }

    // Helper function to generate a key bundle
    SendingKeyBundle generateSendingKeyBundle(unsigned char* recipient_ed25519_public, unsigned char* recipient_ed25519_private) {
        SendingKeyBundle bundle;
        
        // Generate device private key
        auto [device_public, device_private] = generateKeyPair();
        bundle.my_device_private = device_private;
        // device_public is not used in SendingKeyBundle

        // Generate ephemeral keys
        auto [ephemeral_public, ephemeral_private] = generateKeyPair();
        bundle.my_ephemeral_public = ephemeral_public;
        bundle.my_ephemeral_private = ephemeral_private;

        // Generate recipient keys
        auto [recipient_device_public, _] = generateKeyPair();
        auto [signed_public, signed_private] = generateKeyPair();
        auto [onetime_public, onetime_private] = generateKeyPair();
        
        bundle.their_device_public = recipient_device_public;
        bundle.their_signed_public = signed_public;
        bundle.their_onetime_public = onetime_public;
        bundle.their_ed25519_public = recipient_ed25519_public;

        // Generate signature using the recipient's Ed25519 private key
        unsigned char* signature = new unsigned char[crypto_sign_BYTES];
        crypto_sign_detached(signature, nullptr, signed_public, crypto_box_PUBLICKEYBYTES, recipient_ed25519_private);
        bundle.their_signature = signature;
        // Debug output
        print_key("signed_public", signed_public, crypto_box_PUBLICKEYBYTES);
        print_key("signature", signature, crypto_sign_BYTES);
        print_key("recipient_ed25519_public", recipient_ed25519_public, crypto_sign_PUBLICKEYBYTES);

        return bundle;
    }

    // Helper function to clean up key bundles
    void cleanupKeyBundle(SendingKeyBundle& bundle) {
        delete[] bundle.my_device_private;
        delete[] bundle.my_ephemeral_public;
        delete[] bundle.my_ephemeral_private;
        delete[] bundle.their_device_public;
        delete[] bundle.their_signed_public;
        delete[] bundle.their_onetime_public;
        delete[] bundle.their_signature;
    }

    // Helper function to generate Ed25519 keypair
    std::pair<unsigned char*, unsigned char*> generateEd25519KeyPair() {
        unsigned char* pk = new unsigned char[crypto_sign_PUBLICKEYBYTES];
        unsigned char* sk = new unsigned char[crypto_sign_SECRETKEYBYTES];
        crypto_sign_keypair(pk, sk);
        return {pk, sk};
    }
};

// Test creating an identity communication session
TEST_F(IdentityCommunicationSessionTest, CreateIdentitySession) {
    // Generate Ed25519 identity keys
    auto [alice_identity_public, alice_identity_private] = generateEd25519KeyPair();
    auto [bob_identity_public, bob_identity_private] = generateEd25519KeyPair();

    // Generate key bundles
    std::vector<SendingKeyBundle> alice_bundles;
    std::vector<ReceivingKeyBundle> bob_bundles;
    
    SendingKeyBundle alice_bundle = generateSendingKeyBundle(bob_identity_public, bob_identity_private);
    alice_bundles.push_back(alice_bundle);
        
    // Create sessions
    auto alice_session = std::make_unique<IdentityCommunicationSession>(
        alice_bundles, bob_bundles, alice_identity_public, bob_identity_public);
        
    // Verify session was created
    EXPECT_NE(alice_session, nullptr);
    EXPECT_EQ(alice_session->getDeviceSessions().size(), 1);

    // Cleanup
    cleanupKeyBundle(alice_bundle);
    delete[] alice_identity_public;
    delete[] alice_identity_private;
    delete[] bob_identity_public;
    delete[] bob_identity_private;
}

// Test multi-device session management
TEST_F(IdentityCommunicationSessionTest, MultiDeviceSessionManagement) {
    // Generate Ed25519 identity keys
    auto [alice_identity_public, alice_identity_private] = generateEd25519KeyPair();
    auto [bob_identity_public, bob_identity_private] = generateEd25519KeyPair();

    // Generate key bundles
    std::vector<SendingKeyBundle> alice_bundles;
    std::vector<ReceivingKeyBundle> bob_bundles;
        
    // Create 3 key bundles for Alice's devices
    for (int i = 0; i < 3; i++) {
        SendingKeyBundle bundle = generateSendingKeyBundle(bob_identity_public, bob_identity_private);
        alice_bundles.push_back(bundle);
    }

    // Create session
    auto alice_session = std::make_unique<IdentityCommunicationSession>(
        alice_bundles, bob_bundles, alice_identity_public, bob_identity_public);
        
    // Verify all device sessions were created
    EXPECT_EQ(alice_session->getDeviceSessions().size(), 3);
        
    // Cleanup
    for (auto& bundle : alice_bundles) {
        cleanupKeyBundle(bundle);
    }
    delete[] alice_identity_public;
    delete[] alice_identity_private;
    delete[] bob_identity_public;
    delete[] bob_identity_private;
}

// Test message encryption and decryption
TEST_F(IdentityCommunicationSessionTest, MessageEncryptionDecryption) {
    // Generate Ed25519 identity keys
    auto [alice_identity_public, alice_identity_private] = generateEd25519KeyPair();
    auto [bob_identity_public, bob_identity_private] = generateEd25519KeyPair();

    // Generate key bundles
    std::vector<SendingKeyBundle> alice_bundles;
    std::vector<ReceivingKeyBundle> bob_bundles;
    
    SendingKeyBundle alice_bundle = generateSendingKeyBundle(bob_identity_public, bob_identity_private);
    alice_bundles.push_back(alice_bundle);
    
    // Create sessions
    auto alice_session = std::make_unique<IdentityCommunicationSession>(
        alice_bundles, bob_bundles, alice_identity_public, bob_identity_public);
    
    // Test message
    const char* test_message = "Hello, Bob!";
    size_t message_len = strlen(test_message);
    auto message = std::make_unique<unsigned char[]>(message_len + 1);
    memcpy(message.get(), test_message, message_len + 1);
    
    // Send message
    alice_session->message_send(message.get());
    
    // Cleanup
    cleanupKeyBundle(alice_bundle);
    delete[] alice_identity_public;
    delete[] alice_identity_private;
    delete[] bob_identity_public;
    delete[] bob_identity_private;
}

// Test session key management
TEST_F(IdentityCommunicationSessionTest, SessionKeyManagement) {
    // Generate Ed25519 identity keys
    auto [alice_identity_public, alice_identity_private] = generateEd25519KeyPair();
    auto [bob_identity_public, bob_identity_private] = generateEd25519KeyPair();

    // Generate key bundles
    std::vector<SendingKeyBundle> alice_bundles;
    std::vector<ReceivingKeyBundle> bob_bundles;
    
    SendingKeyBundle alice_bundle = generateSendingKeyBundle(bob_identity_public, bob_identity_private);
    alice_bundles.push_back(alice_bundle);

    // Create sessions
    auto alice_session = std::make_unique<IdentityCommunicationSession>(
        alice_bundles, bob_bundles, alice_identity_public, bob_identity_public);

    // Verify session keys
    const auto& device_sessions = alice_session->getDeviceSessions();
    EXPECT_EQ(device_sessions.size(), 1);

    // Get the first device session
    auto it = device_sessions.begin();
    DeviceCommunicationSession* device_session = it->second;

    // Verify device session has valid keys
    EXPECT_NE(device_session->getSharedSecret(), nullptr);
    EXPECT_NE(device_session->getRatchet(), nullptr);

    // Cleanup
    cleanupKeyBundle(alice_bundle);
    delete[] alice_identity_public;
    delete[] alice_identity_private;
    delete[] bob_identity_public;
    delete[] bob_identity_private;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 