#include "USB.h"
#include "USBMIDI.h"
#include <driver/i2s.h>

USBMIDI MIDI;

const int sample_rate = 22050;    
float target_freq = 440.0;
float freq_carrier = 440.0;         
float mod_ratio = 1.0;            
float mod_index = 2.0;            

float phase_c = 0.0;              
float phase_m = 0.0;              

const i2s_port_t i2s_num = I2S_NUM_0;

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = 16,   
    .ws_io_num = 15,    
    .data_out_num = 17, 
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin(i2s_num, &pin_config);
  i2s_zero_dma_buffer(i2s_num);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  USB.begin();
  MIDI.begin();

  setupI2S();
}

void loop() {
  // 1. Procesamiento de MIDI no bloqueante
  midiEventPacket_t packet;
  while (MIDI.readPacket(&packet)) {
    if (packet.header != 0) {
      byte command = packet.byte1 & 0xF0;
      byte data1 = packet.byte2;
      byte data2 = packet.byte3;

      if (command == 0x90 && data2 > 0) {
        target_freq = 440.0 * pow(2.0, (data1 - 69.0) / 12.0);
        digitalWrite(LED_BUILTIN, HIGH);
      }
      else if (command == 0x80 || (command == 0x90 && data2 == 0)) {
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if (command == 0xB0) {
        if (data1 == 1) {
          mod_index = (data2 / 127.0) * 10.0;
        }
        else if (data1 == 74) {
          mod_ratio = (data2 / 127.0) * 4.0;
        }
      }
    }
  }

  // Suavizado de frecuencia
  freq_carrier += (target_freq - freq_carrier) * 0.1;

  // 2. Motor de Síntesis FM (2 Operadores)
  float freq_modulator = freq_carrier * mod_ratio;
  
  float phase_inc_c = (TWO_PI * freq_carrier) / sample_rate;
  float phase_inc_m = (TWO_PI * freq_modulator) / sample_rate;

  phase_m += phase_inc_m;
  if (phase_m >= TWO_PI) phase_m -= TWO_PI;
  float mod_signal = mod_index * sin(phase_m);

  phase_c += phase_inc_c + mod_signal;
  if (phase_c >= TWO_PI) phase_c -= TWO_PI;
  float output = sin(phase_c); 

  // 3. Envío seguro al I2S (sin bloqueos fatales)
  int16_t sample = (int16_t)(output * 16383.0); 
  int32_t stereo_sample = ((int32_t)sample << 16) | (sample & 0xFFFF);
  
  size_t bytes_written = 0;
  // Usamos un timeout de 0 o 2 ticks para evitar que el sistema colapse si el DAC se satura
  i2s_write(i2s_num, &stereo_sample, sizeof(stereo_sample), &bytes_written, 2);
}