/* plugin-mainmenu/lxqtmainmenuconfiguration.h
 * Main menu plugin implementation
 */

#ifndef LXQTMAINMENUCONFIGURATION_H
#define LXQTMAINMENUCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QAbstractButton;

namespace Ui {
class LXQtMainMenuConfiguration;
}

class LXQtMainMenuConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtMainMenuConfiguration(PluginSettings* settings,
                                     const QString& defaultShortcut,
                                     QWidget* parent = nullptr);
  ~LXQtMainMenuConfiguration();

 private:
  Ui::LXQtMainMenuConfiguration* ui;
  QString mDefaultShortcut;
  bool mLockSettingChanges;

 private slots:
  void shortcutChanged(const QString& value);
  /*
    Saves settings in conf file.
  */
  void loadSettings();
  void textButtonChanged(const QString& value);
  void showTextChanged(bool value);
  void chooseIcon();
  void chooseMenuFile();
  void shortcutReset();
  void customFontChanged(bool value);
  void customFontSizeChanged(int value);
};

#endif  // LXQTMAINMENUCONFIGURATION_H