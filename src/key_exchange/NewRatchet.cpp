//
// Created by Josh Sloggett on 30/05/2025.
//

#include "NewRatchet.h"
#include <sstream>
#include "XChaCha20-Poly1305.h"
#include "src/keys/kek_manager.h"
#include "src/sql/queries.h"
#include "src/key_exchange/utils.h"
#include <sodium.h>
#include <memory>
#include <iostream>

// other key is initiator ephemeral for recipient
// other key is receiver signed prekey for initiator
NewRatchet::NewRatchet(const unsigned char *shared_secret, const unsigned char *other_key) {
    memcpy(root_key, shared_secret, 32);

    set_up_initial_state_for_recipient(other_key);

    set_up_initial_chain_keys();
}

NewRatchet::NewRatchet(const unsigned char* shared_secret, const unsigned char* other_key,
                       const unsigned char* my_ephemeral_public, const std::shared_ptr<SecureMemoryBuffer> &my_ephemeral_private) {
    memcpy(root_key, shared_secret, 32);

    set_up_initial_state_for_initiator(other_key, my_ephemeral_public, my_ephemeral_private);

    set_up_initial_chain_keys();
}

//serialised
NewRatchet::NewRatchet(const std::vector<unsigned char> &serialised_ratchet) {
    std::istringstream in(std::string(
        reinterpret_cast<const char*>(serialised_ratchet.data()),
        serialised_ratchet.size()
    ));
    deserialise(in);
}

void NewRatchet::set_up_initial_state_for_initiator(const unsigned char* recipient_signed_public,
                                                    const unsigned char* my_ephemeral_public,
                                                    std::shared_ptr<SecureMemoryBuffer> my_ephemeral_private) {
    reversed = false;
    memcpy(local_dh_public, my_ephemeral_public, 32);
    local_dh_priv = SecureMemoryBuffer::create(32);
    memcpy(local_dh_priv->data(), my_ephemeral_private->data(), 32);
    memcpy(remote_dh_public, recipient_signed_public, 32);
}

void NewRatchet::set_up_initial_state_for_recipient(const unsigned char *initiator_ephemeral_public) {
    reversed = true;
    //setup initial local dh pub priv
    auto [public_signed, sk_signed] = get_decrypted_keypair("signed");
    local_dh_priv = std::move(sk_signed);
    memcpy(local_dh_public, public_signed.constData(), 32);

    //remote dh pub
    memcpy(remote_dh_public, initiator_ephemeral_public, 32);
};

void NewRatchet::set_up_initial_chain_keys() {
    auto dh_output = dh();

    unsigned char kdf_key[32];
    crypto_generichash(kdf_key, sizeof(kdf_key), dh_output.get(), 32, nullptr, 0);

    // run kdf
    crypto_kdf_derive_from_key(
        send_chain.key,
        32,
        0,
        "DRXsend1", // one side send is another recv
        kdf_key
    );

    crypto_kdf_derive_from_key(
        receive_chain.key,
        32,
        1,
        "DRXrecv1", // one side recv is another send
        kdf_key
    );

    crypto_kdf_derive_from_key(
        root_key,
        32,
        2,
        "DRXroot1",
        kdf_key
    );

    // Swap send/receive chains for responder
    if (reversed) {
        unsigned char tmp[32];
        memcpy(tmp, send_chain.key, 32);
        memcpy(send_chain.key, receive_chain.key, 32);
        memcpy(receive_chain.key, tmp, 32);
    }

    due_to_send_new_dh = false;
}

