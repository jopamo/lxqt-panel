/* panel/config/configpluginswidget.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPLUGINSWIDGET_H
#define CONFIGPLUGINSWIDGET_H

#include "../oneg4panel.h"

#include <QWidget>

namespace Ui {
class ConfigPluginsWidget;
}
class AddPluginDialog;

class ConfigPluginsWidget : public QWidget {
  Q_OBJECT

 public:
  ConfigPluginsWidget(OneG4Panel* panel, QWidget* parent = nullptr);
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
  OneG4Panel* mPanel;
};

#endif