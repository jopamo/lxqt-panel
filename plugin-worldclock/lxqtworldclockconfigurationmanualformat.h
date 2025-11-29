/* plugin-worldclock/lxqtworldclockconfigurationmanualformat.h
 * Configuration interface for plugin-worldclock
 */

#ifndef LXQT_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H
#define LXQT_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class LXQtWorldClockConfigurationManualFormat;
}

class QTreeWidgetItem;

class LXQtWorldClockConfigurationManualFormat : public QDialog {
  Q_OBJECT

 public:
  explicit LXQtWorldClockConfigurationManualFormat(QWidget* parent = nullptr);
  ~LXQtWorldClockConfigurationManualFormat();

  void setManualFormat(const QString&);

  QString manualFormat() const;

 signals:
  void manualFormatChanged();

 private:
  Ui::LXQtWorldClockConfigurationManualFormat* ui;
};

#endif  // LXQT_PANEL_WORLDCLOCK_CONFIGURATION_MANUAL_FORMAT_H