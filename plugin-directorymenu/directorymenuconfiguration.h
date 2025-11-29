/* plugin-directorymenu/directorymenuconfiguration.h
 * Configuration interface for plugin-directorymenu
 */

#ifndef DIRECTORYMENUCONFIGURATION_H
#define DIRECTORYMENUCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QLocale>
#include <QDateTime>
#include <QDir>

namespace Ui {
class DirectoryMenuConfiguration;
}

class DirectoryMenuConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit DirectoryMenuConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~DirectoryMenuConfiguration();

 private:
  Ui::DirectoryMenuConfiguration* ui;
  QDir mBaseDirectory;
  QString mIcon;
  QIcon mDefaultIcon;
  QString mDefaultTerminal;

  /*
    Read settings from conf file and put data into controls.
  */
  void loadSettings();

 private slots:
  /*
    Saves settings in conf file.
  */
  void saveSettings();
  void showDirectoryDialog();
  void showIconDialog();
  void showLabelDialog();
  void showTermDialog();

 private:
};

#endif  // DIRECTORYMENUCONFIGURATION_H