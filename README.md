# Sintetizador FM USB-MIDI con ESP32-S3

Sintetizador de software de modulación de fase (FM) de 2 operadores en tiempo real que se ejecuta en un microcontrolador ESP32-S3, controlado mediante USB-MIDI nativo desde Linux (PipeWire/ALSA y Reaper).

## 🛠️ Arquitectura de Hardware

Este proyecto se basa en el microcontrolador **ESP32-S3**, utilizando sus capacidades USB nativas para recibir datos MIDI en tiempo real sin requerir un chip externo USB a Serial.

* **Microcontrolador:** ESP32-S3-DevKitC-1 (aprovechando la interfaz USB nativa).
* **Amplificación de Audio:** Amplificador de audio clase D PAM8302A alimentado de forma segura a **3.3V** para proteger el puerto USB y evitar sobrecorrientes.
* **Reconstrucción de Señal:** Generación de audio PWM de 8 bits en el **GPIO 18**, combinada con un filtro paso bajo RC para eliminar el ruido de conmutación de alta frecuencia.
* **Superficie de Control:** Teclado USB MIDI externo o secuenciador DAW (probado en Ubuntu Linux usando enrutamiento PipeWire/ALSA).

## 💻 Arquitectura de Software y DSP

El firmware personalizado de Arduino implementa un enfoque de pila MIDI pura y ligera con un bucle DSP de software dedicado:

1. **Pila USB-MIDI Nativa:** Utiliza `USBMIDI.h` y `USB.h` para establecer una interfaz MIDI compatible con la clase estándar.
2. **Motor de Síntesis FM de 2 Operadores:**
   * Opera a una **frecuencia de muestreo estable de 10 kHz** utilizando acumulación de fase por software.
   * Calcula dinámicamente las frecuencias de la portadora a partir de los valores de notas MIDI entrantes ($f = 440 \cdot 2^{(d-69)/12}$).
   * Modula la fase de la portadora utilizando un oscilador modulador secundario controlado mediante parámetros MIDI CC (Rueda de Modulación y controles de timbre).
3. **Salida PWM por Hardware:** Controla el pin de audio utilizando el periférico PWM LEDC del ESP32 configurado a una frecuencia de portadora de 64 kHz.

## 🚀 Configuración e Instalación

### Prerrequisitos

* **Arduino IDE** con el paquete de placas ESP32 instalado.
* **Entorno Linux (Ubuntu/Debian)** ejecutando PipeWire o ALSA para el enrutamiento MIDI.
* **DAW o Enrutador MIDI:** Reaper (o cualquier patchbay compatible con ALSA como `qpwgraph` o `helvum`).

### Compilación y Carga del Firmware

1. Clona este repositorio:
   ```bash
   git clone https://github.com/agustintomasrojas/TID2026-2.git
   cd TID2026-2
   ```
2. Abre el sketch del proyecto (`esp32_fm_synth.ino`) en el Arduino IDE.
3. Selecciona tu placa ESP32-S3 y asegúrate de que el **Modo USB** esté configurado correctamente en las herramientas del IDE.
4. Compila y carga el sketch en la placa.

### Enrutamiento MIDI en Linux

1. Conecta tu ESP32-S3 a través de su puerto USB nativo.
2. Abre tu patchbay (`qpwgraph` o `helvum`).
3. Conecta el puerto de captura de tu controlador MIDI de hardware (`iRig Keys MIDI 1` o similar) al puerto **`ESP32S3_DEV MIDI 1 (playback)`**.

## 📊 Estado del Proyecto y Desarrollo

Desarrollado como parte del *Trabajo de Investigación y Desarrollo (TID)* en la Universidad Adolfo Ibáñez.

* **Fase Actual:** Comunicación USB-MIDI central, lógica de firmware y cálculos de frecuencia/fase DSP completamente implementados y verificados.
* **Próximos Pasos:** Finalizar la integración de la salida analógica física mediante el filtro RC y el circuito amplificador PAM8302A.

## 👤 Autor

* **Agustín Rojas**
* **Profesor Guía:** David Aguayo Vera
* **Institución:** Universidad Adolfo Ibáñez --- Facultad de Ingeniería y Ciencias