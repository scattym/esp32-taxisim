//
// Created by sysadmin on 30/06/18.
//
#include <cardsim.h>

unsigned char* make_card_read(unsigned int *len) {
    unsigned char cardReadB64[] = "AiUgIF5BQkNERUZHSEkgSktMTU5PUCBNUi5eXj87NjAwNzU1NTEwMDUwMDQ2MTE1Nz05OTk5MTk3NzA3MDQ9PysgICAgICAgICAgICAgMzEwMCAgICAgICAgICAgIDEgICAgICAgICAgICA1ODAwNTU2MyAgMDAxMDEgICAgICAgICAgICAgICAgICAgICA/DQMK";

    unsigned char* cardReadBytes = base64_decode(cardReadB64, 196, len);
    return cardReadBytes;
}