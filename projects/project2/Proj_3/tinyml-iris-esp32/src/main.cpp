#include <Arduino.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "iris_model.h"

// پارامترهای نرمال‌سازی
const float IRIS_MEAN[] = {5.80916667f, 3.06166667f, 3.72666667f, 1.18333333f};
const float IRIS_SCALE[] = {0.82036535f, 0.44724776f, 1.74502786f, 0.74914766f};

// نام کلاس‌های گل زنبق
const char* LABELS[] = {"setosa", "versicolor", "virginica"};

// متغیرهای مورد نیاز TensorFlow Lite Micro
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}

void setup() {
  // === تغییر مهم: راه‌اندازی Serial1 روی پین‌های 16 (RX) و 17 (TX) ===
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  
  // کمی تاخیر برای بالا آمدن پورت سریال
  delay(1000);
  
  Serial1.println("\n--- Iris Flower Classification TinyML Project ---");

  // ۱. راه‌اندازی سیستم گزارش خطا
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // ۲. لود کردن آرایه مدل مپ شده از فایل هدر
  model = tflite::GetModel(iris_model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter, "Model schema version %d not equal to %d",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // ۳. لود کردن تمام عملگرهای محاسباتی ران‌تایم
  static tflite::AllOpsResolver resolver;

  // ۴. ساخت اینترپرتر برای اجرای مدل
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // ۵. تخصیص حافظه برای تانسورها
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // ۶. اشاره به تانسورهای ورودی و خروجی مدل
  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial1.println("Model loaded successfully!");
  Serial1.println("Please enter 4 features separated by commas (e.g., 5.1,3.5,1.4,0.2):");
}

void loop() {
  // بررسی اینکه ورودی جدیدی روی Serial1 دریافت شده است یا خیر
  if (Serial1.available() > 0) {
    float raw_features[4];
    
    // ایده استفاده از parseFloat برای جدا کردن اعداد ایده‌ی بسیار خوبی بود!
    for (int i = 0; i < 4; i++) {
      raw_features[i] = Serial1.parseFloat();
    }
    
    // خالی کردن بافر پورت سریال برای کاراکتر خط بعد (\n)
    while(Serial1.available() > 0) { Serial1.read(); }

    Serial1.print("\nReceived Features: ");
    for(int i=0; i<4; i++) {
      Serial1.print(raw_features[i]);
      Serial1.print(i < 3 ? ", " : "\n");
    }

    // فرآیند نرمال‌سازی داده‌ها
    for (int i = 0; i < 4; i++) {
      input->data.f[i] = (raw_features[i] - IRIS_MEAN[i]) / IRIS_SCALE[i];
    }

    // اجرای عمل استنتاج (Inference) روی برد
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
      return;
    }

    // یافتن کلاسی که بیشترین احتمال (Softmax) را آورده است
    int predicted_class = 0;
    float max_prob = output->data.f[0];
    for (int i = 1; i < 3; i++) {
      if (output->data.f[i] > max_prob) {
        max_prob = output->data.f[i];
        predicted_class = i;
      }
    }

    // چاپ نتیجه نهایی پروژه
    Serial1.print("Predicted class: ");
    Serial1.println(LABELS[predicted_class]);
    
    Serial1.println("\nEnter 4 features for another test:");
  }
}