//
// Created by Josh Sloggett on 27/05/2025.
//

#include "src/key_exchange/utils.h"
#include "SessionManager.h"
#include "src/sql/queries.h"
#include "src/structs/KeyBundle.h"

SessionManager::SessionManager() {}

SessionManager::~SessionManager() {
    for (auto& pair : identity_sessions) {
        delete pair.second;
    }
}

void SessionManager::import_sending_key_bundles(std::vector<SendingKeyBundle> request, unsigned char* my_identity_public, unsigned char* their_identity_public) {
    size_t identity_session_key_len = sizeof(my_identity_public) + sizeof(their_identity_public);
    unsigned char* identity_session_id = concat_ordered(my_identity_public, crypto_box_PUBLICKEYBYTES, their_identity_public, crypto_box_PUBLICKEYBYTES, identity_session_key_len);

    if (!identity_sessions[identity_session_id]) {
        // Create new identity session with sending key bundles
        identity_sessions[identity_session_id] = new IdentityCommunicationSession(
            request,  // sending key bundles
            {},       // empty receiving key bundles
            my_identity_public,
            their_identity_public
        );
    } else {
        // Update existing session with new sending key bundles
        identity_sessions[identity_session_id]->updateSendingSessionsFromKeyBundles(request);
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

