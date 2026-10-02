# Instrumento Musical Digital con ESP32-S3 (TID 2026)

Repositorio oficial para el Trabajo de Investigación y Desarrollo (TID) titulado **"Instrumento musical digital con reproducción de SoundFonts (.sf2) en ESP32-S3"**.

## 📌 Descripción del Proyecto
Este proyecto busca diseñar e implementar un prototipo funcional de instrumento musical digital capaz de cargar bancos de sonidos en formato SoundFont (`.sf2`) en la memoria externa PSRAM de un microcontrolador ESP32-S3 y reproducirlos en tiempo real mediante un motor de audio embebido en C/C++.

## 🎯 Objetivos Específicos
1. **OE1 (Fundamentos):** Revisión bibliográfica del formato `.sf2` y fundamentos de audio digital (muestreo, *pitch-shifting* e interpolación).
2. **OE2 (Hardware):** Ensamblaje del circuito con ESP32-S3, PSRAM, DAC I2S (PCM5102A), amplificador PAM8302, lector microSD y controles físicos.
3. **OE3 (Motor Embebido):** Desarrollo del motor de audio en C/C++ para lectura desde SD, gestión en PSRAM, envolvente ADSR y polifonía básica.
4. **OE4 (Verificación):** Mediciones instrumentales de afinación (análisis FFT) y latencia de respuesta.
5. **OE5 (Demostración):** Validación del instrumento en un contexto real con distintos bancos de sonido.

## 🛠️ Tecnologías y Hardware
- **Microcontrolador:** ESP32-S3-DevKitC-1 (con PSRAM).
- **Audio:** DAC I2S PCM5102A y Amplificador PAM8302 con parlante.
- **Entorno de desarrollo:** Arduino IDE / ESP-IDF en Linux (Ubuntu/Fedora).
- **Control:** Conectividad USB-MIDI nativa.

## 📂 Estructura del Repositorio
- `esp32_fm_synth/`: Código fuente actual de pruebas para la síntesis FM por software y control USB-MIDI.
- `docs/`: Informes de avance y documentación técnica.

---
*Estudiante:* Agustín Rojas  
*Profesor guía:* David Aguayo Vera  
*Universidad Adolfo Ibáñez*
