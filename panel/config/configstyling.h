/* panel/config/configstyling.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGSTYLING_H
#define CONFIGSTYLING_H

#include "../lxqtpanel.h"
#include <QSettings>
#include <QTimer>
#include <LXQt/ConfigDialog>

class LXQtPanel;

namespace Ui {
class ConfigStyling;
}

class ConfigStyling : public QWidget {
  Q_OBJECT

 public:
  explicit ConfigStyling(LXQtPanel* panel, QWidget* parent = nullptr);
  ~ConfigStyling();

  void updateIconThemeSettings();

 signals:
  void changed();

 public slots:
  void reset();

 private slots:
  void editChanged();
  void pickFontColor();
  void pickBackgroundColor();
  void pickBackgroundImage();

 private:
  Ui::ConfigStyling* ui;
  LXQtPanel* mPanel;

  void fillComboBox_icon();

  // new values
  QColor mFontColor;
  QColor mBackgroundColor;

  // old values for reset
  QColor mOldFontColor;
  QColor mOldBackgroundColor;
  QString mOldBackgroundImage;
  int mOldOpacity;
};

#endif