/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <driver/rmt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"
#include "dht11.h"

static const char* TAG = "tempmon";

#define BLINK_GPIO_1 (GPIO_NUM_15) // pin 4 // d2
#define BLINK_GPIO_2 (GPIO_NUM_2)
#define BLINK_GPIO_3 (GPIO_NUM_4)
#define BLINK_SLEEP_TIME 500
#define PULL_MODE GPIO_PULLUP_ENABLE
#define BLINK_GPIO_MODE GPIO_MODE_INPUT_OUTPUT_OD
void init() {
    gpio_pad_select_gpio(BLINK_GPIO_1);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_1, BLINK_GPIO_MODE);
    gpio_set_pull_mode(BLINK_GPIO_1, PULL_MODE);
    gpio_pad_select_gpio(BLINK_GPIO_2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_2, BLINK_GPIO_MODE);
    gpio_set_pull_mode(BLINK_GPIO_2, PULL_MODE);
    gpio_pad_select_gpio(BLINK_GPIO_3);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO_3, BLINK_GPIO_MODE);
    gpio_set_pull_mode(BLINK_GPIO_3, PULL_MODE);
    setDHTPin(GPIO_NUM_26);

}

int gpio_pins[] = {GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4};

/*********************************************************************************
 * RMT receiver initialization
 *********************************************************************************/
static void dht11_rmt_rx_init(int gpio_pin, int channel)
{
    const int RMT_CLK_DIV            = 80;     /*!< RMT counter clock divider */
    const int RMT_TICK_10_US         = (80000000/RMT_CLK_DIV/100000);   /*!< RMT counter value for 10 us.(Source clock is APB clock) */
    const int  rmt_item32_tIMEOUT_US = 1000;   /*!< RMT receiver timeout value(us) */

    rmt_config_t rmt_rx;
    rmt_rx.gpio_num                      = gpio_pin;
    rmt_rx.channel                       = channel;
    rmt_rx.clk_div                       = RMT_CLK_DIV;
    rmt_rx.mem_block_num                 = 1;
    rmt_rx.rmt_mode                      = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en           = false;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold      = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
}

/*********************************************************************************
 * Processing the pulse data into temp and humidity
 *********************************************************************************/
static int parse_items(rmt_item32_t* item, int item_num, int *humidity, int *temp_x10)
{
    int i=0;
    unsigned rh = 0, temp = 0, checksum = 0;

    ///////////////////////////////
    // Check we have enough pulses
    ///////////////////////////////
    if(item_num < 42)  return 0;

    ///////////////////////////////////////
    // Skip the start of transmission pulse
    ///////////////////////////////////////
    item++;

    ///////////////////////////////
    // Extract the humidity data
    ///////////////////////////////
    for(i = 0; i < 16; i++, item++)
        rh = (rh <<1) + (item->duration1 < 35 ? 0 : 1);

    ///////////////////////////////
    // Extract the temperature data
    ///////////////////////////////
    for(i = 0; i < 16; i++, item++)
        temp = (temp <<1) + (item->duration1 < 35 ? 0 : 1);

    ///////////////////////////////
    // Extract the checksum
    ///////////////////////////////
    for(i = 0; i < 8; i++, item++)
        checksum = (checksum <<1) + (item->duration1 < 35 ? 0 : 1);

    ///////////////////////////////
    // Check the checksum
    ///////////////////////////////
    if((((temp>>8) + temp + (rh>>8) + rh)&0xFF) != checksum) {
        printf("Checksum failure %4X %4X %2X\n", temp, rh, checksum);
        return 0;
    }

    ///////////////////////////////
    // Store into return values
    ///////////////////////////////
    *humidity = rh>>8;
    *temp_x10 = (temp>>8)*10+(temp&0xFF);
    return 1;
}

/*********************************************************************************
 * Use the RMT receiver to get the DHT11 data
 *********************************************************************************/
