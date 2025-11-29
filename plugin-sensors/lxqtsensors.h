/* plugin-sensors/lxqtsensors.h
 * System sensors plugin implementation
 */

#ifndef LXQTSENSORS_H
#define LXQTSENSORS_H

#include "sensors.h"
#include "../panel/pluginsettings.h"
#include <QFrame>
#include <QProgressBar>
#include <QSet>
#include <QTimer>

class ProgressBar : public QProgressBar {
  Q_OBJECT
 public:
  ProgressBar(QWidget* parent = nullptr);

  QSize sizeHint() const;
  void setSensorColor(const QString& colorName);
};

class QSettings;
class ILXQtPanelPlugin;
class QBoxLayout;

class LXQtSensors : public QFrame {
  Q_OBJECT
 public:
  LXQtSensors(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~LXQtSensors();

  void settingsChanged();
  void realign();
 public slots:
  void updateSensorReadings();
  void warningAboutHighTemperature();

 private:
  ILXQtPanelPlugin* mPlugin;
  QBoxLayout* mLayout;
  QTimer mUpdateSensorReadingsTimer;
  QTimer mWarningAboutHighTemperatureTimer;
  Sensors mSensors;
  QList<Chip> mDetectedChips;
  QList<ProgressBar*> mTemperatureProgressBars;
  // With set we can handle updates in very easy way :)
  QSet<ProgressBar*> mHighTemperatureProgressBars;
  double celsiusToFahrenheit(double celsius);
  void initDefaultSettings();
  PluginSettings* mSettings;
};

#endif  // LXQTSENSORS_H