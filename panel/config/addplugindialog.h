/* panel/config/addplugindialog.h
 * Configuration dialogs and widgets
 */

#ifndef ONEG4_ADDPLUGINDIALOG_H
#define ONEG4_ADDPLUGINDIALOG_H

#include <OneG4/PluginInfo.h>
#include <QDialog>
#include <QTimer>

#define SEARCH_DELAY 125

namespace Ui {
class AddPluginDialog;
}

class AddPluginDialog : public QDialog {
  Q_OBJECT

 public:
  AddPluginDialog(QWidget* parent = nullptr);
  ~AddPluginDialog();

 signals:
  void pluginSelected(const OneG4::PluginInfo& plugin);

 private:
  Ui::AddPluginDialog* ui;
  OneG4::PluginInfoList mPlugins;
  QTimer mSearchTimer;

 private slots:
  void filter();
  void emitPluginSelected();
};

#endif  // ONEG4_ADDPLUGINDIALOG_H