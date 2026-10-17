#include <Arduino_FreeRTOS.h>

// تعریف پین‌های متصل به LEDها
const uint8_t LED1_PIN = 11;
const uint8_t LED2_PIN = 12;
const uint8_t LED3_PIN = 13;

// --- تعریف توابع تسک‌ها ---

// تسک اول با دوره تناوب 100 میلی‌ثانیه [cite: 153]
void vTask1(void *pvParameters) {
    pinMode(LED1_PIN, OUTPUT);
    bool state = true;
    
    while (1) {
        digitalWrite(LED1_PIN, state);
        state = !state;
        vTaskDelay(100 / portTICK_PERIOD_MS); // تاخیر 100 میلی‌ثانیه‌ای [cite: 61, 75]
    }
}

// تسک دوم با دوره تناوب 500 میلی‌ثانیه [cite: 155]
void vTask2(void *pvParameters) {
    pinMode(LED2_PIN, OUTPUT);
    bool state = true;
    
    while (1) {
        digitalWrite(LED2_PIN, state);
        state = !state;
        vTaskDelay(500 / portTICK_PERIOD_MS); // تاخیر 500 میلی‌ثانیه‌ای [cite: 61, 75]
    }
}

// تسک سوم با دوره تناوب 1000 میلی‌ثانیه [cite: 157]
void vTask3(void *pvParameters) {
    pinMode(LED3_PIN, OUTPUT);
    bool state = true;
    
    while (1) {
        digitalWrite(LED3_PIN, state);
        state = !state;
        vTaskDelay(1000 / portTICK_PERIOD_MS); // تاخیر 1000 میلی‌ثانیه‌ای [cite: 61, 75]
    }
}

void setup() {
    BaseType_t s; // اصلاح تایپ متغیر برای حل ارور کامپایل 

    // ساخت تسک اول با اولویت 1 [cite: 102, 119]
    s = xTaskCreate(vTask1, "Task 1", 128, NULL, 1, NULL);
    if (s != pdPASS) {
        Serial.println("Task 1 creation failed");
        while (1);
    }

    // ساخت تسک دوم با اولویت 1 [cite: 102, 119]
    s = xTaskCreate(vTask2, "Task 2", 128, NULL, 1, NULL);
    if (s != pdPASS) {
        Serial.println("Task 2 creation failed");
        while (1);
    }

    // ساخت تسک سوم با اولویت 1 [cite: 102, 119]
    s = xTaskCreate(vTask3, "Task 3", 128, NULL, 1, NULL);
    if (s != pdPASS) {
        Serial.println("Task 3 creation failed");
        while (1);
    }

    // راه‌اندازی زمان‌بند سیستم‌عامل [cite: 107]
    vTaskStartScheduler();

    Serial.println("Scheduler failed"); //[cite: 140]
    while (1);
}

void loop() {
    // خالی می‌ماند [cite: 11]
}