void NewRatchet::dh_ratchet_step(const bool received_new_dh) {
    auto dh_output = dh();


    unsigned char kdf_key[32];
    crypto_generichash(kdf_key, sizeof(kdf_key), dh_output.get(), 32, nullptr, 0);

    unsigned char new_root_key[32];
    crypto_kdf_derive_from_key(
        new_root_key,
        32,
        0,
        "DHRatchet",
        kdf_key
    );

    unsigned char new_chain_key[32];
    crypto_kdf_derive_from_key(
        new_chain_key,
        32,
        1,
        "DHRatchet",
        kdf_key
    );

    memcpy(root_key, new_root_key, 32);

    if (received_new_dh) {
        // When receiving a new DH key, update the receive chain
        memcpy(receive_chain.key, new_chain_key, 32);
        receive_chain.index = 0;

        // Generate new local DH keypair for future sends
        generate_new_local_dh_keypair();
        
        // Derive new send chain from updated root key and new DH
        auto new_dh_output = dh();
        unsigned char new_kdf_key[32];
        crypto_generichash(new_kdf_key, sizeof(new_kdf_key), new_dh_output.get(), 32, nullptr, 0);
        
        unsigned char updated_root_key[32];
        crypto_kdf_derive_from_key(
            updated_root_key,
            32,
            0,
            "DHRatchet",
            new_kdf_key
        );
        
        unsigned char new_send_chain_key[32];
        crypto_kdf_derive_from_key(
            new_send_chain_key,
            32,
            1,
            "DHRatchet",
            new_kdf_key
        );
        
        memcpy(root_key, updated_root_key, 32);
        memcpy(send_chain.key, new_send_chain_key, 32);
        send_chain.index = 0;

    } else {
        // When sending a new DH key, update the send chain
        memcpy(send_chain.key, new_chain_key, 32);
        send_chain.index = 0;
    }

    printf("  send_chain.key: ");
    for (int i = 0; i < 32; ++i) printf("%02x", send_chain.key[i]);
    printf("\n  receive_chain.key: ");
    for (int i = 0; i < 32; ++i) printf("%02x", receive_chain.key[i]);
    printf("\n  root_key: ");
    for (int i = 0; i < 32; ++i) printf("%02x", root_key[i]);
    printf("\n");
}

void NewRatchet::generate_new_local_dh_keypair() {
    crypto_box_keypair(local_dh_public, local_dh_priv->data());
};

std::unique_ptr<unsigned char[]> NewRatchet::dh() const {
    auto result = std::make_unique<unsigned char[]>(32);
    if (crypto_scalarmult(result.get(), local_dh_priv->data(), remote_dh_public) != 0) {
        throw std::runtime_error("Failed to perform dh");
    }
    return result;
}

std::tuple<std::array<unsigned char,32>, MessageHeader*> NewRatchet::advance_send() {

    if (due_to_send_new_dh) {
        dh_ratchet_step(false); // we are sending the new dh not receiving
        due_to_send_new_dh = false;
    }

    auto [raw_id, message_header] = progress_sending_ratchet();
    std::array<unsigned char, 32> device_id;
    std::memcpy(device_id.data(), raw_id, 32);

    return std::make_tuple(device_id, message_header);
}

unsigned char* NewRatchet::advance_receive(const MessageHeader* header) {

    // if new dh public
    if (memcmp(remote_dh_public, header->dh_public, 32) != 0) {
        int skipped_count = receive_chain.index;
        // if we need to forward cache keys due to prev chain length being longer than expected
        for (int i = header->prev_chain_length; i < skipped_count; ++i) {
            auto key = progress_receive_ratchet();
            if (skipped_keys.find(i) != skipped_keys.end()) {
                delete[] skipped_keys[i];  // Delete old key if it exists
            }
            skipped_keys[i] = key;
        }
        memcpy(remote_dh_public, header->dh_public, 32);
        dh_ratchet_step(true); // true as we received the new dh
        due_to_send_new_dh = true; // Next send should use new DH keys
    }

    // if we've already computed the key
    if (header->message_index < receive_chain.index) {
        if (skipped_keys.find(header->message_index) == skipped_keys.end()) {
            throw std::runtime_error("Key not found in backlog");
        }
        auto key = skipped_keys[header->message_index];
        skipped_keys.erase(header->message_index);
        return key;
    }

    // get the key normally
    for (int i = receive_chain.index; i <= header->message_index; i++) {
        const auto message_key = progress_receive_ratchet();
        printf("  Derived message_key at index %d: ", i);
        for (int j = 0; j < 32; ++j) printf("%02x", message_key[j]);
        printf("\n");

        if (i == header->message_index) {
            receive_chain.index = i + 1;
            return message_key;
        }
        if (skipped_keys.find(i) != skipped_keys.end()) {
            delete[] skipped_keys[i];  // Delete old key if it exists
        }
        skipped_keys[i] = message_key;
    }

    throw std::runtime_error("Unexpected error in advance_receive");
}

unsigned char* NewRatchet::progress_receive_ratchet() {
    auto msg_key = new unsigned char[32];
    unsigned char next_receive_key[32];
    const char *ctx = "DRSndKey"; // Use same context as sender

    crypto_kdf_derive_from_key(msg_key, 32, 0, ctx, receive_chain.key);
    crypto_kdf_derive_from_key(next_receive_key, 32, 1, ctx, receive_chain.key);

    memcpy(receive_chain.key, next_receive_key, 32);
    return msg_key;
}

