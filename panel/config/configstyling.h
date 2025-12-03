/* panel/config/configstyling.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGSTYLING_H
#define CONFIGSTYLING_H

#include "../oneg4panel.h"
#include <QSettings>
#include <QTimer>
#include <OneG4/ConfigDialog.h>

class OneG4Panel;

namespace Ui {
class ConfigStyling;
}

class ConfigStyling : public QWidget {
  Q_OBJECT

 public:
  explicit ConfigStyling(OneG4Panel* panel, QWidget* parent = nullptr);
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
  OneG4Panel* mPanel;

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