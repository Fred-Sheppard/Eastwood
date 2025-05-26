//
// Created by Josh Sloggett on 22/05/2025.
//

#ifndef IDENTITYCOMMUNICATIONSESSION_H
#define IDENTITYCOMMUNICATIONSESSION_H
#include <vector>
#include "DeviceCommunicationSession.h"
#include <map>
#include <string>

struct keyBundle {
    bool isSending;
    unsigned char* device_key_public[crypto_box_PUBLICKEYBYTES];
    unsigned char* device_key_private[crypto_box_SECRETKEYBYTES];

    unsigned char* ed25519_device_key_public[crypto_sign_PUBLICKEYBYTES];
    unsigned char* ed25519_device_key_private[crypto_sign_SECRETKEYBYTES];

    unsigned char* ephemeral_key_public[crypto_box_PUBLICKEYBYTES];
    unsigned char* ephemeral_key_private[crypto_box_SECRETKEYBYTES];

    unsigned char* signed_prekey_public[crypto_box_PUBLICKEYBYTES];
    unsigned char* signed_prekey_private[crypto_box_SECRETKEYBYTES];
    unsigned char* signed_prekey_signature[crypto_sign_BYTES];

    unsigned char* onetime_prekey_public[crypto_box_PUBLICKEYBYTES];
    unsigned char* onetime_prekey_private[crypto_box_SECRETKEYBYTES];
};

class IdentityCommunicationSession {
    // identity session key = two identity keys together in alphabetical order hashed
    // this is our out of band code to verify
public:
    IdentityCommunicationSession(const keyBundle &myBundle, const std::vector<keyBundle> &, const unsigned char* , const unsigned char*);
    // use vector of keybundles to establish per device sessions
    // ensure to make sure the device session does not already exist
    // device session id of two device ids in alphabetical order hashed
    ~IdentityCommunicationSession();

    void add_device_session(const std::string& device_id, DeviceCommunicationSession* session);
    void remove_device_session(const std::string& device_id);
    void send_msg(const std::string& device_id, std::vector<unsigned char> message);
    void recv_msg(const DeviceMessage &msg);

private:
    keyBundle myBundle;
    unsigned char* identity_session_id;
    std::map<std::string, DeviceCommunicationSession*> device_sessions;

    void createSessionFromKeyBundle(const keyBundle &);
    void updateSessionsFromKeyBundles(const std::vector<keyBundle> &);
};



#endif //IDENTITYCOMMUNICATIONSESSION_H
