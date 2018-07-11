//
// Created by sysadmin on 22/06/18.
//

#ifndef ESP_IDF_TEMPLATE_GWP5043_H
#define ESP_IDF_TEMPLATE_GWP5043_H
typedef struct {
    unsigned char stx;
    unsigned char commandNumber;
    unsigned char length[4];
    unsigned char datetime[4];
    unsigned char meterSerialNumber[8];
    unsigned char carSpeed[2];
    unsigned char tbd[6];
    unsigned char currentPassengerDistance[4];
    unsigned char currentPassengerTravelTime[2];
    unsigned char currentPassengerWaitTime[2];
    unsigned char currentTotalFare[2];
    unsigned char etx;
    unsigned char dle;
} occupied_command_t;

typedef struct {
    unsigned char stx;
    unsigned char commandNumber;
    unsigned char length[4];
    unsigned char datetime[4];
    unsigned char meterSerialNumber[8];
    unsigned char passengerOnBoardTime[4];
    unsigned char passengerExitTime[4];
    unsigned char passengerTravelDistance[4];
    unsigned char passengerTravelTime[2];
    unsigned char passengerWaitTime[2];
    unsigned char totalFare[2];
    unsigned char carSpeed[2];
    unsigned char passengerOnBoardLongitude[4];
    unsigned char passengerOnBoardLatitude[4];
    unsigned char passengerExitLongitude[4];
    unsigned char passengerExitLatitude[4];
    unsigned char taxiMeterDeviceNumber[8];
    unsigned char receiptNumber[4];
    unsigned char totalFare4[4];
    unsigned char driverIdentificationNumber[16];
    unsigned char kConstantSetting[4];
    unsigned char etx;
    unsigned char dle;
} vacancy_command_t;

typedef struct {
    unsigned char stx;
    unsigned char commandNumber;
    unsigned char length[4];
    unsigned char datetime[4];
    unsigned char meterSerialNumber[8];
    unsigned char passengerOnBoardTime[4];
    unsigned char passengerExitTime[4];
    unsigned char passengerTravelDistance[4];
    unsigned char passengerTravelTime[2];
    unsigned char passengerWaitTime[2];
    unsigned char totalFare[2];
    unsigned char carSpeed[2];
    unsigned char passengerOnBoardLongitude[4];
    unsigned char passengerOnBoardLatitude[4];
    unsigned char passengerExitLongitude[4];
    unsigned char passengerExitLatitude[4];
    unsigned char taxiMeterDeviceNumber[8];
    unsigned char receiptNumber[4];
    unsigned char totalFare4[4];
    unsigned char driverIdentificationNumber[16];
    unsigned char kConstantSetting[4];
    unsigned char etx;
    unsigned char dle;
} print_command_t;

int make_occupied_command(occupied_command_t* destination);
int make_print_command(print_command_t* destination);
int make_vacancy_command(vacancy_command_t* destination);
void set_timestamp_offset_vacancy(vacancy_command_t* destination, int offset);
void set_onboard_offset_vacancy(vacancy_command_t* destination, int offset);
void set_exit_offset_vacancy(vacancy_command_t* destination, int offset);
void set_timestamp_offset_print(print_command_t* destination, int offset);
void set_onboard_offset_print(print_command_t* destination, int offset);
void set_exit_offset_print(print_command_t* destination, int offset);
void set_timestamp_offset_occupied(occupied_command_t* destination, int offset);

#endif //ESP_IDF_TEMPLATE_GWP5043_H
