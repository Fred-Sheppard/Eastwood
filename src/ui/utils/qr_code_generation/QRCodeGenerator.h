//
// Created by Josh Sloggett on 30/05/2025.
//

#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H
#include <QImage>
#include <QByteArray>

QImage getQRCodeImage(std::string input);

QImage getQRCodeForMyDevicePublicKey(const QByteArray public_key);

#endif //QRCODEGENERATOR_H
