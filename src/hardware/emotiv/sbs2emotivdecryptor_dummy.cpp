#include "sbs2emotivdecryptor.h"
#include <stdint.h>

Sbs2EmotivDecryptor::Sbs2EmotivDecryptor(QObject *parent) :
    QObject(parent)
{
    oRijndael = new CRijndael();
    serialNumber = new char[16];
    currentPacket = 0;
    key = new char[EMOKIT_KEYSIZE];
}

void Sbs2EmotivDecryptor::setSerialNumber(char *serialNumber_)
{
    for (int i=0; i<16; ++i)
    {
        serialNumber[i] = serialNumber_[i];
    }

    initialize();
}

void Sbs2EmotivDecryptor::setSerialNumber(QString serialNumber_)
{
    for (int i=0; i<16; ++i)
    {
        serialNumber[i] = serialNumber_.at(i).toAscii();
    }
    initialize();
}

void Sbs2EmotivDecryptor::initialize() {
    // emokit_get_crypto_key(dev_type);
    // FIXME dev_type can be found automatically as in the original emokit application
    emokit_get_crypto_key(EMOKIT_RESEARCH);

    // crypt key initialization
    oRijndael->MakeKey(reinterpret_cast<const char *>(key), CRijndael::sm_chain0, 16, 16);
}

void Sbs2EmotivDecryptor::decrypt(char cipher[32], char plain[32])
{
/*
    As found in emokit, the key to decrypt datas is made from the serial number.
    And then we'll use the Rijndael tool to decrypt datas using this key.
*/

    oRijndael->Decrypt(cipher, plain, 32, CRijndael::ECB);

    /*plain[0] = currentPacket;
    for(int i=1;i<32;++i){
        plain[i] = qrand()%RAND_MAX*10;
    }*/

    currentPacket = (currentPacket+1)%129;
}

/*
    This function was found by emokit developpers
*/
void Sbs2EmotivDecryptor::emokit_get_crypto_key(int dev_type) {
    unsigned char type = (unsigned char) dev_type;
    unsigned int l = 16;
    type &= 0xF;
    type = (type == 0);

    key[0] = (uint8_t)serialNumber[l-1];
    key[1] = '\0';
    key[2] = (uint8_t)serialNumber[l-2];
    if(type) {
        key[3] = 'H';
        key[4] = (uint8_t)serialNumber[l-1];
        key[5] = '\0';
        key[6] = (uint8_t)serialNumber[l-2];
        key[7] = 'T';
        key[8] = (uint8_t)serialNumber[l-3];
        key[9] = '\x10';
        key[10] = (uint8_t)serialNumber[l-4];
        key[11] = 'B';
    }
    else {
        key[3] = 'T';
        key[4] = (uint8_t)serialNumber[l-3];
        key[5] = '\x10';
        key[6] = (uint8_t)serialNumber[l-4];
        key[7] = 'B';
        key[8] = (uint8_t)serialNumber[l-1];
        key[9] = '\0';
        key[10] = (uint8_t)serialNumber[l-2];
        key[11] = 'H';
    }
    key[12] = (uint8_t)serialNumber[l-3];
    key[13] = '\0';
    key[14] = (uint8_t)serialNumber[l-4];
    key[15] = 'P';
}
