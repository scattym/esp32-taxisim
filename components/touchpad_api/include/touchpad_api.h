//
// Created by sysadmin on 22/06/18.
//

#ifndef ESP_IDF_TEMPLATE_TOUCHPAD_H
#define ESP_IDF_TEMPLATE_TOUCHPAD_H

#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT  (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

void touchpad_isr_init();
void setEmitEventFunctionPtr(void (*inputEmitEventFunctionPtr)(int));

#endif //ESP_IDF_TEMPLATE_TOUCHPAD_H

