/* panel/lxqtpanelpluginconfigdialog.cpp
 * Main panel implementation, window management
 */

#include "lxqtpanelpluginconfigdialog.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDebug>
/************************************************

 ************************************************/
LXQtPanelPluginConfigDialog::LXQtPanelPluginConfigDialog(PluginSettings& settings, QWidget* parent)
    : QDialog(parent), mSettings(settings) {}

/************************************************

 ************************************************/
LXQtPanelPluginConfigDialog::~LXQtPanelPluginConfigDialog() = default;

/************************************************

 ************************************************/
PluginSettings& LXQtPanelPluginConfigDialog::settings() const {
  return mSettings;
}

/************************************************

 ************************************************/
void LXQtPanelPluginConfigDialog::closeEvent(QCloseEvent* event) {
  mSettings.storeToCache();
  return QDialog::closeEvent(event);
}

/************************************************

 ************************************************/
void LXQtPanelPluginConfigDialog::dialogButtonsAction(QAbstractButton* btn) {
  QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(btn->parent());

  if (box && box->buttonRole(btn) == QDialogButtonBox::ResetRole) {
    mSettings.loadFromCache();
    loadSettings();
  }
  else {
    close();
  }
}

/************************************************

 ************************************************/
void LXQtPanelPluginConfigDialog::setComboboxIndexByData(QComboBox* comboBox,
                                                         const QVariant& data,
                                                         int defaultIndex) const {
  int index = comboBox->findData(data);
  if (index < 0)
    index = defaultIndex;

  comboBox->setCurrentIndex(index);
}