static int dht11_rmt_rx(int gpio_pin, int rmt_channel,
                        int *humidity, int *temp_x10)
{
    RingbufHandle_t rb = NULL;
    size_t rx_size = 0;
    rmt_item32_t* item;
    int rtn = 0;

    //get RMT RX ringbuffer
    rmt_get_ringbuf_handle(rmt_channel, &rb);
    if(!rb)
        return 0;

    //////////////////////////////////////////////////
    // Send the 20ms pulse to kick the DHT11 into life
    //////////////////////////////////////////////////
    gpio_set_level( gpio_pin, 1 );
    gpio_set_direction( gpio_pin, GPIO_MODE_OUTPUT );
    ets_delay_us( 1000 );
    gpio_set_level( gpio_pin, 0 );
    ets_delay_us( 20000 );

    ////////////////////////////////////////////////
    // Bring rmt_rx_start & rmt_rx_stop into cache
    ////////////////////////////////////////////////
    rmt_rx_start(rmt_channel, 1);
    rmt_rx_stop(rmt_channel);

    //////////////////////////////////////////////////
    // Now get the sensor to send the data
    //////////////////////////////////////////////////
    gpio_set_level( gpio_pin, 1 );
    gpio_set_direction( gpio_pin, GPIO_MODE_INPUT );

    ////////////////////////////////////////////////
    // Start the RMT receiver for the data this time
    ////////////////////////////////////////////////
    rmt_rx_start(rmt_channel, 1);

    /////////////////////////////////////////////////
    // Pull the data from the ring buffer
    /////////////////////////////////////////////////
    item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 2);
    if(item != NULL) {
        int n;
        n = rx_size / 4 - 0;
        //parse data value from ringbuffer.
        rtn = parse_items(item, n, humidity, temp_x10);
        //after parsing the data, return spaces to ringbuffer.
        vRingbufferReturnItem(rb, (void*) item);
    }
    /////////////////////////////////////////////////
    // Stop the RMT Receiver
    /////////////////////////////////////////////////
    rmt_rx_stop(rmt_channel);

    return rtn;
}


void blink_task(void *pvParameter)
{
    int temp, ftemp, humid;
    int currentTemp = 0;
    while( true )
    {
        temp = getTemp();
        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
        if( temp <= 21 ) {
            gpio_set_level(BLINK_GPIO_1, 0);
            gpio_set_level(BLINK_GPIO_2, 1);
            gpio_set_level(BLINK_GPIO_3, 1);
        } else if( temp <= 26 ) {
            gpio_set_level(BLINK_GPIO_1, 1);
            gpio_set_level(BLINK_GPIO_2, 0);
            gpio_set_level(BLINK_GPIO_3, 1);
        } else {
            gpio_set_level(BLINK_GPIO_1, 1);
            gpio_set_level(BLINK_GPIO_2, 1);
            gpio_set_level(BLINK_GPIO_3, 0);
        }
        ESP_LOGI(TAG, "Read %d temp", temp);
        //dht11_rmt_rx(GPIO_NUM_12, &humid, &temp);


//        for( int i = 0; i < 2; i++ ) {
//            gpio_set_level(BLINK_GPIO_1, i);
//
//            for( int j = 0; j < 2; j++ ) {
//                gpio_set_level(BLINK_GPIO_2, j);
//                for (int k = 0; k < 2; k++) {
//
//                    gpio_set_level(BLINK_GPIO_3, k);
//                    vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//                }
//            }
//        }
//        gpio_set_level(BLINK_GPIO_1, 0);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//        gpio_set_level(BLINK_GPIO_2, 0);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//        gpio_set_level(BLINK_GPIO_3, 0);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//        gpio_set_level(BLINK_GPIO_1, 1);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//        gpio_set_level(BLINK_GPIO_2, 1);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
//
//        gpio_set_level(BLINK_GPIO_3, 1);
//        vTaskDelay(BLINK_SLEEP_TIME / portTICK_PERIOD_MS);
    }
}





void app_main()
{
    init();
    xTaskCreate(&blink_task, "blink_task", 4096, NULL, configMAX_PRIORITIES, NULL);


}
