#pragma once

#ifdef USE_ESP32

#include "../i2s_audio.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "esphome/components/microphone/microphone.h"
#include "esphome/core/component.h"
#include "esphome/core/ring_buffer.h"

namespace esphome {
namespace i2s_audio {

class I2SAudioMicrophone : public I2SAudioIn, public microphone::Microphone, public Component {
 public:
  void setup() override;
  void start() override;
  void stop() override;

  void loop() override;

  size_t read(int16_t *buf, size_t len) override;
  size_t read_secondary(int16_t *buf, size_t len) override;

  size_t available_secondary() override { return this->comm_ring_buffer_->available(); }

#if SOC_I2S_SUPPORTS_ADC
  void set_adc_channel(adc1_channel_t channel) {
    this->adc_channel_ = channel;
    this->adc_ = true;
  }
#endif

  void set_channel(i2s_channel_fmt_t channel) { this->channel_ = channel; }
  void set_sample_rate(uint32_t sample_rate) { this->sample_rate_ = sample_rate; }
  void set_bits_per_sample(i2s_bits_per_sample_t bits_per_sample) { this->bits_per_sample_ = bits_per_sample; }
  void set_use_apll(uint32_t use_apll) { this->use_apll_ = use_apll; }

  void set_din_pin(int8_t pin) { this->din_pin_ = pin; }
  void set_pdm(bool pdm) { this->pdm_ = pdm; }

 protected:
  esp_err_t start_i2s_driver_();

  static void read_task_(void *params);

  TaskHandle_t read_task_handle_{nullptr};
  QueueHandle_t event_queue_;
  std::unique_ptr<RingBuffer> asr_ring_buffer_;
  std::unique_ptr<RingBuffer> comm_ring_buffer_;

  bool use_apll_;
  bool pdm_{false};
  int8_t din_pin_{I2S_PIN_NO_CHANGE};

#if SOC_I2S_SUPPORTS_ADC
  bool adc_{false};
  adc1_channel_t adc_channel_{ADC1_CHANNEL_MAX};
#endif

  i2s_bits_per_sample_t bits_per_sample_;
  i2s_channel_fmt_t channel_;
  uint32_t sample_rate_;
};

}  // namespace i2s_audio
}  // namespace esphome

#endif  // USE_ESP32
