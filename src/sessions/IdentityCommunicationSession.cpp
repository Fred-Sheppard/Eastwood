//
// Created by Josh Sloggett on 22/05/2025.
//
#include "IdentityCommunicationSession.h"

#include <QByteArray>

#include "../key_exchange/utils.h"
#include "src/endpoints/endpoints.h"
#include "src/sql/queries.h"

IdentityCommunicationSession::IdentityCommunicationSession(std::vector<SendingKeyBundle> sending_key_bundles, std::vector<ReceivingKeyBundle> receiving_key_bundles,
                                                           unsigned char* public_identity_key_1, unsigned char* public_identity_key_2) {
    // out of band verification between users
    size_t identity_session_key_len = sizeof(public_identity_key_1) + sizeof(public_identity_key_2);
    identity_session_id = concat_ordered(public_identity_key_1, crypto_box_PUBLICKEYBYTES, public_identity_key_2, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    // Fetch device keys from database
    std::tuple<QByteArray, QByteArray> device_keypair = get_keypair("device");
    QByteArray device_public = std::get<0>(device_keypair);

    // Allocate and copy device keys
    my_device_key_public = new unsigned char[crypto_box_PUBLICKEYBYTES];
    memcpy(my_device_key_public, device_public.data(), crypto_box_PUBLICKEYBYTES);

    if (!sending_key_bundles.empty()) {
        updateSendingSessionsFromKeyBundles(sending_key_bundles);
    }
    if (!receiving_key_bundles.empty()) {
        // update receiving
    }
}

void IdentityCommunicationSession::updateSendingSessionsFromKeyBundles(std::vector<SendingKeyBundle> key_bundles) {
    // create session for key bundle
    for (const auto & key_bundle : key_bundles) {
        createSendingSessionFromKeyBundle(key_bundle);
    }
}

void IdentityCommunicationSession::updateReceivingSessionsFromKeyBundles(std::vector<ReceivingKeyBundle> key_bundles) {
    // create session for key bundle
    for (const auto & key_bundle : key_bundles) {
        createReceivingSessionFromKeyBundle(key_bundle);
    }
}

void IdentityCommunicationSession::createSendingSessionFromKeyBundle(SendingKeyBundle key_bundle) {
    //1. compute device_session_id
    size_t identity_session_key_len = sizeof(my_device_key_public) + sizeof(key_bundle.their_device_public);
    unsigned char* device_session_id_new = concat_ordered(my_device_key_public, crypto_box_PUBLICKEYBYTES, key_bundle.their_device_public, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    //2. verify if device_session_id doesnt exist already
    if (!device_sessions[device_session_id_new]) {
        //3. create new device session
        post_handshake_device(
                key_bundle.their_device_public,  // recipient's device key
                key_bundle.their_signed_public,  // recipient's signed prekey
                key_bundle.their_onetime_public,  // recipient's one-time prekey
                my_device_key_public,  // my device key
                key_bundle.my_ephemeral_public  // my ephemeral key
            );

        device_sessions[device_session_id_new] = new DeviceSendingCommunicationSession(
            my_device_key_public,
            key_bundle.my_device_private,
            key_bundle.my_ephemeral_public,
            key_bundle.my_ephemeral_private,
            key_bundle.their_device_public,
            key_bundle.their_signed_public,
            key_bundle.their_onetime_public,
            key_bundle.their_signature,
            key_bundle.their_ed25519_public
        );
    }
}

void IdentityCommunicationSession::createReceivingSessionFromKeyBundle(ReceivingKeyBundle key_bundle) {
    //1. compute device_session_id
    size_t identity_session_key_len = sizeof(my_device_key_public) + sizeof(key_bundle.their_device_public);
    unsigned char* device_session_id_new = concat_ordered(my_device_key_public, crypto_box_PUBLICKEYBYTES, key_bundle.their_device_public, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    //2. verify if device_session_id doesnt exist already
    if (!device_sessions[device_session_id_new]) {
        device_sessions[device_session_id_new] = new DeviceReceivingCommunicationSession(
                key_bundle.their_device_public,
                key_bundle.their_ephemeral_public,
                my_device_key_public,
                key_bundle.my_device_private,
                key_bundle.my_signed_public,
                key_bundle.my_signed_private,
                key_bundle.my_onetime_private
            );
    }
}

IdentityCommunicationSession::~IdentityCommunicationSession() {
    // Clean up all device sessions
    for (auto& pair : device_sessions) {
        delete pair.second;
    }
    device_sessions.clear();
    
    // Clean up identity session ID
    if (identity_session_id) {
        delete[] identity_session_id;
    }
}

void IdentityCommunicationSession::message_send(unsigned char* message) {
    for (auto&[fst, snd] : device_sessions) {
        snd->message_send(message);
    }
}

void IdentityCommunicationSession::message_receive(DeviceMessage message) {
    size_t identity_session_key_len = sizeof(my_device_key_public) + sizeof(message.header->device_id);
    unsigned char* device_session_id_new = concat_ordered(my_device_key_public, crypto_box_PUBLICKEYBYTES, message.header->device_id, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    device_sessions[device_session_id_new]->message_receive(message);
}

#include "IdentityCommunicationSession.h"
