import os
import numpy as np
import tensorflow as tf
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

# ۱. ساخت پوشه برای ذخیره مدل‌ها
os.makedirs("models", exist_ok=True)

# ۲. لود کردن دیتاست Iris
iris = load_iris()
X, y = iris.data, iris.target

# ۳. تقسیم داده‌ها به آموزشی و تست
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# ۴. استانداردسازی داده‌ها
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# ذخیره پارامترهای نرمال‌سازی (Mean و Scale) برای استفاده در ESP32
np.savez("models/iris_scaler_params.npz", mean=scaler.mean_, scale=scaler.scale_)
print("Mean:", scaler.mean_)
print("Scale:", scaler.scale_)

# ۵. ساخت معماری شبکه عصبی پیشنهادی پروژه
model = tf.keras.Sequential([
    tf.keras.layers.Dense(16, activation='relu', input_shape=(4,)),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(3, activation='softmax')
])

# ۶. کامپایل و آموزش مدل
model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['accuracy'])
model.fit(X_train_scaled, y_train, epochs=50, batch_size=8, validation_split=0.1)

# ۷. ذخیره مدل اصلی Keras
model.save("models/iris_model.keras")

# ۸. تبدیل مدل به فرمت TensorFlow Lite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# ۹. ذخیره فایل TFLite
with open("models/iris_model.tflite", "wb") as f:
    f.write(tflite_model)

print("Model successfully trained and converted to TFLite!")