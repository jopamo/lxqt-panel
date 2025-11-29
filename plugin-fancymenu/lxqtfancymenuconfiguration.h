/* plugin-fancymenu/lxqtfancymenuconfiguration.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUCONFIGURATION_H
#define LXQTFANCYMENUCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QAbstractButton;

namespace Ui {
class LXQtFancyMenuConfiguration;
}

class LXQtFancyMenuConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtFancyMenuConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtFancyMenuConfiguration();

 private:
  void fillButtonPositionComboBox();
  void fillCategoryPositionComboBox();

 private:
  Ui::LXQtFancyMenuConfiguration* ui;
  QString mDefaultShortcut;
  bool mLockSettingChanges;

 private slots:
  /*
    Saves settings in conf file.
  */
  void loadSettings();
  void textButtonChanged(const QString& value);
  void showTextChanged(bool value);
  void chooseIcon();
  void chooseMenuFile();
  void customFontChanged(bool value);
  void customFontSizeChanged(int value);
  void buttonRowPositionChanged(int idx);
  void categoryPositionChanged(int idx);
};

#endif  // LXQTFANCYMENUCONFIGURATION_H