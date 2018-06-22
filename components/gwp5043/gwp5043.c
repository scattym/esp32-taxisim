#include <malloc.h>
#include <stdio.h>
#include "include/gwp5043.h"

//
// Created by sysadmin on 22/06/18.
//



int char_4_to_int(unsigned char* destination) {
    int returnVal = 0;
    returnVal += destination[0] << 24;
    returnVal += destination[1] << 16;
    returnVal += destination[2] << 8;
    returnVal += destination[3];
    return returnVal;
}

int int_to_char_4(unsigned int input, unsigned char* destination) {
    destination[0] = (unsigned char) (input >> 24 & 0xFF);
    destination[1] = (unsigned char) (input >> 16 & 0xFF);
    destination[2] = (unsigned char) (input >> 8 & 0xFF);
    destination[3] = (unsigned char) (input & 0xFF);
    return 0;
}

int int_to_char_2(unsigned int input, unsigned char* destination) {
    destination[0] = (unsigned char) (input >> 8 & 0xFF);
    destination[1] = (unsigned char) (input & 0xFF);
    return 0;
}

int make_occupied_command(occupied_command_t* destination) {
    destination->stx = 0x02;
    destination->etx = 0x03;
    destination->dle = 0x10;
    destination->commandNumber = 0xC2;
    int_to_char_4(30, destination->length);

    int_to_char_4(1529665690, destination->datetime);
    for( int i = 0; i < 8; i++ ) {
        destination->meterSerialNumber[i] = 0x30 + i;
    }
    int_to_char_2(55, destination->carSpeed);
    for( int i = 0; i < 6; i++ ) {
        destination->tbd[i] = 0;
    }
    int_to_char_4(1000, destination->currentPassengerDistance);
    int_to_char_2(3600, destination->currentPassengerTravelTime);
    int_to_char_2(1800, destination->currentPassengerWaitTime);
    int_to_char_2(60, destination->currentTotalFare);
    return sizeof(occupied_command_t);

}

int make_print_command(print_command_t* destination) {
    destination->stx = 0x02;
    destination->etx = 0x03;
    destination->dle = 0x10;
    destination->commandNumber = 0xB4;
    int_to_char_4(30, destination->length);

    int_to_char_4(1529665690, destination->datetime);
    for( int i = 0; i < 8; i++ ) {
        destination->meterSerialNumber[i] = 0x30 + i;
    }
    int_to_char_2(55, destination->carSpeed);
    int_to_char_4(1529665690, destination->passengerOnBoardTime);
    int_to_char_4(1529665690, destination->passengerExitTime);
    int_to_char_4(1000, destination->passengerTravelDistance);
    int_to_char_2(3600, destination->passengerTravelTime);
    int_to_char_2(1800, destination->passengerWaitTime);
    int_to_char_2(60, destination->totalFare);
    int_to_char_2(100, destination->carSpeed);
    int_to_char_4(0, destination->passengerOnBoardLatitude);
    int_to_char_4(0, destination->passengerOnBoardLongitude);
    int_to_char_4(0, destination->passengerExitLatitude);
    int_to_char_4(0, destination->passengerExitLongitude);
    for( int i = 0; i < 8; i++ ) {
        destination->taxiMeterDeviceNumber[i] = 61 + i;
    }
    int_to_char_4(1010101, destination->receiptNumber);
    int_to_char_4(60, destination->totalFare4);
    for( int i = 0; i < 16; i++ ) {
        destination->driverIdentificationNumber[i] = 71 + i;
    }
    int_to_char_4(98989898, destination->kConstantSetting);
    return sizeof(print_command_t);
}

int make_vacancy_command(vacancy_command_t* destination) {
    destination->stx = 0x02;
    destination->etx = 0x03;
    destination->dle = 0x10;
    destination->commandNumber = 0xC1;
    int_to_char_4(30, destination->length);

    int_to_char_4(1529665690, destination->datetime);
    for( int i = 0; i < 8; i++ ) {
        destination->meterSerialNumber[i] = 0x30 + i;
    }
    int_to_char_2(55, destination->carSpeed);
    int_to_char_4(1529665690, destination->passengerOnBoardTime);
    int_to_char_4(1529665690, destination->passengerExitTime);
    int_to_char_4(1000, destination->passengerTravelDistance);
    int_to_char_2(3600, destination->passengerTravelTime);
    int_to_char_2(1800, destination->passengerWaitTime);
    int_to_char_2(60, destination->totalFare);
    int_to_char_2(100, destination->carSpeed);
    int_to_char_4(0, destination->passengerOnBoardLatitude);
    int_to_char_4(0, destination->passengerOnBoardLongitude);
    int_to_char_4(0, destination->passengerExitLatitude);
    int_to_char_4(0, destination->passengerExitLongitude);
    for( int i = 0; i < 8; i++ ) {
        destination->taxiMeterDeviceNumber[i] = 61 + i;
    }
    int_to_char_4(1010101, destination->receiptNumber);
    int_to_char_4(60, destination->totalFare4);
    for( int i = 0; i < 16; i++ ) {
        destination->driverIdentificationNumber[i] = 71 + i;
    }
    int_to_char_4(98989898, destination->kConstantSetting);
    return sizeof(vacancy_command_t);
}

void print_command(void *gwpPayload, int num_bytes) {
    for( int i=0; i < num_bytes; i++ ) {
        printf("%c", ((unsigned char *)gwpPayload)[i]);
    }
}

int main() {
    occupied_command_t occupied_command;
    make_occupied_command(&occupied_command);
    print_command(&occupied_command, sizeof(occupied_command_t));

    vacancy_command_t vac;
    make_vacancy_command(&vac);
    print_command(&vac, sizeof(vacancy_command_t));

    print_command_t print;
    make_print_command(&print);
    print_command(&print, sizeof(print));

    return 0;
}