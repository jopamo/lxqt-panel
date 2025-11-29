/* plugin-worldclock/lxqtworldclockconfigurationtimezones.h
 * Configuration interface for plugin-worldclock
 */

#ifndef LXQT_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H
#define LXQT_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class LXQtWorldClockConfigurationTimeZones;
}

class QTreeWidgetItem;

class LXQtWorldClockConfigurationTimeZones : public QDialog {
  Q_OBJECT

 public:
  explicit LXQtWorldClockConfigurationTimeZones(QWidget* parent = nullptr);
  ~LXQtWorldClockConfigurationTimeZones();

  int updateAndExec();

  QString timeZone();

 public slots:
  void itemSelectionChanged();
  void itemDoubleClicked(QTreeWidgetItem*, int);

 private:
  Ui::LXQtWorldClockConfigurationTimeZones* ui;

  QString mTimeZone;

  QTreeWidgetItem* makeSureParentsExist(const QStringList& parts, QMap<QString, QTreeWidgetItem*>& parentItems);
};

#endif  // LXQT_PANEL_WORLDCLOCK_CONFIGURATION_TIMEZONES_H