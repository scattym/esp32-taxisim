/*
This example code is in the Public Domain (or CC0 licensed, at your option.)

Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
        CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "touchpad_api.h"

#define TOUCHPAD_MAX_USED 1
#define TOUCHPAD_MAX 9
static const char* TAG = "Touch pad";
static bool s_pad_activated[TOUCHPAD_MAX];
static uint32_t s_pad_init_val[TOUCHPAD_MAX];
void (*emitEventFunctionPtr)(int) = NULL;


void setEmitEventFunctionPtr(void (*inputEmitEventFunctionPtr)(int)) {
    emitEventFunctionPtr = inputEmitEventFunctionPtr;
}

/*
  Read values sensed at all available touch pads.
  Use 2 / 3 of read value as the threshold
  to trigger interrupt when the pad is touched.
  Note: this routine demonstrates a simple way
  to configure activation threshold for the touch pads.
  Do not touch any pads when this routine
  is running (on application start).
 */
static void tp_example_set_thresholds(void)
{
    ESP_LOGI(TAG, "Touch pad maximum isl: %d", TOUCHPAD_MAX_USED);

    uint16_t touch_value;
    //delay some time in order to make the filter work and get a initial value
    vTaskDelay(500/portTICK_PERIOD_MS);

    int i = 0;
    //read filtered value
    touch_pad_read_filtered(i, &touch_value);
    s_pad_init_val[i] = touch_value;
    ESP_LOGI(TAG, "test init touch val: %d", touch_value);
    //set interrupt threshold.
    ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 4 / 5));

    i = 6;
    touch_pad_read_filtered(i, &touch_value);
    s_pad_init_val[i] = touch_value;
    ESP_LOGI(TAG, "test init touch val: %d", touch_value);
    //set interrupt threshold.
    ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 4 / 5));
}

/*
  Check if any of touch pads has been activated
  by reading a table updated by rtc_intr()
  If so, then print it out on a serial monitor.
  Clear related entry in the table afterwards

  In interrupt mode, the table is updated in touch ISR.

  In filter mode, we will compare the current filtered value with the initial one.
  If the current filtered value is less than 80% of the initial value, we can
  regard it as a 'touched' event.
  When calling touch_pad_init, a timer will be started to run the filter.
  This mode is designed for the situation that the pad is covered
  by a 2-or-3-mm-thick medium, usually glass or plastic.
  The difference caused by a 'touch' action could be very small, but we can still use
  filter mode to detect a 'touch' event.
 */
static void tp_example_read_task(void *pvParameter)
{
    int quit = 0;
    static int show_message;
    while ( !quit ) {
        //interrupt mode, enable touch interrupt
        touch_pad_intr_enable();
        for (int i = 0; i < TOUCHPAD_MAX; i++) {
            if (s_pad_activated[i] == true) {
                ESP_LOGI(TAG, "T%d activated!", i);
                // Wait a while for the pad being released
                vTaskDelay(200 / portTICK_PERIOD_MS);
                // Clear information on pad activation
                s_pad_activated[i] = false;
                // Reset the counter triggering a message
                // that application is running
                show_message = 1;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // If no pad is touched, every couple of seconds, show a message
        // that application is running
//        if (show_message++ % 500 == 0) {
//            ESP_LOGI(TAG, "Waiting for any pad being touched...");
//        }
    }
}

/*
  Handle an interrupt triggered when a pad is touched.
  Recognize what pad has been touched and save it in a table.
 */
static void tp_example_rtc_intr(void * arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    for (int i = 0; i < TOUCHPAD_MAX; i++) {
        if ((pad_intr >> i) & 0x01) {
            s_pad_activated[i] = true;
            if( emitEventFunctionPtr != NULL ) {
                (*emitEventFunctionPtr)(i);
            }
        }
    }
}

/*
 * Before reading touch pad, we need to initialize the RTC IO.
 */
static void tp_example_touch_pad_init()
{
    int i = 0;
    //init RTC IO and mode for touch pad.
    touch_pad_config(i, TOUCH_THRESH_NO_USE);
    i = 6;
    touch_pad_config(i, TOUCH_THRESH_NO_USE);
}

void touchpad_isr_init()
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    touch_pad_init();
    ESP_LOGI(TAG, "Initializing touch pad");

    // If use interrupt trigger mode, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    ESP_LOGI(TAG, "Initializing touch pad");
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    ESP_LOGI(TAG, "Initializing touch pad");
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    ESP_LOGI(TAG, "Initializing touch pad");
    // Init touch pad IO
    tp_example_touch_pad_init();
    ESP_LOGI(TAG, "Initializing touch pad");
    // Set thresh hold
    tp_example_set_thresholds();
    ESP_LOGI(TAG, "Initializing touch pad");
    // Register touch interrupt ISR
    touch_pad_isr_register(tp_example_rtc_intr, NULL);
    ESP_LOGI(TAG, "Initializing touch pad");
    // Start a task to show what pads have been touched
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}
