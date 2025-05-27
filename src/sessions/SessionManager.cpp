//
// Created by Josh Sloggett on 27/05/2025.
//

#include "src/key_exchange/utils.h"
#include "SessionManager.h"
#include "src/sql/queries.h"

SessionManager::SessionManager() {}

SessionManager::~SessionManager() {
    for (auto& pair : identity_sessions) {
        delete pair.second;
    }
}

void SessionManager::import_key_bundles(keyBundleRequest request) {
    size_t identity_session_key_len = sizeof(request.my_identity_public) + sizeof(request.their_identity_public);
    unsigned char* identity_session_id = concat_ordered(request.my_identity_public, crypto_box_PUBLICKEYBYTES, request.their_identity_public, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    if (!identity_sessions[identity_session_id]) {
        keyBundle my_key_bundle;
        // TODO:: fetch device keys from db
        identity_sessions[identity_session_id] = new IdentityCommunicationSession(my_key_bundle, request.key_bundles, request.my_identity_public, request.their_identity_public);
    } else {
        identity_sessions[identity_session_id]->updateSessionsFromKeyBundles(request.key_bundles);
    }
}

void SessionManager::routeToIdentity(DeviceMessage message, unsigned char* other_identity) {
    std::tuple<QByteArray, QByteArray> keypair = get_keypair("identity");
    QByteArray identity_key_ba = std::get<0>(keypair);
    unsigned char* identity_key = new unsigned char[identity_key_ba.size()];
    memcpy(identity_key, identity_key_ba.data(), identity_key_ba.size());

    size_t identity_session_key_len = crypto_box_PUBLICKEYBYTES * 2;
    unsigned char* identity_session_id = new unsigned char[identity_session_key_len];
    memcpy(identity_session_id, identity_key, crypto_box_PUBLICKEYBYTES);
    memcpy(identity_session_id + crypto_box_PUBLICKEYBYTES, other_identity, crypto_box_PUBLICKEYBYTES);

    auto it = identity_sessions.find(identity_session_id);
    if (it == identity_sessions.end()) {
        delete[] identity_key;
        delete[] identity_session_id;
        throw std::runtime_error("Identity session does not exist");
    }

    it->second->message_receive(message);
    delete[] identity_key;
    delete[] identity_session_id;
}

