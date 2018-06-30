//
// Created by sysadmin on 30/06/18.
//
#include <cardsim.h>

unsigned char* make_card_read(unsigned int *len) {
    unsigned char cardReadB64[] = "***REMOVED***";

    unsigned char* cardReadBytes = base64_decode(cardReadB64, 196, len);
    return cardReadBytes;
}