/* panel/lxqtpanelpluginconfigdialog.h
 * Main panel implementation, window management
 */

#ifndef LXQTPANELPLUGINCONFIGDIALOG_H
#define LXQTPANELPLUGINCONFIGDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include "lxqtpanelglobals.h"
#include "pluginsettings.h"

class QComboBox;
class QCloseEvent;

class LXQT_PANEL_API LXQtPanelPluginConfigDialog : public QDialog {
  Q_OBJECT
 public:
  explicit LXQtPanelPluginConfigDialog(PluginSettings& settings, QWidget* parent = nullptr);
  explicit LXQtPanelPluginConfigDialog(PluginSettings* settings, QWidget* parent = nullptr)
      : LXQtPanelPluginConfigDialog(*settings, parent) {}
  virtual ~LXQtPanelPluginConfigDialog();

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

#endif  // LXQTPANELPLUGINCONFIGDIALOG_H