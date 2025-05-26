#include "DeviceCommunicationSession.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "utils.h"

static void print_key(const char* name, const unsigned char* key, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)key[i];
    std::cout << name << ": " << oss.str() << std::endl;
}

DeviceCommunicationSession::DeviceCommunicationSession() 
    : shared_secret(), ratchet(nullptr), device_session_id() {
    // Initialize base class - common functionality can be implemented here
    std::cout << "\n===== INITIALIZING DEVICE COMMUNICATION SESSION =====" << std::endl;
}

const std::vector<uint8_t>& DeviceCommunicationSession::getDeviceSessionId() const {
    return device_session_id;
}

DeviceSendingCommunicationSession::DeviceSendingCommunicationSession(
    const unsigned char* device_key_public,
    const unsigned char* device_key_private,
    const unsigned char* ephemeral_key_public,
    const unsigned char* ephemeral_key_private,
    const unsigned char* recipient_device_key_public,
    const unsigned char* recipient_signed_prekey_public,
    const unsigned char* recipient_onetime_prekey_public,
    const unsigned char* recipient_signed_prekey_signature,
    const unsigned char* recipient_ed25519_device_key_public
) : DeviceCommunicationSession() {

    size_t device_session_key_len = crypto_box_PUBLICKEYBYTES + crypto_box_PUBLICKEYBYTES;
    size_t out_len;
    unsigned char* session_id_ptr = concat_ordered(device_key_public, crypto_box_PUBLICKEYBYTES, 
                                                 recipient_device_key_public, crypto_box_PUBLICKEYBYTES, 
                                                 out_len);
    device_session_id = std::vector<uint8_t>(session_id_ptr, session_id_ptr + out_len);
    delete[] session_id_ptr;

    unsigned char* shared_secret_ptr = x3dh_initiator(
        device_key_private,
        ephemeral_key_private,
        recipient_device_key_public,
        recipient_signed_prekey_public,
        recipient_onetime_prekey_public,
        recipient_signed_prekey_signature,
        recipient_ed25519_device_key_public
    );
    shared_secret = std::vector<uint8_t>(shared_secret_ptr, shared_secret_ptr + crypto_kx_SESSIONKEYBYTES);
    delete[] shared_secret_ptr;

    // SEND POST REQUEST TO /handshake/identity_key
    // request contains;
    // recipients public signed prekey, recipients public onetime prekey
    // initiator (my) public device key, initiatior (my) public ephemeral key
    
    ratchet = std::make_unique<DoubleRatchet>(
        shared_secret.data(),
        recipient_signed_prekey_public,
        ephemeral_key_public,
        ephemeral_key_private
    );
}

const std::vector<uint8_t>& DeviceSendingCommunicationSession::getSharedSecret() const {
    return shared_secret;
}

DoubleRatchet* DeviceSendingCommunicationSession::getRatchet() {
    return ratchet.get();
}

DeviceReceivingCommunicationSession::DeviceReceivingCommunicationSession(
    const unsigned char* initiator_device_key_public,
    const unsigned char* initiator_ephemeral_key_public,
    const unsigned char* device_key_public,
    const unsigned char* device_key_private,
    const unsigned char* signed_prekey_public,
    const unsigned char* signed_prekey_private,
    const unsigned char* onetime_prekey_private
) : DeviceCommunicationSession() {
    size_t device_session_key_len = crypto_box_PUBLICKEYBYTES + crypto_box_PUBLICKEYBYTES;
    size_t out_len;
    unsigned char* session_id_ptr = concat_ordered(device_key_public, crypto_box_PUBLICKEYBYTES, 
                                                 initiator_device_key_public, crypto_box_PUBLICKEYBYTES, 
                                                 out_len);
    device_session_id = std::vector<uint8_t>(session_id_ptr, session_id_ptr + out_len);
    delete[] session_id_ptr;

    unsigned char* shared_secret_ptr = x3dh_responder(
        initiator_device_key_public,
        initiator_ephemeral_key_public,
        device_key_private,
        signed_prekey_private,
        onetime_prekey_private
    );
    shared_secret = std::vector<uint8_t>(shared_secret_ptr, shared_secret_ptr + crypto_kx_SESSIONKEYBYTES);
    delete[] shared_secret_ptr;
    
    ratchet = std::make_unique<DoubleRatchet>(
        shared_secret.data(),
        initiator_ephemeral_key_public,
        signed_prekey_public,
        signed_prekey_private
    );
}

const std::vector<uint8_t>& DeviceReceivingCommunicationSession::getSharedSecret() const {
    return shared_secret;
}

DoubleRatchet* DeviceReceivingCommunicationSession::getRatchet() {
    return ratchet.get();
}
