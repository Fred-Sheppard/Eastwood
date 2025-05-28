#include <iostream>
#include <iomanip>
#include <sstream>
#include <sodium.h>
#include "src/key_exchange/utils.h"
#include "src/key_exchange/DoubleRatchet.h"
#include "src/sessions/KeyBundle.h"
#include <memory>

void test_double_ratchet_shared_secret() {
    std::cout << "\n===== TESTING DOUBLE RATCHET SHARED SECRET =====" << std::endl;
    
    // Initialize libsodium
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return;
    }
    
    try {
        // Generate Ed25519 identity keys for Alice (Initiator)
        unsigned char alice_id_public[crypto_sign_PUBLICKEYBYTES];
        unsigned char alice_id_private[crypto_sign_SECRETKEYBYTES];
        crypto_sign_keypair(alice_id_public, alice_id_private);

        // Generate X25519 ephemeral keys for Alice
        unsigned char alice_ephemeral_public[crypto_box_PUBLICKEYBYTES];
        unsigned char alice_ephemeral_private[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(alice_ephemeral_public, alice_ephemeral_private);

        // Generate Ed25519 identity keys for Bob (Responder)
        unsigned char bob_id_public[crypto_sign_PUBLICKEYBYTES];
        unsigned char bob_id_private[crypto_sign_SECRETKEYBYTES];
        crypto_sign_keypair(bob_id_public, bob_id_private);

        // Generate X25519 signed prekey and one-time prekey for Bob
        unsigned char bob_signed_public[crypto_box_PUBLICKEYBYTES];
        unsigned char bob_signed_private[crypto_box_SECRETKEYBYTES];
        unsigned char bob_onetime_public[crypto_box_PUBLICKEYBYTES];
        unsigned char bob_onetime_private[crypto_box_SECRETKEYBYTES];
        crypto_box_keypair(bob_signed_public, bob_signed_private);
        crypto_box_keypair(bob_onetime_public, bob_onetime_private);

        // Sign Bob's X25519 signed prekey with Bob's Ed25519 private key
        unsigned char bob_signed_signature[crypto_sign_BYTES];
        crypto_sign_detached(bob_signed_signature, nullptr,
                            bob_signed_public, crypto_box_PUBLICKEYBYTES,
                            bob_id_private);

        // (Optional) Verify signature
        if (crypto_sign_verify_detached(bob_signed_signature,
                                        bob_signed_public, crypto_box_PUBLICKEYBYTES,
                                        bob_id_public) != 0) {
            std::cerr << "❌ Signature verification failed!" << std::endl;
            return;
        }

        // Create key bundles
        auto alice_bundle = std::make_unique<SendingKeyBundle>(
            alice_id_public,
            alice_id_private,
            alice_ephemeral_public,
            alice_ephemeral_private,
            bob_id_public,
            bob_signed_public,
            bob_onetime_public,
            bob_signed_signature
        );

        auto bob_bundle = std::make_unique<ReceivingKeyBundle>(
            alice_id_public,
            alice_ephemeral_public,
            bob_id_private,
            bob_signed_private,
            bob_onetime_private
        );

        // Create Double Ratchet instances
        auto alice_ratchet = std::make_unique<DoubleRatchet>(alice_bundle.get());
        auto bob_ratchet = std::make_unique<DoubleRatchet>(bob_bundle.get());
        
        // Print initial state
        std::cout << "\nInitial state:" << std::endl;
        std::cout << "Alice's ratchet:" << std::endl;
        alice_ratchet->print_state();
        std::cout << "\nBob's ratchet:" << std::endl;
        bob_ratchet->print_state();
        
        // Test message
        const char* test_message = "Hello, Bob!";
        size_t message_len = strlen(test_message);
        auto message = std::make_unique<unsigned char[]>(message_len + 1);
        memcpy(message.get(), test_message, message_len + 1);
        
        // Alice sends message to Bob
        DeviceMessage encrypted_message = alice_ratchet->message_send(message.get(), bob_id_public);
        
        // Bob receives the message
        std::vector<unsigned char> decrypted_message = bob_ratchet->message_receive(encrypted_message);
        
        // Print state after message exchange
        std::cout << "\nState after message exchange:" << std::endl;
        std::cout << "Alice's ratchet:" << std::endl;
        alice_ratchet->print_state();
        std::cout << "\nBob's ratchet:" << std::endl;
        bob_ratchet->print_state();
        
        // Verify the message was decrypted correctly
        std::string decrypted_str(decrypted_message.begin(), decrypted_message.end());
        std::cout << "\nDecrypted message: " << decrypted_str << std::endl;
        std::cout << "Original message: " << test_message << std::endl;
        std::cout << "Messages match: " << (decrypted_str == test_message ? "YES" : "NO") << std::endl;
        
        // Send multiple messages back and forth
        std::vector<std::string> messages = {
            // First batch from Alice
            "Hello, Bob!",
            "How are you doing?",
            "I have some exciting news!",
            // One from Bob
            "Hi Alice, I'm doing great!",
            // More from Alice
            "I've been working on a new project",
            "It's a secure messaging system",
            "Using the Double Ratchet protocol",
            // Final batch from Bob
            "That sounds fascinating!",
            "Can you tell me more about it?",
            "I'd love to learn more about the implementation"
        };

        std::cout << "\n===== TESTING MULTIPLE MESSAGE EXCHANGES =====" << std::endl;
        for (size_t i = 0; i < messages.size(); ++i) {
            // Determine sender and receiver
            DoubleRatchet* sender = (i < 3 || (i >= 4 && i < 7)) ? alice_ratchet.get() : bob_ratchet.get();
            DoubleRatchet* receiver = (i < 3 || (i >= 4 && i < 7)) ? bob_ratchet.get() : alice_ratchet.get();
            std::string message = messages[i];

            // Print current state before sending
            std::cout << "\nBefore message " << i + 1 << " (" << (sender == alice_ratchet.get() ? "Alice" : "Bob") << " sending):" << std::endl;
            sender->print_state();
            std::cout << "\nReceiver state:" << std::endl;
            receiver->print_state();

            // Send message
            DeviceMessage encrypted = sender->message_send((unsigned char*)message.c_str(), bob_id_public);
            std::vector<unsigned char> decrypted = receiver->message_receive(encrypted);

            // Verify decryption
            std::string decrypted_str(decrypted.begin(), decrypted.end());
            std::cout << "\nMessage " << i + 1 << " sent: " << message << std::endl;
            std::cout << "Message " << i + 1 << " decrypted: " << decrypted_str << std::endl;
            
            // Print state after message exchange
            std::cout << "\nAfter message " << i + 1 << ":" << std::endl;
            sender->print_state();
            std::cout << "\nReceiver state:" << std::endl;
            receiver->print_state();

            if (message != decrypted_str) {
                std::cerr << "❌ Message " << i + 1 << " decryption failed!" << std::endl;
                return;
            }
            std::cout << "✅ Message " << i + 1 << " successfully exchanged" << std::endl;
        }
        
        std::cout << "\n✅ Double Ratchet shared secret test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error in test: " << e.what() << std::endl;
    }
    
    std::cout << "\n===== DOUBLE RATCHET SHARED SECRET TEST COMPLETED =====" << std::endl;
}

int main() {
    test_double_ratchet_shared_secret();
    return 0;
} 