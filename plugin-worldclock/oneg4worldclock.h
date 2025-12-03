/* plugin-worldclock/oneg4worldclock.h
 * Header file for oneg4worldclock
 */

#ifndef ONEG4_PANEL_WORLDCLOCK_H
#define ONEG4_PANEL_WORLDCLOCK_H

#include <QTimeZone>

#include <QDialog>
#include <QLabel>

#include <OneG4/RotatedWidget.h>

#include "../panel/ioneg4panelplugin.h"
#include "oneg4worldclockconfiguration.h"

class ActiveLabel;
class QTimer;
class OneG4WorldClockPopup;

class OneG4WorldClock : public QObject, public IOneG4PanelPlugin {
  Q_OBJECT
 public:
  OneG4WorldClock(const IOneG4PanelPluginStartupInfo& startupInfo);
  ~OneG4WorldClock();

  virtual QWidget* widget() { return mMainWidget; }
  virtual QString themeId() const { return QLatin1String("WorldClock"); }
  virtual IOneG4PanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
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
  OneG4::RotatedWidget* mRotatedWidget;
  ActiveLabel* mContent;
  OneG4WorldClockPopup* mPopup;

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

class OneG4WorldClockPopup : public QDialog {
  Q_OBJECT

 public:
  OneG4WorldClockPopup(QWidget* parent = nullptr);

  void show();
};

class OneG4WorldClockLibrary : public QObject, public IOneG4PanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "oneg4.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(IOneG4PanelPluginLibrary)
 public:
  IOneG4PanelPlugin* instance(const IOneG4PanelPluginStartupInfo& startupInfo) const {
    return new OneG4WorldClock(startupInfo);
  }
};

#endif  // ONEG4_PANEL_WORLDCLOCK_H