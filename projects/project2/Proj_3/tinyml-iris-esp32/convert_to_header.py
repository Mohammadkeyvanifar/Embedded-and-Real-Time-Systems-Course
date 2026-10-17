import os

tflite_path = "models/iris_model.tflite"
header_path = "include/iris_model.h"

# خواندن فایل باینری مدل
with open(tflite_path, "rb") as f:
    tflite_content = f.read()

# ساخت محتوای فایل هدر C++
hex_lines = []
for i in range(0, len(tflite_content), 12):
    chunk = tflite_content[i:i+12]
    hex_line = ", ".join([f"0x{b:02x}" for b in chunk])
    hex_lines.append("  " + hex_line)

hex_array = ",\n".join(hex_lines)

header_content = f"""#ifndef IRIS_MODEL_H
#define IRIS_MODEL_H

const unsigned char iris_model_tflite[] = {{
{hex_array}
}};

const unsigned int iris_model_tflite_len = {len(tflite_content)};

#endif
"""

# ذخیره در پوشه include
with open(header_path, "w") as f:
    f.write(header_content)

print(f"Header file successfully generated at {header_path}!")
print(f"Model length: {len(tflite_content)} bytes")