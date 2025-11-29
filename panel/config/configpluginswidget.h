/* panel/config/configpluginswidget.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPLUGINSWIDGET_H
#define CONFIGPLUGINSWIDGET_H

#include "../lxqtpanel.h"

#include <QWidget>

namespace Ui {
class ConfigPluginsWidget;
}
class AddPluginDialog;

class ConfigPluginsWidget : public QWidget {
  Q_OBJECT

 public:
  ConfigPluginsWidget(LXQtPanel* panel, QWidget* parent = nullptr);
  ~ConfigPluginsWidget();

 signals:
  void changed();

 public slots:
  void reset();

 private slots:
  void showAddPluginDialog();
  void resetButtons();

 private:
  Ui::ConfigPluginsWidget* ui;
  std::unique_ptr<AddPluginDialog> mAddPluginDialog;
  LXQtPanel* mPanel;
};

#endif