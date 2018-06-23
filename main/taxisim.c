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

#define TXD_PIN (GPIO_NUM_14)
#define RXD_PIN (GPIO_NUM_15)
#define BLINK_GPIO (GPIO_NUM_2)
vacancy_command_t vacancyCommand;
occupied_command_t occupiedCommand;
print_command_t printCommand;
int currentCommand = 0;

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
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    currentCommandSemaphore = xSemaphoreCreateMutex();
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(TAG, "Wrote %d bytes", txBytes);
    return txBytes;
}

int sendByteArray(const char* logName, const char* data, int len) {
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
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
    while( !quit ) {
        xSemaphoreTake(currentCommandSemaphore, portMAX_DELAY);
        switch( currentCommand ) {
            case 0:
                break;
            case 1:
                sendByteArray(TX_TASK_TAG, (const char*)&vacancyCommand, vacancy_length);
                break;
            case 2:
                sendByteArray(TX_TASK_TAG, (const char*)&printCommand, print_length);
                break;
            case 3:
                sendByteArray(TX_TASK_TAG, (const char*)&occupiedCommand, occupied_length);
                break;
        }

        xSemaphoreGive(currentCommandSemaphore);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

static void rx_task()
{
    int quit = 0;
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while( !quit ) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
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
        currentCommand = (currentCommand + 1) % 4;
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
        for( int i=0; i < copyOfCurrentCommand; i++ )
        {
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(300 / portTICK_PERIOD_MS);
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(300 / portTICK_PERIOD_MS);

        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}


void app_main()
{
    simple_wifi_init();
    init();
    setEmitEventFunctionPtr(&toggle_command);
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);

    //xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
    touchpad_isr_init();


}
