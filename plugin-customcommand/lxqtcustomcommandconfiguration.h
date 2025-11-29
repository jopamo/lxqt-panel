/* plugin-customcommand/lxqtcustomcommandconfiguration.h
 * Configuration interface for plugin-customcommand
 */

#ifndef LXQTCUSTOMCOMMANDCONFIGURATION_H
#define LXQTCUSTOMCOMMANDCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

namespace Ui {
class LXQtCustomCommandConfiguration;
}

class LXQtCustomCommandConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  enum OutputFormat_t { OUTPUT_BEGIN, OUTPUT_TEXT = OUTPUT_BEGIN, OUTPUT_ICON, OUTPUT_STRUCTURED, OUTPUT_END };

  static const QStringList msOutputFormatStrings;

 public:
  explicit LXQtCustomCommandConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtCustomCommandConfiguration();

 private slots:
  void autoRotateChanged(bool autoRotate);
  void fontButtonClicked();
  void textColorChanged();
  void textColorResetButtonClicked();
  void commandPlainTextEditChanged();
  void runWithBashCheckBoxChanged(bool runWithBash);
  void outputFormatComboBoxChanged(int index);
  void continuousOutputCheckBoxChanged(bool continuousOutput);
  void repeatCheckBoxChanged(bool repeat);
  void repeatTimerSpinBoxChanged();
  void iconLineEditChanged();
  void iconBrowseButtonClicked();
  void textLineEditChanged();
  void tooltipLineEditChanged();
  void maxWidthSpinBoxChanged();
  void clickLineEditChanged();
  void wheelUpLineEditChanged();
  void wheelDownLineEditChanged();

 protected slots:
  virtual void loadSettings();

 private:
  Ui::LXQtCustomCommandConfiguration* ui;
  bool mLockSettingChanges;
  QString mHelpText;
};

#endif  // LXQTCUSTOMCOMMANDCONFIGURATION_H