//
// Created by Josh Sloggett on 20/05/2025.
//

#ifndef X3DH_H

unsigned char* x3dh(
    const unsigned char* identity_key_public,
    const unsigned char* identity_key_private,
    const unsigned char* ephemeral_key_public,
    const unsigned char* ephemeral_key_private,
    const unsigned char* signed_prekey_public,
    const unsigned char* signed_prekey_private,
    const unsigned char* one_time_prekey_public,
    const unsigned char* one_time_prekey_private,
    const unsigned char* signed_prekey_signature,
    const unsigned char* ed25519_identity_key_public,
    bool is_initiator);
#define X3DH_H

#endif //X3DH_H
