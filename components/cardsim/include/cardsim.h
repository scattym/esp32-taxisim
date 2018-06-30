//
// Created by sysadmin on 30/06/18.
//

#ifndef ESP_IDF_TEMPLATE_CARDSIM_H
#define ESP_IDF_TEMPLATE_CARDSIM_H

#include <stddef.h>

unsigned char * base64_encode(const unsigned char *src, size_t len, size_t *out_len);
unsigned char * base64_decode(const unsigned char *src, size_t len, size_t *out_len);
unsigned char* make_card_read(unsigned int *len);

#endif //ESP_IDF_TEMPLATE_CARDSIM_H
