/* plugin-worldclock/worldclockmanualformatconfig.h
 * Configuration interface for plugin-worldclock
 */

#ifndef WORLDCLOCK_MANUAL_FORMAT_CONFIG_H
#define WORLDCLOCK_MANUAL_FORMAT_CONFIG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class WorldClockManualFormatConfig;
}

class QTreeWidgetItem;

class WorldClockManualFormatConfig : public QDialog {
  Q_OBJECT

 public:
  explicit WorldClockManualFormatConfig(QWidget* parent = nullptr);
  ~WorldClockManualFormatConfig();

  void setManualFormat(const QString&);

  QString manualFormat() const;

 signals:
  void manualFormatChanged();

 private:
  Ui::WorldClockManualFormatConfig* ui;
};

#endif  // WORLDCLOCK_MANUAL_FORMAT_CONFIG_H
