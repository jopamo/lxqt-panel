/* plugin-volume/1g4-mixer/src/rolewidget.cc
 * Implementation of rolewidget.cc
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rolewidget.h"

#include <pulse/ext-stream-restore.h>

RoleWidget::RoleWidget(MainWindow* parent) : StreamWidget(parent) {
  lockToggleButton->hide();
  directionLabel->hide();
  deviceButton->hide();
  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void RoleWidget::onMuteToggleButton() {
  StreamWidget::onMuteToggleButton();

  executeVolumeUpdate();
}

void RoleWidget::executeVolumeUpdate() {
  pa_ext_stream_restore_info info;

  if (updating)
    return;

  info.name = role.constData();
  info.channel_map.channels = 1;
  info.channel_map.map[0] = PA_CHANNEL_POSITION_MONO;
  info.volume = volume;
  info.device = device == "" ? nullptr : device.constData();
  info.mute = muteToggleButton->isChecked();

  pa_operation* o;
  if (!(o = pa_ext_stream_restore_write(get_context(), PA_UPDATE_REPLACE, &info, 1, TRUE, nullptr, nullptr))) {
    show_error(tr("pa_ext_stream_restore_write() failed").toUtf8().constData());
    return;
  }

  pa_operation_unref(o);
}
