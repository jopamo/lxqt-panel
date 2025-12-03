/* panel/config/configplacement.cpp
 * Configuration dialogs and widgets
 */

#include "configplacement.h"
#include "ui_configplacement.h"

#include "../oneg4panellimits.h"

#include <KWindowSystem>
#include <QDebug>
#include <QListView>
#include <QScreen>
#include <QWindow>
#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>

using namespace OneG4;

struct ScreenPosition {
  int screen;
  IOneG4Panel::Position position;
};
Q_DECLARE_METATYPE(ScreenPosition)

ConfigPlacement::ConfigPlacement(OneG4Panel* panel, QWidget* parent)
    : QWidget(parent), ui(new Ui::ConfigPlacement), mPanel(panel) {
  ui->setupUi(this);

  fillComboBox_position();
  fillComboBox_alignment();

  mOldPanelSize = mPanel->panelSize();
  mOldIconSize = mPanel->iconSize();
  mOldLineCount = mPanel->lineCount();

  mOldLength = mPanel->length();
  mOldLengthInPercents = mPanel->lengthInPercents();

  mOldAlignment = mPanel->alignment();

  mOldScreenNum = mPanel->screenNum();
  mScreenNum = mOldScreenNum;

  mOldPosition = mPanel->position();
  mPosition = mOldPosition;

  mOldHidable = mPanel->hidable();

  mOldVisibleMargin = mPanel->visibleMargin();

  mOldHideOnOverlap = mPanel->hideOnOverlap();

  mOldAnimation = mPanel->animationTime();
  mOldShowDelay = mPanel->showDelay();

  ui->spinBox_panelSize->setMinimum(PANEL_MINIMUM_SIZE);
  ui->spinBox_panelSize->setMaximum(PANEL_MAXIMUM_SIZE);

  mOldReserveSpace = mPanel->reserveSpace();

  // reset configurations from file
  reset();

  connect(ui->spinBox_panelSize, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);
  connect(ui->spinBox_iconSize, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);
  connect(ui->spinBox_lineCount, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);

  connect(ui->spinBox_length, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);
  connect(ui->comboBox_lengthType, &QComboBox::activated, this, &ConfigPlacement::widthTypeChanged);

  connect(ui->comboBox_alignment, &QComboBox::activated, this, &ConfigPlacement::editChanged);
  connect(ui->comboBox_position, &QComboBox::activated, this, &ConfigPlacement::positionChanged);
  connect(ui->groupBox_hidable, &QGroupBox::toggled, this, &ConfigPlacement::editChanged);
  connect(ui->checkBox_visibleMargin, &QCheckBox::toggled, this, &ConfigPlacement::editChanged);
  connect(ui->checkBox_overlap, &QAbstractButton::toggled, this, &ConfigPlacement::editChanged);
  connect(ui->spinBox_animation, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);
  connect(ui->spinBox_delay, &QSpinBox::valueChanged, this, &ConfigPlacement::editChanged);

  connect(ui->checkBox_reserveSpace, &QAbstractButton::toggled, this,
          [this](bool checked) { mPanel->setReserveSpace(checked, true); });
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::reset() {
  ui->spinBox_panelSize->setValue(mOldPanelSize);
  ui->spinBox_iconSize->setValue(mOldIconSize);
  ui->spinBox_lineCount->setValue(mOldLineCount);

  ui->comboBox_position->setCurrentIndex(indexForPosition(mOldScreenNum, mOldPosition));

  ui->groupBox_hidable->setChecked(mOldHidable);

  ui->checkBox_visibleMargin->setChecked(mOldVisibleMargin);

  ui->checkBox_overlap->setChecked(mOldHideOnOverlap);

  ui->spinBox_animation->setValue(mOldAnimation);
  ui->spinBox_delay->setValue(mOldShowDelay);

  fillComboBox_alignment();
  ui->comboBox_alignment->setCurrentIndex(mOldAlignment + 1);

  ui->comboBox_lengthType->setCurrentIndex(mOldLengthInPercents ? 0 : 1);
  widthTypeChanged();
  ui->spinBox_length->setValue(mOldLength);

  ui->checkBox_reserveSpace->setChecked(mOldReserveSpace);

  // update position
  positionChanged();
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::fillComboBox_position() {
  int screenCount = QApplication::screens().size();
  if (screenCount == 1) {
    addPosition(tr("Top of screen"), 0, OneG4Panel::PositionTop);
    addPosition(tr("Left of screen"), 0, OneG4Panel::PositionLeft);
    addPosition(tr("Right of screen"), 0, OneG4Panel::PositionRight);
    addPosition(tr("Bottom of screen"), 0, OneG4Panel::PositionBottom);
  }
  else {
    for (int screenNum = 0; screenNum < screenCount; screenNum++) {
      if (screenNum)
        ui->comboBox_position->insertSeparator(9999);

      addPosition(tr("Top of screen %1").arg(screenNum + 1), screenNum, OneG4Panel::PositionTop);
      addPosition(tr("Left of screen %1").arg(screenNum + 1), screenNum, OneG4Panel::PositionLeft);
      addPosition(tr("Right of screen %1").arg(screenNum + 1), screenNum, OneG4Panel::PositionRight);
      addPosition(tr("Bottom of screen %1").arg(screenNum + 1), screenNum, OneG4Panel::PositionBottom);
    }
  }
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::fillComboBox_alignment() {
  ui->comboBox_alignment->setItemData(0, QVariant(OneG4Panel::AlignmentLeft));
  ui->comboBox_alignment->setItemData(1, QVariant(OneG4Panel::AlignmentCenter));
  ui->comboBox_alignment->setItemData(2, QVariant(OneG4Panel::AlignmentRight));

  if (mPosition == IOneG4Panel::PositionTop || mPosition == IOneG4Panel::PositionBottom) {
    ui->comboBox_alignment->setItemText(0, tr("Left"));
    ui->comboBox_alignment->setItemText(1, tr("Center"));
    ui->comboBox_alignment->setItemText(2, tr("Right"));
  }
  else {
    ui->comboBox_alignment->setItemText(0, tr("Top"));
    ui->comboBox_alignment->setItemText(1, tr("Center"));
    ui->comboBox_alignment->setItemText(2, tr("Bottom"));
  };
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::addPosition(const QString& name, int screen, OneG4Panel::Position position) {
  if (OneG4Panel::canPlacedOn(screen, position))
    ui->comboBox_position->addItem(name, QVariant::fromValue(ScreenPosition{screen, position}));
}

/************************************************
 *
 ************************************************/
int ConfigPlacement::indexForPosition(int screen, IOneG4Panel::Position position) {
  for (int i = 0; i < ui->comboBox_position->count(); i++) {
    ScreenPosition sp = ui->comboBox_position->itemData(i).value<ScreenPosition>();
    if (screen == sp.screen && position == sp.position)
      return i;
  }
  return -1;
}

/************************************************
 *
 ************************************************/
ConfigPlacement::~ConfigPlacement() {
  delete ui;
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::editChanged() {
  mPanel->setPanelSize(ui->spinBox_panelSize->value(), true);
  mPanel->setIconSize(ui->spinBox_iconSize->value(), true);
  mPanel->setLineCount(ui->spinBox_lineCount->value(), true);

  mPanel->setLength(ui->spinBox_length->value(), ui->comboBox_lengthType->currentIndex() == 0, true);

  OneG4Panel::Alignment align =
      OneG4Panel::Alignment(ui->comboBox_alignment->itemData(ui->comboBox_alignment->currentIndex()).toInt());

  mPanel->setAlignment(align, true);
  mPanel->setPosition(mScreenNum, mPosition, true);
  mPanel->setHidable(ui->groupBox_hidable->isChecked(), true);
  mPanel->setVisibleMargin(ui->checkBox_visibleMargin->isChecked(), true);
  mPanel->setHideOnOverlap(ui->checkBox_overlap->isChecked(), true);
  mPanel->setAnimationTime(ui->spinBox_animation->value(), true);
  mPanel->setShowDelay(ui->spinBox_delay->value(), true);
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::widthTypeChanged() {
  int max = getMaxLength();

  if (ui->comboBox_lengthType->currentIndex() == 0) {
    // Percents .............................
    int v = ui->spinBox_length->value() * 100.0 / max;
    ui->spinBox_length->setRange(1, 100);
    ui->spinBox_length->setValue(v);
  }
  else {
    // Pixels ...............................
    int v = max / 100.0 * ui->spinBox_length->value();
    ui->spinBox_length->setRange(-max, max);
    ui->spinBox_length->setValue(v);
  }
}

/************************************************
 *
 ************************************************/
int ConfigPlacement::getMaxLength() {
  auto screens = QApplication::screens();
  if (screens.size() > mScreenNum) {
    if (mPosition == IOneG4Panel::PositionTop || mPosition == IOneG4Panel::PositionBottom)
      return screens.at(mScreenNum)->geometry().width();
    else
      return screens.at(mScreenNum)->geometry().height();
  }
  return 0;
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::positionChanged() {
  ScreenPosition sp = ui->comboBox_position->itemData(ui->comboBox_position->currentIndex()).value<ScreenPosition>();

  bool updateAlig = (sp.position == IOneG4Panel::PositionTop || sp.position == IOneG4Panel::PositionBottom) !=
                    (mPosition == IOneG4Panel::PositionTop || mPosition == IOneG4Panel::PositionBottom);

  int oldMax = getMaxLength();
  mPosition = sp.position;
  mScreenNum = sp.screen;
  int newMax = getMaxLength();

  if (ui->comboBox_lengthType->currentIndex() == 1 && oldMax != newMax) {
    // Pixels ...............................
    int v = ui->spinBox_length->value() * 1.0 * newMax / oldMax;
    ui->spinBox_length->setMaximum(newMax);
    ui->spinBox_length->setValue(v);
  }

  if (updateAlig)
    fillComboBox_alignment();

  editChanged();
}