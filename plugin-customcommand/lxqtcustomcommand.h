/* plugin-customcommand/lxqtcustomcommand.h
 * Header file for lxqtcustomcommand
 */

#ifndef LXQTCUSTOMCOMMAND_H
#define LXQTCUSTOMCOMMAND_H

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtcustomcommandconfiguration.h"

#include <QToolButton>
#include <QTimer>
#include <QProcess>
#include <QPointer>
#include <LXQt/RotatedWidget>

class CustomButton;

class LXQtCustomCommand : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

  using OutputFormat_t = LXQtCustomCommandConfiguration::OutputFormat_t;

 public:
  LXQtCustomCommand(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtCustomCommand();

  virtual QWidget* widget();
  virtual QString themeId() const { return QStringLiteral("Custom"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  void realign();
  QDialog* configureDialog();

 protected slots:
  virtual void settingsChanged();

 private slots:
  void handleClick();
  void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void handleOutput();
  void handleWheelScrolled(int delta);
  void updateButton();
  void runCommand();
  void runDetached(QString command);

 private:
  CustomButton* mButton;
  QPointer<LXQtCustomCommandConfiguration> mConfigDialog;

  QProcess* mProcess;
  bool mTerminating;
  QTimer* mTimer;
  QTimer* mDelayedRunTimer;

  bool mFirstRun;
  QByteArray mOutputByteArray;

  bool mAutoRotate;
  QString mFont;
  QString mCommand;
  bool mRunWithBash;
  OutputFormat_t mOutputFormat;
  bool mContinuousOutput;
  bool mRepeat;
  int mRepeatTimer;
  QString mIcon;
  QString mText;
  QString mTooltip;
  int mMaxWidth;
  QString mClick;
  QString mWheelUp;
  QString mWheelDown;
};

class LXQtCustomCommandPluginLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtCustomCommand(startupInfo);
  }
};

#endif  // LXQTCUSTOMCOMMAND_H