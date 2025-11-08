# 🐣 INCUBATOR

Sistema automatizado de **control de incubadora de huevos de aves** basado en microcontrolador (Arduino).  
El proyecto regula **temperatura**, **humedad**, **ventilación**, y el **volteo automático** de los huevos durante el proceso de incubación, garantizando condiciones óptimas para el desarrollo embrionario.

---

## 📋 Descripción General

El programa controla una incubadora mediante sensores de temperatura (**DS18B20** y **DHT22**), humedad y CO₂ (**MQ7**).  
A través de **relés**, gestiona los dispositivos de **calefacción**, **ventilación** y **volteo** de forma automática.  
También incluye una **pantalla informativa**, un **contador de días** y un **modo de depuración**.

El sistema adapta automáticamente los valores de referencia según el **día de incubación**, reduciendo temperatura y deteniendo el volteo a partir del día 18.

---

## ⚙️ Funcionalidades Principales

- 🔥 Control de **temperatura interna** (DS18B20).  
- 💧 Monitoreo de **humedad** y **temperatura ambiente** (DHT22).  
- 🌫️ Lectura de **CO₂** (MQ7).  
- ⚡ Control de relés:
  - `HEATER_PIN` → Resistencia calefactora  
  - `FAN_PIN` → Ventilador  
  - `FLIP_EGGS_PIN` → Motor de volteo  
- 🔄 **Volteo automático** de huevos varias veces al día.  
- ⏱️ **Contador de días de incubación** almacenado en EEPROM.  
- 🔔 **Alarma sonora (buzzer)** para avisos.  
- 🧠 **Modo depuración (debug)** para ver datos por Serial.  

---

## 🧠 Lógica de Control

| Día de incubación | Temperatura objetivo | Humedad | Acción especial |
|-------------------|----------------------|----------|-----------------|
| 1–17              | 37.7 °C              | 60 %     | Volteo activo   |
| 18–21             | 36.5 °C              | 73 %     | Volteo desactivado, ventilación frecuente |

**Reglas básicas:**
- Si la temperatura cae por debajo del **umbral inferior**, se activa la **calefacción**.  
- Si supera el **umbral superior**, se activa el **ventilador**.  
- A partir del día **21**, se muestra el mensaje: Los pollitos deberían haber nacido.

  
---

## 🧩 Configuración de Pines y Parámetros

```cpp
#define UMBRAL 37.70
#define UMBRAL_INF 37.30
#define UMBRAL_SUP 38.00
#define TEMP_LAST_6 36.50
#define TEMP_INF_LAST_6 36.00
#define TEMP_SUP_LAST_6 37.00
#define HUM_PREV_18 60
#define HUM_LAST_6 73
#define HEATER_PIN 9
#define FAN_PIN 10
#define FLIP_EGGS_PIN 11
```
## Los umbrales pueden ajustarse según el tipo de incubadora o especie a incubar.
---

## 🕐 Horarios de volteo de huevos

Los huevos se voltean automáticamente varias veces al día, a las siguientes horas:

```yaml
0, 3, 6, 9, 12, 14, 15, 18, 20, 21
```
Durante el minuto 11 de cada hora definida, se activa el relé de volteo.

---
## 💾 Persistencia de datos

El sistema guarda en EEPROM:

Día del último conteo (`pos 9`)

Días transcurridos desde el inicio de incubación (`pos 10`)

Esto permite continuar el ciclo correctamente incluso después de un reinicio

---
## 🧪 Modos de operación

```cpp
bool dev = true;   // Modo desarrollo (muestra datos por Serial)
bool prod = false; // Modo producción
bool reset = false; // Reinicia contadores al iniciar
bool flip = true;   // Habilita/deshabilita el volteo automático
```
---
## 🧰 Dependencias

Librerías necesarias:

- `RTClib` (para el reloj en tiempo real)

- `EEPROM.h`

- `OneWire.h` y `DallasTemperature.h` (para `DS18B20`)

- `DHT.h` (para sensor `DHT22`)

- `Wire.h`

- `Incubator.h` (archivo propio con funciones auxiliares y gestión de relés)

---
## 🧾 Créditos

- Proyecto: INCUBATOR
- Autor: Juan Manuel Fernández
- Versión: 1.0
- Licencia: MIT

## 📸 Ejemplo de pantalla o salida serial (modo debug)

```yaml
Temp interna: 37.55°C
Temp ambiente: 25.12°C
Humedad: 60%
CO2: 412 ppm
Día incubación: 12
Estado: Normal
```

----
## 🖥️ Diagrama ASCII de Conexión

`````````
                ┌───────────────────────────────┐
                │         ARDUINO UNO           │
                │                               │
        5V  ────┤•5V                         A0•│─── MQ7 (CO₂)
       GND  ────┤•GND                        A1•│─── (No usado)
    DS18B20 ────┤•D2                         A2•│─── (No usado)
      DHT22 ────┤•D3                         A3•│─── (No usado)
   LCD / I2C ───┤•A4(SDA)                    A4•│─── SDA → Pantalla LCD
   LCD / I2C ───┤•A5(SCL)                    A5•│─── SCL → Pantalla LCD
     BUZZER ────┤•D8                            │
     HEATER ────┤•D9                            │→ Relé Calefacción
       FAN ─────┤•D10                           │→ Relé Ventilador
 FLIP_EGGS_MOTOR┤•D11                           │→ Relé Volteo Huevos
    RTC (SDA) ──┤•A4                            │→ SDA Reloj RTC
    RTC (SCL) ──┤•A5                            │→ SCL Reloj RTC
                │                               │
                │ ┌─────────────┐               │
                │ │ EEPROM      │               │
                │ │ (interna)   │               │
                │ └─────────────┘               │
                └───────────────────────────────┘
`````````

