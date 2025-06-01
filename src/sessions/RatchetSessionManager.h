//
// Created by Josh Sloggett on 01/06/2025.
//

#ifndef RATCHETSESSIONMANAGER_H
#define RATCHETSESSIONMANAGER_H
#include <map>

#include "src/key_exchange/NewRatchet.h"


class RatchetSessionManager{
public:
    RatchetSessionManager();

    // device id : <key, message header>
    std::map<unsigned char*, std::tuple<unsigned char*, MessageHeader*>> get_keys_for_message(std::string username);
private:
    // username : [ device_id : ratchet ]
    std::map<std::string, std::map<unsigned char*, std::unique_ptr<NewRatchet>>> ratchets;
};



#endif //RATCHETSESSIONMANAGER_H
