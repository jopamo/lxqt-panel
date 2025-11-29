/* plugin-statusnotifier/statusnotifier.cpp
 * Implementation file for statusnotifier
 */

#include "statusnotifier.h"

StatusNotifier::StatusNotifier(const ILXQtPanelPluginStartupInfo& startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  m_widget = new StatusNotifierWidget(this);
}

QDialog* StatusNotifier::configureDialog() {
  auto dialog = new StatusNotifierConfiguration(settings());
  dialog->addItems(m_widget->itemTitles());
  return dialog;
}

void StatusNotifier::realign() {
  m_widget->realign();
}