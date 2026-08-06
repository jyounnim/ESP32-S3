# Source: 03_SENSOR_ANOMALY_LAB.md
# Section: Step 2. Python으로 Autoencoder 학습 (PC에서, 오프디바이스)

import numpy as np
import tensorflow as tf
from tensorflow import keras

# normal_data.csv: 한 줄에 20개 값 (SAMPLE_WINDOW)
data = np.loadtxt("normal_data.csv", delimiter=",")
data = data / 4095.0  # 0~1 정규화 (ADC 12bit 기준)

# 간단한 Autoencoder: 20 -> 8 -> 20
inputs = keras.Input(shape=(20,))
encoded = keras.layers.Dense(8, activation="relu")(inputs)
decoded = keras.layers.Dense(20, activation="sigmoid")(encoded)
autoencoder = keras.Model(inputs, decoded)
autoencoder.compile(optimizer="adam", loss="mse")

autoencoder.fit(data, data, epochs=50, batch_size=16, validation_split=0.1)

# 정상 데이터의 복원 오차 분포를 보고 임계값(threshold)을 정함
reconstructed = autoencoder.predict(data)
errors = np.mean((data - reconstructed) ** 2, axis=1)
print("정상 데이터 복원 오차 - 평균:", errors.mean(), "최대:", errors.max())
# 임계값 예시: 평균 + 3 * 표준편차
threshold = errors.mean() + 3 * errors.std()
print("추천 임계값:", threshold)

# INT8 양자화하여 .tflite로 변환
def representative_dataset():
    for sample in data[:100]:
        yield [sample.reshape(1, 20).astype(np.float32)]

converter = tf.lite.TFLiteConverter.from_keras_model(autoencoder)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_dataset
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8
tflite_model = converter.convert()

with open("anomaly_model.tflite", "wb") as f:
    f.write(tflite_model)
