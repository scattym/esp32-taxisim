/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "gwp5043.h"
#include "touchpad_api.h"
#include "simple_wifi.h"

static const int RX_BUF_SIZE = 1024;
static const char* TAG = "taxisim";

#define TAXI_UART UART_NUM_1
#define TAXI_TXD_PIN (GPIO_NUM_17) // pin 7 // tx2
#define TAXI_RXD_PIN (GPIO_NUM_16) // pin 6 // rx2
#define BLINK_GPIO (GPIO_NUM_2) // pin 4 // d2
#define TOUCH_PAD_PIN (GPIO_NUM_4) // pin 5 // d4
#define ENGINE_PIN (GPIO_NUM_22) // pin 14 // d22
#define SOS_PIN (GPIO_NUM_23) // pin 15 // d23
vacancy_command_t vacancyCommand;
occupied_command_t occupiedCommand;
print_command_t printCommand;
int currentCommand = 4;

SemaphoreHandle_t currentCommandSemaphore = NULL;
unsigned long previousTouchEvent = 0;

void init() {
    //touch_init();
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(TAXI_UART, &uart_config);
    uart_set_pin(TAXI_UART, TAXI_TXD_PIN, TAXI_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(TAXI_UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    currentCommandSemaphore = xSemaphoreCreateMutex();
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(TAXI_UART, data, len);
    ESP_LOGI(TAG, "Wrote %d bytes", txBytes);
    return txBytes;
}

int sendByteArray(const char* logName, const char* data, int len) {
    const int txBytes = uart_write_bytes(TAXI_UART, data, len);
    ESP_LOGI(TAG, "Wrote %d bytes", txBytes);
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, len, ESP_LOG_INFO);
    return txBytes;
}

static void tx_task()
{
    int quit = 0;
    int vacancy_length = make_vacancy_command(&vacancyCommand);
    int print_length = make_print_command(&printCommand);
    int occupied_length = make_occupied_command(&occupiedCommand);
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    int copyOfCurrentCommand = 0;
    int loopCounter = 0;
    int loopRegion = 0;
    while( !quit ) {
        xSemaphoreTake(currentCommandSemaphore, portMAX_DELAY);
        copyOfCurrentCommand = currentCommand;
        xSemaphoreGive(currentCommandSemaphore);
        if( copyOfCurrentCommand == 4 ) {
            loopRegion = loopCounter % 1200;
            if( loopRegion < 100 )
            {
                // Nothing to do car empty
                copyOfCurrentCommand = 0;
            }
            else if( loopRegion < 600 )
            {
                // Car is occupied for 100-600
                copyOfCurrentCommand = 1;
            }
            else if( loopRegion == 600 )
            {
                // Trip finished at 1200
                copyOfCurrentCommand = 2;
            }
            else if( loopRegion == 620 )
            {
                // Print receipt at 1202
                copyOfCurrentCommand = 3;
            }
        }
        switch( copyOfCurrentCommand ) {
            case 0:
                break;
            case 1:
                sendByteArray(TX_TASK_TAG, (const char*)&occupiedCommand, occupied_length);
                break;
            case 2:
                sendByteArray(TX_TASK_TAG, (const char*)&vacancyCommand, vacancy_length);
                break;
            case 3:
                sendByteArray(TX_TASK_TAG, (const char*)&printCommand, print_length);
                break;
            default:
                break;
        }
        loopCounter++;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void rx_task()
{
    int quit = 0;
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while( !quit ) {
        const int rxBytes = uart_read_bytes(TAXI_UART, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
    }
    free(data);
}

void toggle_command(int pinNumber) {
    unsigned long now = xTaskGetTickCount();
    // ESP_LOGI("TOGGLE", "Toggle command at time %lu on pin %d", now, pinNumber);
    if( now - previousTouchEvent < 200 ) {
        // ESP_LOGI("TOGGLE", "Double tap!");
    } else {
        xSemaphoreTake(currentCommandSemaphore, portMAX_DELAY);
        currentCommand = (currentCommand + 1) % 5;
        xSemaphoreGive(currentCommandSemaphore);
        // ESP_LOGI("TOGGLE", "Toggle command value is now %d", currentCommand);
    }
    previousTouchEvent = xTaskGetTickCount();
}

void blink_task(void *pvParameter)
{
    int quit = 0;
    int copyOfCurrentCommand = 0;

    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while( !quit )
    {
        xSemaphoreTake(currentCommandSemaphore, portMAX_DELAY);
        copyOfCurrentCommand = currentCommand;
        xSemaphoreGive(currentCommandSemaphore);
        /* Blink off (output low) */
        if( copyOfCurrentCommand == 0 )
        {
            // Mode 0 means send no commands. Turn off led.
            gpio_set_level(BLINK_GPIO, 0);
        }
        else if( copyOfCurrentCommand == 4 )
        {
            // Mode 4 is send commands based on timer. Turn on led.
            gpio_set_level(BLINK_GPIO, 1);
        }
        else
        {
            // Modes 1-3 are sending each taxi messages every second.
            // Blink led to indicate modes 1 - 3.
            for( int i=0; i < copyOfCurrentCommand; i++ )
            {
                gpio_set_level(BLINK_GPIO, 1);
                vTaskDelay(300 / portTICK_PERIOD_MS);
                gpio_set_level(BLINK_GPIO, 0);
                vTaskDelay(300 / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}


void gpio_task(void *pvParameter)
{
    int quit = 0;
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(ENGINE_PIN);
    gpio_pad_select_gpio(SOS_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(ENGINE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SOS_PIN, GPIO_MODE_OUTPUT);
    while( !quit )
    {
        gpio_set_level(ENGINE_PIN, 1);
        vTaskDelay(600000 / portTICK_PERIOD_MS);
        gpio_set_level(SOS_PIN, 1);
        vTaskDelay(600000 / portTICK_PERIOD_MS);
        gpio_set_level(SOS_PIN, 0);
        vTaskDelay(600000 / portTICK_PERIOD_MS);
        gpio_set_level(ENGINE_PIN, 0);
        vTaskDelay(600000 / portTICK_PERIOD_MS);
    }
}


void app_main()
{
    init();
    setEmitEventFunctionPtr(&toggle_command);
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);
    xTaskCreate(&gpio_task, "gpio_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);

    //xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
    touchpad_isr_init();
    simple_wifi_init();


}
