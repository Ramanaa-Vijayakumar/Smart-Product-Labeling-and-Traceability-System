# Smart Product Labeling and Traceability System

> A virtual AI-powered automation system for quality inspection, product labeling, and traceability in smart manufacturing.

## 🧠 Overview

This project simulates a smart industrial labeling station designed to automate the inspection, labeling, and traceability of products on a virtual conveyor belt. The system combines Python automation, AI models, sensor simulation, and data logging to mimic a real-world smart manufacturing environment.

Developed as part of the **Intel® Unnati Industrial Training Program – July 2025**.

## 👥 Team

- **Ramanaa Vijaykumar**
- **Vivan Jain**

Under the guidance of **Dr. Mahesh Anil Inamdar**, Assistant Professor, MIT

---

## 🛠️ Features

- Simulated **conveyor belt system** using Python.
- **QR code generation** with metadata: Device ID, Batch ID, Manufacturing Date, RoHS status.
- **AI-based OCR** validation using EasyOCR.
- **Defect detection model** using EfficientNetB0 (fresh/rotten classification).
- **SQLite-based traceability logging**.
- Label rejection for invalid or non-compliant products.

---

## 🧰 Tech Stack

| Component              | Tool/Library             |
|------------------------|--------------------------|
| Programming Language   | Python                   |
| Label Generation       | `qrcode`, `Pillow`       |
| OCR Validation         | EasyOCR                  |
| AI Classification      | TensorFlow + EfficientNetB0 |
| Database               | SQLite3                  |
| Simulation             | Tinkercad                |

---

## 🔗 Project Modules

### 📦 Module 1: Conveyor Belt & QR Label Generator
[Colab Notebook](https://colab.research.google.com/drive/1PTK9P4NTbBkZI6yEhSNHZDXRE0ndIRbk#scrollTo=vr3cbsTfdpLh)  
Generates and stores QR code labels; logs metadata to SQLite.

```python
import sqlite3
import qrcode

def generate_product():
    return {
        "device_id": f"DEV-{random.randint(1000, 9999)}",
        "batch_id": f"BATCH-{random.randint(1, 5)}",
        "manufacturing_date": f"2023-{random.randint(1, 12):02d}-{random.randint(1, 28):02d}",
        "rohs_compliant": random.choice([True, False]),
        "has_defect": random.random() < 0.1  # 10% defect rate
    }

def log_product(product, result):
    cursor.execute("""
    INSERT INTO products (device_id, batch_id, manufacturing_date, rohs_compliant, inspection_result)
    VALUES (?, ?, ?, ?, ?)
    """, (
        product["device_id"],
        product["batch_id"],
        product["manufacturing_date"],
        int(product["rohs_compliant"]),
        result["status"]
    ))
    conn.commit()
```

---

### 📦 Module 2: OCR Verification of Labels
[Colab Notebook](https://colab.research.google.com/drive/1QRugiNGdjPtFn-UuZO2QA-HjjDPghsXn?usp=sharing)
Verifies label content using OCR.

```python
import easyocr

reader = easyocr.Reader(['en'])
results = reader.readtext('DVC001.png')

for bbox, text, prob in results:
    print(f"Detected: {text} | Confidence: {prob:.2f}")
```

---

### 📦 Module 3: Defect Detection (Fresh/Rotten Classification)
[Colab Notebook](https://colab.research.google.com/drive/1Y5aS2oXVnpM2KqBC6HBPh64eOy2tW3D3?usp=sharing)
Classifies product state using a pre-trained EfficientNetB0 model.

```python
from tensorflow.keras.applications import EfficientNetB0
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Dense, GlobalAveragePooling2D

base_model = EfficientNetB0(include_top=False, input_shape=(224, 224, 3), weights='imagenet')
x = GlobalAveragePooling2D()(base_model.output)
x = Dense(128, activation='relu')(x)
output = Dense(2, activation='softmax')(x)
model = Model(inputs=base_model.input, outputs=output)
```

---

### ⚙️ Module 4: Tinkercad Simulation – Hardware Control System
[Tinkercad](https://www.tinkercad.com/things/1swFNVQqdLB-smartlabelingsystem)  
[Arduino Code](https://github.com/Ramanaa-Vijayakumar/Smart-Product-Labeling-and-Traceability-System/blob/main/ArduinoCode.ino)

This module simulates product detection, labeling, and rejection using an Arduino Uno, ultrasonic sensor, servo motor, LEDs, buzzer, and LCD display. It also handles real-time communication with the Python-based AI inspection modules over Serial.

The logic handles:
- Product presence detection via ultrasonic sensor
- Manual or automatic inspection triggers
- Labeling simulation and AI result handling
- Decision logic for rejection or acceptance
- Status display via LCD and indicators

---

## 📈 Results Summary

| Feature              | Outcome                    |
|----------------------|-----------------------------|
| QR Label Generation  | ✅ Successful (Image + Text) |
| OCR Accuracy         | ~96% using EasyOCR          |
| Defect Detection     | ~93% using EfficientNetB0   |
| Data Logging         | ✅ Logged via SQLite         |
| Label Rejection      | ✅ Handled invalid units      |

---

## 🚧 Challenges & Learnings

- **OCR Misreads**: Blurry or low-resolution label images led to occasional recognition errors.
- **Integration Complexity**: Synchronizing label generation, OCR validation, defect detection, and database logging in a cohesive virtual pipeline.
- **Hybrid Simulation**: Merging mechatronic simulation with AI-based modules required thoughtful architecture and modular code design.

**Key Learnings:**

- Full-stack prototyping of AI-integrated manufacturing systems  
- Python-based database operations and automation  
- Transfer learning for industrial defect classification

---

## 🎯 Conclusion

The project successfully simulates a Smart Product Labeling and Traceability System by combining automation, sensor simulation, OCR, and machine learning. It validates label integrity, detects faulty products, and logs metadata for traceability — all in a modular and scalable format. The system is adaptable for real-world deployment with embedded controllers, physical sensors, and industrial-grade cameras, serving as a robust prototype for intelligent manufacturing.

---
