#include <iostream>
#include <iomanip>
#include <sstream>
#include <sodium.h>
#include "cryptography/DoubleRatchet.h"
#include "cryptography/CommunicationSession.h"
#include "cryptography/x3dh.h"

std::string bin2hex(const unsigned char* bin, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bin[i];
    return oss.str();
}

// Print details of a message header
void print_message_header(const MessageHeader& header) {
    std::cout << "  Public key: " << bin2hex(header.dh_public, crypto_kx_PUBLICKEYBYTES) << std::endl;
    std::cout << "  Message index: " << header.message_index << std::endl;
    std::cout << "  Previous chain length: " << header.prev_chain_length << std::endl;
}

void test_double_ratchet() {
    std::cout << "\n===== COMPREHENSIVE DOUBLE RATCHET TESTING =====" << std::endl;
    
    // Initialize root key directly for this test
    unsigned char root_key[crypto_kdf_KEYBYTES];
    randombytes_buf(root_key, crypto_kdf_KEYBYTES);
    std::cout << "Initial root key: " << bin2hex(root_key, crypto_kdf_KEYBYTES) << std::endl;
    
    // Generate initial keypairs for Alice and Bob
    unsigned char alice_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char alice_private[crypto_kx_SECRETKEYBYTES];
    crypto_kx_keypair(alice_public, alice_private);
    
    unsigned char bob_public[crypto_kx_PUBLICKEYBYTES];
    unsigned char bob_private[crypto_kx_SECRETKEYBYTES];
    crypto_kx_keypair(bob_public, bob_private);
    
    // Create Double Ratchet instances
    DoubleRatchet alice(root_key, bob_public, alice_public, alice_private);
    DoubleRatchet bob(root_key, alice_public, bob_public, bob_private);
    
    // Save initial keys for comparison
    std::cout << "\nInitial keys:" << std::endl;
    std::cout << "Alice public key: " << bin2hex(alice_public, crypto_kx_PUBLICKEYBYTES) << std::endl;
    std::cout << "Bob public key: " << bin2hex(bob_public, crypto_kx_PUBLICKEYBYTES) << std::endl;
    
    // Array to store message keys for later verification
    const int NUM_MESSAGES = 10;  // Increased for out-of-order test
    unsigned char* alice_keys[NUM_MESSAGES];
    unsigned char* bob_keys[NUM_MESSAGES];
    MessageHeader alice_headers[NUM_MESSAGES];
    MessageHeader bob_headers[NUM_MESSAGES];
    
    // Test multiple messages in the same chain
    std::cout << "\n--- 1. Multiple messages in the same ratchet chain ---" << std::endl;
    std::cout << "\nAlice sending 3 messages to Bob:" << std::endl;
    
    // Alice sends 3 messages on her chain
    for (int i = 0; i < 3; i++) {
        std::cout << "\n[Alice → Bob: Message " << (i+1) << "]\n";
        alice_keys[i] = alice.message_send(alice_headers[i]);
        std::cout << "  Message key: " << bin2hex(alice_keys[i], crypto_kdf_KEYBYTES) << std::endl;
        print_message_header(alice_headers[i]);
    }
    
    // Bob receives 3 messages from Alice
    std::cout << "\nBob receiving 3 messages from Alice:" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "\n[Bob receives message " << (i+1) << "]\n";
        bob_keys[i] = bob.message_receive(alice_headers[i]);
        std::cout << "  Message key: " << bin2hex(bob_keys[i], crypto_kdf_KEYBYTES) << std::endl;
        
        // Check if the keys match
        bool keys_match = (memcmp(alice_keys[i], bob_keys[i], crypto_kdf_KEYBYTES) == 0);
        std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    }
    
    // Test DH ratchet rotation
    std::cout << "\n--- 2. DH Ratchet rotation ---" << std::endl;
    std::cout << "\nBob sending message to Alice (triggers DH ratchet on Alice's side):" << std::endl;
    
    // Bob sends a message back (this will cause a DH ratchet when Alice receives it)
    bob_keys[3] = bob.message_send(bob_headers[0]);
    std::cout << "\n[Bob → Alice: Message 1]\n";
    std::cout << "  Message key: " << bin2hex(bob_keys[3], crypto_kdf_KEYBYTES) << std::endl;
    print_message_header(bob_headers[0]);
    
    // Alice receives Bob's message, which should trigger a DH ratchet
    std::cout << "\nAlice receiving message from Bob (triggers DH ratchet):" << std::endl;
    alice_keys[3] = alice.message_receive(bob_headers[0]);
    std::cout << "  Message key: " << bin2hex(alice_keys[3], crypto_kdf_KEYBYTES) << std::endl;
    
    bool keys_match = (memcmp(bob_keys[3], alice_keys[3], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Alice sends a message after DH ratchet
    std::cout << "\nAlice sending message after DH ratchet:" << std::endl;
    alice_keys[4] = alice.message_send(alice_headers[3]);
    std::cout << "\n[Alice → Bob: Message after ratchet]\n";
    std::cout << "  Message key: " << bin2hex(alice_keys[4], crypto_kdf_KEYBYTES) << std::endl;
    print_message_header(alice_headers[3]);
    
    // Bob receives Alice's post-ratchet message
    std::cout << "\nBob receiving message from Alice (after ratchet):" << std::endl;
    bob_keys[4] = bob.message_receive(alice_headers[3]);
    std::cout << "  Message key: " << bin2hex(bob_keys[4], crypto_kdf_KEYBYTES) << std::endl;
    
    keys_match = (memcmp(alice_keys[4], bob_keys[4], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Test out-of-order message handling
    std::cout << "\n--- 3. Out-of-order message handling ---" << std::endl;
    
    // Alice sends 3 more messages
    std::cout << "\nAlice sending 3 more messages to Bob:" << std::endl;
    for (int i = 0; i < 3; i++) {
        int index = 5 + i;
        std::cout << "\n[Alice → Bob: Message " << (index-4) << "]\n";
        alice_keys[index] = alice.message_send(alice_headers[index-2]);
        std::cout << "  Message key: " << bin2hex(alice_keys[index], crypto_kdf_KEYBYTES) << std::endl;
        print_message_header(alice_headers[index-2]);
    }
    
    // Bob receives these messages out of order: 3, 1, 2
    std::cout << "\nBob receiving messages out of order (3, 1, 2):" << std::endl;
    
    // Message 3
    std::cout << "\n[Bob receives message 3 first]\n";
    bob_keys[7] = bob.message_receive(alice_headers[5]);
    std::cout << "  Message key: " << bin2hex(bob_keys[7], crypto_kdf_KEYBYTES) << std::endl;
    keys_match = (memcmp(alice_keys[7], bob_keys[7], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Message 1
    std::cout << "\n[Bob receives message 1 second]\n";
    bob_keys[5] = bob.message_receive(alice_headers[3]);
    std::cout << "  Message key: " << bin2hex(bob_keys[5], crypto_kdf_KEYBYTES) << std::endl;
    keys_match = (memcmp(alice_keys[5], bob_keys[5], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Message 2
    std::cout << "\n[Bob receives message 2 last]\n";
    bob_keys[6] = bob.message_receive(alice_headers[4]);
    std::cout << "  Message key: " << bin2hex(bob_keys[6], crypto_kdf_KEYBYTES) << std::endl;
    keys_match = (memcmp(alice_keys[6], bob_keys[6], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Another direction flip with out-of-order
    std::cout << "\n--- 4. Direction flip with out-of-order messages ---" << std::endl;
    
    // Bob sends 2 messages to Alice
    std::cout << "\nBob sending 2 messages to Alice:" << std::endl;
    for (int i = 0; i < 2; i++) {
        int index = 8 + i;
        std::cout << "\n[Bob → Alice: Message " << (i+1) << "]\n";
        bob_keys[index] = bob.message_send(bob_headers[i+1]);
        std::cout << "  Message key: " << bin2hex(bob_keys[index], crypto_kdf_KEYBYTES) << std::endl;
        print_message_header(bob_headers[i+1]);
    }
    
    // Alice receives these messages out of order: 2, 1
    std::cout << "\nAlice receiving messages out of order (2, 1):" << std::endl;
    
    // Message 2
    std::cout << "\n[Alice receives message 2 first]\n";
    alice_keys[9] = alice.message_receive(bob_headers[2]);
    std::cout << "  Message key: " << bin2hex(alice_keys[9], crypto_kdf_KEYBYTES) << std::endl;
    keys_match = (memcmp(bob_keys[9], alice_keys[9], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Message 1
    std::cout << "\n[Alice receives message 1 second]\n";
    alice_keys[8] = alice.message_receive(bob_headers[1]);
    std::cout << "  Message key: " << bin2hex(alice_keys[8], crypto_kdf_KEYBYTES) << std::endl;
    keys_match = (memcmp(bob_keys[8], alice_keys[8], crypto_kdf_KEYBYTES) == 0);
    std::cout << "  Keys match: " << (keys_match ? "YES" : "NO") << std::endl;
    
    // Final verification
    std::cout << "\n--- Final verification ---" << std::endl;
    int match_count = 0;
    for (int i = 0; i < NUM_MESSAGES; i++) {
        bool match = (memcmp(alice_keys[i], bob_keys[i], crypto_kdf_KEYBYTES) == 0);
        match_count += match ? 1 : 0;
        std::cout << "Message " << (i+1) << ": " << (match ? "MATCH" : "MISMATCH") << std::endl;
    }
    
    std::cout << "Total matching keys: " << match_count << "/" << NUM_MESSAGES << std::endl;
    std::cout << "Double Ratchet test " << (match_count == NUM_MESSAGES ? "PASSED" : "FAILED") << std::endl;
    
    // Clean up
    for (int i = 0; i < NUM_MESSAGES; i++) {
        delete[] alice_keys[i];
        delete[] bob_keys[i];
    }
}

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }
    
    test_double_ratchet();
    
    return 0;
}