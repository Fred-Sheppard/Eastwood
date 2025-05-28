//
// Created by Josh Sloggett on 22/05/2025.
//

#ifndef IDENTITYCOMMUNICATIONSESSION_H
#define IDENTITYCOMMUNICATIONSESSION_H

#include <vector>
#include <map>
#include "DeviceCommunicationSession.h"
#include "../key_exchange/DoubleRatchet.h"
#include "../key_exchange/utils.h"
#include "src/structs/KeyBundle.h"

class IdentityCommunicationSession {
    // identity session key = two identity keys together in alphabetical order hashed
    // this is our out of band code to verify
public:
    IdentityCommunicationSession(std::vector<SendingKeyBundle>, std::vector<ReceivingKeyBundle>, unsigned char* , unsigned char*);
    // use vector of keybundles to establish per device sessions
    // ensure to make sure the device session does not already exist
    // device session id of two device ids in alphabetical order hashed
    ~IdentityCommunicationSession();

    void message_send(unsigned char* message);
    void message_receive(DeviceMessage message);

    // Public methods for testing
    const std::map<unsigned char*, DeviceCommunicationSession*>& getDeviceSessions() const { return device_sessions; }
    void updateSendingSessionsFromKeyBundles(std::vector<SendingKeyBundle> key_bundles);
    void updateReceivingSessionsFromKeyBundles(std::vector<ReceivingKeyBundle> key_bundles);

private:
    unsigned char* identity_session_id;
    std::map<unsigned char*, DeviceCommunicationSession*> device_sessions;
    unsigned char* my_device_key_public;

    void createSendingSessionFromKeyBundle(SendingKeyBundle);
    void createReceivingSessionFromKeyBundle(ReceivingKeyBundle key_bundle);
};

#endif //IDENTITYCOMMUNICATIONSESSION_H
