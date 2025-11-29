/* plugin-worldclock/lxqtworldclock.h
 * Header file for lxqtworldclock
 */

#ifndef LXQT_PANEL_WORLDCLOCK_H
#define LXQT_PANEL_WORLDCLOCK_H

#include <QTimeZone>

#include <QDialog>
#include <QLabel>

#include <LXQt/RotatedWidget>

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtworldclockconfiguration.h"

class ActiveLabel;
class QTimer;
class LXQtWorldClockPopup;

class LXQtWorldClock : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  LXQtWorldClock(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~LXQtWorldClock();

  virtual QWidget* widget() { return mMainWidget; }
  virtual QString themeId() const { return QLatin1String("WorldClock"); }
  virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
  bool isSeparate() const { return true; }
  void activated(ActivationReason reason);

  virtual void settingsChanged();
  virtual void realign();
  QDialog* configureDialog();
  bool eventFilter(QObject* watched, QEvent* event);

 private slots:
  void timeout();
  void wheelScrolled(int);
  void updateTimeText();

 private:
  QWidget* mMainWidget;
  LXQt::RotatedWidget* mRotatedWidget;
  ActiveLabel* mContent;
  LXQtWorldClockPopup* mPopup;

  QTimer* mTimer;
  int mUpdateInterval;

  QStringList mTimeZones;
  QMap<QString, QString> mTimeZoneCustomNames;
  QString mDefaultTimeZone;
  QString mActiveTimeZone;
  QString mFormat;
  bool mTimeZoneWheel;

  bool mAutoRotate;
  bool mShowWeekNumber;
  bool mShowTooltip;
  QLabel* mPopupContent;

  QDateTime mShownTime;

  void restartTimer();

  void setTimeText();
  QString formatDateTime(const QDateTime& datetime, const QString& timeZoneName);
  void updatePopupContent();
  bool formatHasTimeZone(QString format);
  QString preformat(const QString& format, const QTimeZone& timeZone, const QDateTime& dateTime);
};

class ActiveLabel : public QLabel {
  Q_OBJECT

 public:
  explicit ActiveLabel(QWidget* = nullptr);

 signals:
  void wheelScrolled(int);
  void leftMouseButtonClicked();
  void middleMouseButtonClicked();

 protected:
  void wheelEvent(QWheelEvent*);
  void mouseReleaseEvent(QMouseEvent* event);
};

class LXQtWorldClockPopup : public QDialog {
  Q_OBJECT

 public:
  LXQtWorldClockPopup(QWidget* parent = nullptr);

  void show();
};

class LXQtWorldClockLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new LXQtWorldClock(startupInfo);
  }
};

#endif  // LXQT_PANEL_WORLDCLOCK_H