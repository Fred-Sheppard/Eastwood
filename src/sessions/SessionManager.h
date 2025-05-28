//
// Created by Josh Sloggett on 27/05/2025.
//

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "IdentityCommunicationSession.h"
#include "src/key_exchange/DoubleRatchet.h"
#include "src/structs/KeyBundle.h"

class SessionManager {
    public:
    SessionManager();
    ~SessionManager();

    void import_sending_key_bundles(std::vector<SendingKeyBundle> request, unsigned char* my_identity_public, unsigned char* their_identity_public);
    void routeToIdentity(DeviceMessage message, unsigned char* other_identity);

private:
    std::map<unsigned char*, IdentityCommunicationSession*> identity_sessions;
};

#endif //SESSIONMANAGER_H
