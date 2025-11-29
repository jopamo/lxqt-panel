/* plugin-dom/domplugin.cpp
 * Plugin implementation for plugin-dom
 */

#include "domplugin.h"
#include "treewindow.h"
#include <QDebug>
#include <XdgIcon>

DomPlugin::DomPlugin(const ILXQtPanelPluginStartupInfo& startupInfo) : QObject(), ILXQtPanelPlugin(startupInfo) {
  mButton.setAutoRaise(true);
  mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mButton.setIcon(XdgIcon::fromTheme(QStringLiteral("preferences-plugin")));
  mButton.setToolTip(tr("Panel DOM Tree"));
  connect(&mButton, &QToolButton::clicked, this, &DomPlugin::showDialog);
}

void DomPlugin::showDialog() {
  TreeWindow* dialog = mButton.findChild<TreeWindow*>();

  if (dialog == 0) {
    dialog = new TreeWindow(&mButton);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
  }

  willShowWindow(dialog);
  dialog->show();
  dialog->activateWindow();
}