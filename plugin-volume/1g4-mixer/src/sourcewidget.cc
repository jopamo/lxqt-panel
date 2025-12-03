/* plugin-volume/1g4-mixer/src/sourcewidget.cc
 * Implementation of sourcewidget.cc
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sourcewidget.h"

SourceWidget::SourceWidget(MainWindow* parent) : DeviceWidget(parent, "source") {}

void SourceWidget::executeVolumeUpdate() {
  pa_operation* o;

  if (!(o = pa_context_set_source_volume_by_index(get_context(), index, &volume, nullptr, nullptr))) {
    show_error(tr("pa_context_set_source_volume_by_index() failed").toUtf8().constData());
    return;
  }

  pa_operation_unref(o);
}

void SourceWidget::onMuteToggleButton() {
  DeviceWidget::onMuteToggleButton();

  if (updating)
    return;

  pa_operation* o;
  if (!(o = pa_context_set_source_mute_by_index(get_context(), index, muteToggleButton->isChecked(), nullptr,
                                                nullptr))) {
    show_error(tr("pa_context_set_source_mute_by_index() failed").toUtf8().constData());
    return;
  }

  pa_operation_unref(o);
}

void SourceWidget::onDefaultToggleButton() {
  pa_operation* o;

  if (updating)
    return;

  if (!(o = pa_context_set_default_source(get_context(), name.constData(), nullptr, nullptr))) {
    show_error(tr("pa_context_set_default_source() failed").toUtf8().constData());
    return;
  }
  pa_operation_unref(o);
}

void SourceWidget::onPortChange() {
  if (updating)
    return;

  int current = portList->currentIndex();
  if (current != -1) {
    pa_operation* o;
    QByteArray port = portList->itemData(current).toByteArray();

    if (!(o = pa_context_set_source_port_by_index(get_context(), index, port.constData(), nullptr, nullptr))) {
      show_error(tr("pa_context_set_source_port_by_index() failed").toUtf8().constData());
      return;
    }

    pa_operation_unref(o);
  }
}
