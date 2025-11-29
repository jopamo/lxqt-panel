/* plugin-spacer/spacerconfiguration.h
 * Spacer plugin implementation
 */

#ifndef SPACERCONFIGURATION_H
#define SPACERCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QAbstractButton;

namespace Ui {
class SpacerConfiguration;
}

class SpacerConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit SpacerConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~SpacerConfiguration();

 public:
  static const QStringList msTypes;

 private:
  Ui::SpacerConfiguration* ui;

 private slots:
  /*
     Saves settings in conf file.
  */
  void loadSettings();
  void sizeChanged(int value);
  void typeChanged(int index);
  void widthTypeChanged(bool expandableChecked);
};

#endif  // SPACERCONFIGURATION_H