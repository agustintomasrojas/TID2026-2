#include "USB.h"
#include "USBMIDI.h"

USBMIDI MIDI;

// Parámetros de síntesis
const float sample_rate = 10000.0;
float freq_carrier = 440.0;
float mod_ratio = 0.5; 
float mod_index = 2.0;

float phase_c = 0;
float phase_m = 0;

const int audio_pin = 18;

void setup() {
  // Pure MIDI USB stack configuration (Sin depuración Serial)
  MIDI.begin();
  USB.begin();

  ledcAttach(audio_pin, 64000, 8);
}

void loop() {
  midiEventPacket_t packet;
  
  // 1. Escuchar a Reaper / Teclado MIDI de forma segura
  if (MIDI.readPacket(&packet)) {
    if (packet.header != 0) {
      byte command = packet.byte1 & 0xF0;
      byte data1 = packet.byte2;
      byte data2 = packet.byte3;

      if (command == 0x90 && data2 > 0) {
        freq_carrier = 440.0 * pow(2.0, (data1 - 69.0) / 12.0);
      }
      else if (command == 0xB0) {
        if (data1 == 1) mod_index = (data2 / 127.0) * 10.0;
        else if (data1 == 74) mod_ratio = (data2 / 127.0) * 4.0;
      }
    }
  }

  // 2. Motor DSP (Síntesis FM)
  float freq_modulator = freq_carrier * mod_ratio;
  float phase_inc_c = (TWO_PI * freq_carrier) / sample_rate;
  float phase_inc_m = (TWO_PI * freq_modulator) / sample_rate;

  phase_m += phase_inc_m;
  if (phase_m >= TWO_PI) phase_m -= TWO_PI;
  float mod_signal = mod_index * sin(phase_m);

  phase_c += phase_inc_c;
  if (phase_c >= TWO_PI) phase_c -= TWO_PI;
  float output = sin(phase_c + mod_signal);

  // 3. Salida de Audio por Hardware (Atenuación segura al 20%)
  int pwm_value = (int)(((output * 0.2) + 1.0) * 127.5);
  ledcWrite(audio_pin, pwm_value);
  
  delayMicroseconds(100); 
} 