std::tuple<unsigned char*, MessageHeader*> NewRatchet::progress_sending_ratchet() {
    auto message_key = new unsigned char[32];
    unsigned char next_send_key[32];
    const char *ctx = "DRSndKey"; // Use same context as in advance_send

    auto header = new MessageHeader();
    memcpy(header->dh_public, local_dh_public, 32);
    header->message_index = send_chain.index;
    header->prev_chain_length = receive_chain.index; // Set previous chain length

    crypto_kdf_derive_from_key(message_key, 32, 0, ctx, send_chain.key);
    crypto_kdf_derive_from_key(next_send_key, 32, 1, ctx, send_chain.key);

    memcpy(send_chain.key, next_send_key, 32);
    send_chain.index++;

    return std::make_tuple(message_key, header);
}

//remove this is for tesitng
const unsigned char *NewRatchet::get_current_dh_public() const {
    return local_dh_public;
}

std::tuple<int, int> NewRatchet::get_chain_lengths() {
    return std::make_tuple(send_chain.index, receive_chain.index);
}

void NewRatchet::serialise(std::ostream &out) const {
    out.write((char*)root_key, 32);
    out.write((char*)local_dh_public, 32);
    out.write((char*)local_dh_priv->data(), 32);
    out.write((char*)remote_dh_public, 32);

    out.write((char*)send_chain.key, 32);
    out.write((char*)&send_chain.index, sizeof(send_chain.index));

    out.write((char*)receive_chain.key, 32);
    out.write((char*)&receive_chain.index, sizeof(receive_chain.index));

    // serialize skipped_keys count
    uint32_t skipped_count = skipped_keys.size();
    out.write((char*)&skipped_count, sizeof(skipped_count));
    for (auto &pair : skipped_keys) {
        out.write((char*)&pair.first, sizeof(pair.first));
        out.write((char*)pair.second, 32);
    }

    out.write((char*)&due_to_send_new_dh, sizeof(due_to_send_new_dh));
    out.write((char*)&reversed, sizeof(reversed));
    out.write((char*)&prev_chain_length, sizeof(prev_chain_length));
}

void NewRatchet::deserialise(std::istream &in) {
    in.read((char*)root_key, 32);
    in.read((char*)local_dh_public, 32);

    unsigned char priv[32];
    in.read(reinterpret_cast<char*>(priv), 32);

    // Now create or reuse your buffer, copying raw data into it
    local_dh_priv = SecureMemoryBuffer::create(32);
    memcpy(local_dh_priv->data(), priv, 32);

    in.read((char*)remote_dh_public, 32);

    in.read((char*)send_chain.key, 32);
    in.read((char*)&send_chain.index, sizeof(send_chain.index));

    in.read((char*)receive_chain.key, 32);
    in.read((char*)&receive_chain.index, sizeof(receive_chain.index));

    uint32_t skipped_count;
    in.read((char*)&skipped_count, sizeof(skipped_count));
    
    skipped_keys.clear();
    for (uint32_t i = 0; i < skipped_count; ++i) {
        int key;
        auto val = std::make_unique<unsigned char[]>(32);
        in.read((char*)&key, sizeof(key));
        in.read((char*)val.get(), 32);
        skipped_keys[key] = val.release();  // Transfer ownership to the map
    }

    in.read((char*)&due_to_send_new_dh, sizeof(due_to_send_new_dh));
    in.read((char*)&reversed, sizeof(reversed));
    in.read((char*)&prev_chain_length, sizeof(prev_chain_length));
}

void NewRatchet::save(const std::string& username, const std::array<unsigned char, 32>& device_id) {
    std::ostringstream oss(std::ios::binary);
    serialise(oss);

    std::string str = oss.str();
    QByteArray bytes(str.data(), static_cast<int>(str.size()));

    auto nonce_data = std::make_unique<unsigned char[]>(CHA_CHA_NONCE_LEN);
    randombytes_buf(nonce_data.get(), CHA_CHA_NONCE_LEN);

    auto nonce_key = std::make_unique<unsigned char[]>(CHA_CHA_NONCE_LEN);
    randombytes_buf(nonce_key.get(), CHA_CHA_NONCE_LEN);

    auto encryption_key = SecureMemoryBuffer::create(32);
    crypto_stream_chacha20_keygen(encryption_key->data());

    auto copy_encryption_key = SecureMemoryBuffer::create(32);
    memcpy(copy_encryption_key->data(), encryption_key->data(), 32);

    auto encrypted_data = encrypt_bytes(bytes, std::move(copy_encryption_key), nonce_data.get());
    auto encrypted_encryption_key = encrypt_symmetric_key(encryption_key, nonce_key.get());

    save_ratchet_and_key_by_username_device(username, device_id, encrypted_data, nonce_data.get(), std::move(encrypted_encryption_key), nonce_key.get());
}






