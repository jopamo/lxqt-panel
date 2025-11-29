/* panel/config/configplacement.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPLACEMENT_H
#define CONFIGPLACEMENT_H

#include "../lxqtpanel.h"
#include <QSettings>
#include <QTimer>
#include <LXQt/ConfigDialog>

class LXQtPanel;

namespace Ui {
class ConfigPlacement;
}

class ConfigPlacement : public QWidget {
  Q_OBJECT

 public:
  explicit ConfigPlacement(LXQtPanel* panel, QWidget* parent = nullptr);
  ~ConfigPlacement();

  int screenNum() const { return mScreenNum; }
  ILXQtPanel::Position position() const { return mPosition; }

 signals:
  void changed();

 public slots:
  void reset();

 private slots:
  void editChanged();
  void widthTypeChanged();
  void positionChanged();

 private:
  Ui::ConfigPlacement* ui;
  LXQtPanel* mPanel;
  int mScreenNum;
  ILXQtPanel::Position mPosition;

  void addPosition(const QString& name, int screen, LXQtPanel::Position position);
  void fillComboBox_position();
  void fillComboBox_alignment();
  int indexForPosition(int screen, ILXQtPanel::Position position);
  int getMaxLength();

  // old values for reset
  int mOldPanelSize;
  int mOldIconSize;
  int mOldLineCount;
  int mOldLength;
  bool mOldLengthInPercents;
  LXQtPanel::Alignment mOldAlignment;
  ILXQtPanel::Position mOldPosition;
  bool mOldHidable;
  bool mOldVisibleMargin;
  bool mOldHideOnOverlap;
  int mOldAnimation;
  int mOldShowDelay;
  int mOldScreenNum;
  bool mOldReserveSpace;
};

#endif