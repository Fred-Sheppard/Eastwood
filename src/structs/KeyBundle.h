//
// Created by Josh Sloggett on 28/05/2025.
//

#ifndef KEYBUNDLE_H
#define KEYBUNDLE_H

// key bundles needed for sending
struct SendingKeyBundle {
    unsigned char* my_device_private;
    unsigned char* my_ephemeral_public;
    unsigned char* my_ephemeral_private;
    unsigned char* their_device_public;
    unsigned char* their_signed_public;
    unsigned char* their_onetime_public;
    unsigned char* their_signature;
    unsigned char* their_ed25519_public;
};

// key bundles needed for receiving
struct ReceivingKeyBundle {
    unsigned char* their_device_public;
    unsigned char* their_ephemeral_public;
    unsigned char* my_device_private;
    unsigned char* my_signed_public;
    unsigned char* my_signed_private;
    unsigned char* my_onetime_private;
};

#endif //KEYBUNDLE_H
