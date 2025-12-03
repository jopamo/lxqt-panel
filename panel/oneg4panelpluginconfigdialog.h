/* panel/oneg4panelpluginconfigdialog.h
 * Main panel implementation, window management
 */

#ifndef ONEG4PANELPLUGINCONFIGDIALOG_H
#define ONEG4PANELPLUGINCONFIGDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include "oneg4panelglobals.h"
#include "pluginsettings.h"

class QComboBox;
class QCloseEvent;

class ONEG4_PANEL_API OneG4PanelPluginConfigDialog : public QDialog {
  Q_OBJECT
 public:
  explicit OneG4PanelPluginConfigDialog(PluginSettings& settings, QWidget* parent = nullptr);
  explicit OneG4PanelPluginConfigDialog(PluginSettings* settings, QWidget* parent = nullptr)
      : OneG4PanelPluginConfigDialog(*settings, parent) {}
  virtual ~OneG4PanelPluginConfigDialog();

  PluginSettings& settings() const;

 protected:
  virtual void closeEvent(QCloseEvent* event) override;

 protected slots:
  /*
    Saves settings in conf file.
  */
  virtual void loadSettings() = 0;
  virtual void dialogButtonsAction(QAbstractButton* btn);

 protected:
  void setComboboxIndexByData(QComboBox* comboBox, const QVariant& data, int defaultIndex = 0) const;

 private:
  PluginSettings& mSettings;
};

#endif  // ONEG4PANELPLUGINCONFIGDIALOG_H