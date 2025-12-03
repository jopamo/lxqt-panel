/* plugin-worldclock/oneg4worldclockconfigurationtimezones.h
 * Configuration interface for plugin-worldclock
 */

#ifndef ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H
#define ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class OneG4WorldClockConfigurationTimeZones;
}

class QTreeWidgetItem;

class OneG4WorldClockConfigurationTimeZones : public QDialog {
  Q_OBJECT

 public:
  explicit OneG4WorldClockConfigurationTimeZones(QWidget* parent = nullptr);
  ~OneG4WorldClockConfigurationTimeZones();

  int updateAndExec();

  QString timeZone();

 public slots:
  void itemSelectionChanged();
  void itemDoubleClicked(QTreeWidgetItem*, int);

 private:
  Ui::OneG4WorldClockConfigurationTimeZones* ui;

  QString mTimeZone;

  QTreeWidgetItem* makeSureParentsExist(const QStringList& parts, QMap<QString, QTreeWidgetItem*>& parentItems);
};

#endif  // ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H