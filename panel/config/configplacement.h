/* panel/config/configplacement.h
 * Configuration dialogs and widgets
 */

#ifndef CONFIGPLACEMENT_H
#define CONFIGPLACEMENT_H

#include "../oneg4panel.h"
#include <QSettings>
#include <QTimer>
#include <OneG4/ConfigDialog.h>

class OneG4Panel;

namespace Ui {
class ConfigPlacement;
}

class ConfigPlacement : public QWidget {
  Q_OBJECT

 public:
  explicit ConfigPlacement(OneG4Panel* panel, QWidget* parent = nullptr);
  ~ConfigPlacement();

  int screenNum() const { return mScreenNum; }
  IOneG4Panel::Position position() const { return mPosition; }

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
  OneG4Panel* mPanel;
  int mScreenNum;
  IOneG4Panel::Position mPosition;

  void addPosition(const QString& name, int screen, OneG4Panel::Position position);
  void fillComboBox_position();
  void fillComboBox_alignment();
  int indexForPosition(int screen, IOneG4Panel::Position position);
  int getMaxLength();

  // old values for reset
  int mOldPanelSize;
  int mOldIconSize;
  int mOldLineCount;
  int mOldLength;
  bool mOldLengthInPercents;
  OneG4Panel::Alignment mOldAlignment;
  IOneG4Panel::Position mOldPosition;
  bool mOldHidable;
  bool mOldVisibleMargin;
  bool mOldHideOnOverlap;
  int mOldAnimation;
  int mOldShowDelay;
  int mOldScreenNum;
  bool mOldReserveSpace;
};

#endif