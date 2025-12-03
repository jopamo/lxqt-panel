/* panel/oneg4panelpluginconfigdialog.cpp
 * Main panel implementation, window management
 */

#include "oneg4panelpluginconfigdialog.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDebug>
/************************************************

 ************************************************/
OneG4PanelPluginConfigDialog::OneG4PanelPluginConfigDialog(PluginSettings& settings, QWidget* parent)
    : QDialog(parent), mSettings(settings) {}

/************************************************

 ************************************************/
OneG4PanelPluginConfigDialog::~OneG4PanelPluginConfigDialog() = default;

/************************************************

 ************************************************/
PluginSettings& OneG4PanelPluginConfigDialog::settings() const {
  return mSettings;
}

/************************************************

 ************************************************/
void OneG4PanelPluginConfigDialog::closeEvent(QCloseEvent* event) {
  mSettings.storeToCache();
  return QDialog::closeEvent(event);
}

/************************************************

 ************************************************/
void OneG4PanelPluginConfigDialog::dialogButtonsAction(QAbstractButton* btn) {
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
void OneG4PanelPluginConfigDialog::setComboboxIndexByData(QComboBox* comboBox,
                                                          const QVariant& data,
                                                          int defaultIndex) const {
  int index = comboBox->findData(data);
  if (index < 0)
    index = defaultIndex;

  comboBox->setCurrentIndex(index);
}