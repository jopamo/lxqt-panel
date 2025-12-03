/* plugin-volume/1g4-mixer/src/1g4-mixer.h
 * Implementation of 1g4-mixer.h
 */

#ifndef ONEG4_MIXER_H
#define ONEG4_MIXER_H

#include <signal.h>
#include <string.h>
#include <glib.h>

#include <pulse/pulseaudio.h>

/* Can be removed when PulseAudio 0.9.23 or newer is required */
#ifndef PA_VOLUME_UI_MAX
#define PA_VOLUME_UI_MAX (pa_sw_volume_from_dB(+11.0))
#endif

#define HAVE_SOURCE_OUTPUT_VOLUMES PA_CHECK_VERSION(0, 99, 0)
#define HAVE_EXT_DEVICE_RESTORE_API PA_CHECK_VERSION(0, 99, 0)

enum SinkInputType { SINK_INPUT_ALL, SINK_INPUT_CLIENT, SINK_INPUT_VIRTUAL };

enum SinkType {
  SINK_ALL,
  SINK_HARDWARE,
  SINK_VIRTUAL,
};

enum SourceOutputType { SOURCE_OUTPUT_ALL, SOURCE_OUTPUT_CLIENT, SOURCE_OUTPUT_VIRTUAL };

enum SourceType {
  SOURCE_ALL,
  SOURCE_NO_MONITOR,
  SOURCE_HARDWARE,
  SOURCE_VIRTUAL,
  SOURCE_MONITOR,
};

pa_context* get_context(void);
void show_error(const char* txt);

#ifdef ONEG4_MIXER_LIBRARY
#include <QDialog>
QDialog* create_1g4_mixer_dialog();
#endif

#endif
