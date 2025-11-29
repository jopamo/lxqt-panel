/* panel/config/addplugindialog.h
 * Configuration dialogs and widgets
 */

#ifndef LXQT_ADDPLUGINDIALOG_H
#define LXQT_ADDPLUGINDIALOG_H

#include <LXQt/PluginInfo>
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
  void pluginSelected(const LXQt::PluginInfo& plugin);

 private:
  Ui::AddPluginDialog* ui;
  LXQt::PluginInfoList mPlugins;
  QTimer mSearchTimer;

 private slots:
  void filter();
  void emitPluginSelected();
};

#endif  // LXQT_ADDPLUGINDIALOG_H