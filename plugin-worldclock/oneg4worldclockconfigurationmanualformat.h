/* plugin-worldclock/oneg4worldclockconfigurationmanualformat.h
 * Configuration interface for plugin-worldclock
 */

#ifndef ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H
#define ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class OneG4WorldClockConfigurationManualFormat;
}

class QTreeWidgetItem;

class OneG4WorldClockConfigurationManualFormat : public QDialog {
  Q_OBJECT

 public:
  explicit OneG4WorldClockConfigurationManualFormat(QWidget* parent = nullptr);
  ~OneG4WorldClockConfigurationManualFormat();

  void setManualFormat(const QString&);

  QString manualFormat() const;

 signals:
  void manualFormatChanged();

 private:
  Ui::OneG4WorldClockConfigurationManualFormat* ui;
};

#endif  // ONEG4_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H