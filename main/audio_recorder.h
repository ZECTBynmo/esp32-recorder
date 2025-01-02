#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include "esp_err.h"

#define AUDIO_SAMPLE_RATE 8000
#define AUDIO_BUFFER_SIZE 1024
#define MIC_GPIO_NUM 34

esp_err_t init_audio_recorder(void);
void start_audio_recording(void);

#endif // AUDIO_RECORDER_H 