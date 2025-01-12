#include <asoundlib.h>

static const char *SOUND_DEVICE = "default";
static const int SOUND_FORMAT = SND_PCM_FORMAT_S16;

struct SoundHandleLinux {
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  unsigned int rate;
  unsigned int channels;
  unsigned int buffer_time;
  unsigned int period_time;
  snd_pcm_channel_area_t *areas;
  double frequency;
  snd_pcm_sframes_t buffer_size;
  snd_pcm_sframes_t period_size;
};

int init_sound_handle(struct SoundHandleLinux *soundhandle) {
  int err;
  snd_pcm_hw_params_alloca(&soundhandle->hwparams);
  snd_pcm_sw_params_alloca(&soundhandle->swparams);
  snd_pcm_hw_params_any(soundhandle->handle, soundhandle->hwparams);
  if ((err = snd_pcm_open(&soundhandle->handle, SOUND_DEVICE, SND_PCM_STREAM_PLAYBACK,
                          0)) < 0) {
    printf("playback open error: %s\n", snd_strerror(err));
    return err;
  }

  /* Setting hardware params */

  err = snd_pcm_hw_params_set_rate_resample(soundhandle->handle, soundhandle->hwparams,
                                            1);
  if (err < 0) {
    printf("setting hw failed: %s\n", snd_strerror(err));
    return err;
  }

  err = snd_pcm_hw_params_set_access(soundhandle->handle, soundhandle->hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    printf("setting hw failed: %s\n", snd_strerror(err));
    return err;
  }

  err = snd_pcm_hw_params_set_channels(soundhandle->handle, soundhandle->hwparams, soundhandle->channels);
  if (err < 0) {
    printf("setting hw failed: %s\n", snd_strerror(err));
    return err;
  }
  int dir;
  err = snd_pcm_hw_params_set_rate_near(soundhandle->handle,
                                        soundhandle->hwparams,
                                        &soundhandle->rate,
                                        &dir
                                        );
  if (err < 0) {
    printf("setting hw failed: %s\n", snd_strerror(err));
    return err;
  }


  /* setting software params */


  return 0;
}
