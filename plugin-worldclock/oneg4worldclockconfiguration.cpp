/* plugin-worldclock/oneg4worldclockconfiguration.cpp
 * Configuration implementation for plugin-worldclock
 */

#include "oneg4worldclockconfiguration.h"

#include "ui_oneg4worldclockconfiguration.h"

#include "oneg4worldclockconfigurationtimezones.h"
#include "worldclockmanualformatconfig.h"

#include <QWidget>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFont>
#include <QMap>
#include <QVariant>
#include <QList>
#include <QString>
#include <QStringList>

OneG4WorldClockConfiguration::OneG4WorldClockConfiguration(PluginSettings* settings, QWidget* parent)
    : OneG4PanelPluginConfigDialog(settings, parent),
      ui(new Ui::OneG4WorldClockConfiguration),
      mLockCascadeSettingChanges(false),
      mConfigurationTimeZones(nullptr),
      mConfigurationManualFormat(nullptr) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QLatin1String("WorldClockConfigurationWindow"));
  ui->setupUi(this);

  connect(ui->buttons, &QDialogButtonBox::clicked, this, &OneG4WorldClockConfiguration::dialogButtonsAction);

  connect(ui->timeFormatCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timeShowSecondsCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timePadHourCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timeAMPMCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timezoneGB, &QGroupBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timezonePositionCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->timezoneFormatCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->dateGB, &QGroupBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->datePositionCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->dateFormatCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->dateShowYearCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->dateShowDoWCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->datePadDayCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->dateLongNamesCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->advancedManualGB, &QGroupBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->customisePB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::customiseManualFormatClicked);

  connect(ui->timeFormatCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::timeFormatChanged);
  connect(ui->dateGB, &QGroupBox::toggled, this, &OneG4WorldClockConfiguration::dateGroupToggled);
  connect(ui->dateFormatCB, &QComboBox::currentIndexChanged, this, &OneG4WorldClockConfiguration::dateFormatChanged);
  connect(ui->advancedManualGB, &QGroupBox::toggled, this, &OneG4WorldClockConfiguration::advancedFormatToggled);

  connect(ui->timeZonesTW, &QTableWidget::itemSelectionChanged, this,
          &OneG4WorldClockConfiguration::updateTimeZoneButtons);
  connect(ui->addPB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::addTimeZone);
  connect(ui->removePB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::removeTimeZone);
  connect(ui->setAsDefaultPB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::setTimeZoneAsDefault);
  connect(ui->editCustomNamePB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::editTimeZoneCustomName);
  connect(ui->moveUpPB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::moveTimeZoneUp);
  connect(ui->moveDownPB, &QPushButton::clicked, this, &OneG4WorldClockConfiguration::moveTimeZoneDown);

  connect(ui->autorotateCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->showWeekNumberCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);
  connect(ui->showTooltipCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);

  connect(ui->wheelCB, &QCheckBox::clicked, this, &OneG4WorldClockConfiguration::saveSettings);

  loadSettings();
}

OneG4WorldClockConfiguration::~OneG4WorldClockConfiguration() {
  delete ui;
}

void OneG4WorldClockConfiguration::loadSettings() {
  mLockCascadeSettingChanges = true;

  bool longTimeFormatSelected = false;

  QString formatType = settings().value(QLatin1String("formatType"), QString()).toString();
  QString dateFormatType = settings().value(QLatin1String("dateFormatType"), QString()).toString();
  bool advancedManual = settings().value(QLatin1String("useAdvancedManualFormat"), false).toBool();
  mManualFormat = settings()
                      .value(QLatin1String("customFormat"),
                             tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'"))
                      .toString();

  // backward compatibility
  if (formatType == QLatin1String("custom")) {
    formatType = QLatin1String("short-timeonly");
    dateFormatType = QLatin1String("short");
    advancedManual = true;
  }
  else if (formatType == QLatin1String("short")) {
    formatType = QLatin1String("short-timeonly");
    dateFormatType = QLatin1String("short");
    advancedManual = false;
  }
  else if ((formatType == QLatin1String("full")) || (formatType == QLatin1String("long")) ||
           (formatType == QLatin1String("medium"))) {
    formatType = QLatin1String("long-timeonly");
    dateFormatType = QLatin1String("long");
    advancedManual = false;
  }

  if (formatType == QLatin1String("short-timeonly"))
    ui->timeFormatCB->setCurrentIndex(0);
  else if (formatType == QLatin1String("long-timeonly")) {
    ui->timeFormatCB->setCurrentIndex(1);
    longTimeFormatSelected = true;
  }
  else
    ui->timeFormatCB->setCurrentIndex(2);

  ui->timeShowSecondsCB->setChecked(settings().value(QLatin1String("timeShowSeconds"), false).toBool());
  ui->timePadHourCB->setChecked(settings().value(QLatin1String("timePadHour"), false).toBool());
  ui->timeAMPMCB->setChecked(settings().value(QLatin1String("timeAMPM"), false).toBool());
  ui->showTooltipCB->setChecked(settings().value(QLatin1String("showTooltip"), false).toBool());

  const bool customTimeFormatSelected = ui->timeFormatCB->currentIndex() == ui->timeFormatCB->count() - 1;
  ui->timeCustomW->setEnabled(customTimeFormatSelected);

  ui->timezoneGB->setEnabled(!longTimeFormatSelected);

  // timezone
  ui->timezoneGB->setChecked(settings().value(QLatin1String("showTimezone"), false).toBool() &&
                             !longTimeFormatSelected);

  const QString timezonePosition = settings().value(QLatin1String("timezonePosition"), QString()).toString();
  if (timezonePosition == QLatin1String("above"))
    ui->timezonePositionCB->setCurrentIndex(1);
  else if (timezonePosition == QLatin1String("before"))
    ui->timezonePositionCB->setCurrentIndex(2);
  else if (timezonePosition == QLatin1String("after"))
    ui->timezonePositionCB->setCurrentIndex(3);
  else
    ui->timezonePositionCB->setCurrentIndex(0);

  const QString timezoneFormatType = settings().value(QLatin1String("timezoneFormatType"), QString()).toString();
  if (timezoneFormatType == QLatin1String("short"))
    ui->timezoneFormatCB->setCurrentIndex(0);
  else if (timezoneFormatType == QLatin1String("long"))
    ui->timezoneFormatCB->setCurrentIndex(1);
  else if (timezoneFormatType == QLatin1String("offset"))
    ui->timezoneFormatCB->setCurrentIndex(2);
  else if (timezoneFormatType == QLatin1String("abbreviation"))
    ui->timezoneFormatCB->setCurrentIndex(3);
  else
    ui->timezoneFormatCB->setCurrentIndex(4);

  // date
  const bool dateIsChecked = settings().value(QLatin1String("showDate"), false).toBool();
  ui->dateGB->setChecked(dateIsChecked);

  const QString datePosition = settings().value(QLatin1String("datePosition"), QString()).toString();
  if (datePosition == QLatin1String("above"))
    ui->datePositionCB->setCurrentIndex(1);
  else if (datePosition == QLatin1String("before"))
    ui->datePositionCB->setCurrentIndex(2);
  else if (datePosition == QLatin1String("after"))
    ui->datePositionCB->setCurrentIndex(3);
  else
    ui->datePositionCB->setCurrentIndex(0);

  if (dateFormatType == QLatin1String("short"))
    ui->dateFormatCB->setCurrentIndex(0);
  else if (dateFormatType == QLatin1String("long"))
    ui->dateFormatCB->setCurrentIndex(1);
  else if (dateFormatType == QLatin1String("iso"))
    ui->dateFormatCB->setCurrentIndex(2);
  else
    ui->dateFormatCB->setCurrentIndex(3);

  ui->dateShowYearCB->setChecked(settings().value(QLatin1String("dateShowYear"), false).toBool());
  ui->dateShowDoWCB->setChecked(settings().value(QLatin1String("dateShowDoW"), false).toBool());
  ui->datePadDayCB->setChecked(settings().value(QLatin1String("datePadDay"), false).toBool());
  ui->dateLongNamesCB->setChecked(settings().value(QLatin1String("dateLongNames"), false).toBool());

  const bool customDateFormatSelected = ui->dateFormatCB->currentIndex() == ui->dateFormatCB->count() - 1;
  ui->dateCustomW->setEnabled(dateIsChecked && customDateFormatSelected);

  ui->advancedManualGB->setChecked(advancedManual);

  mDefaultTimeZone = settings().value(QStringLiteral("defaultTimeZone"), QString()).toString();

  ui->timeZonesTW->setRowCount(0);

  const QList<QMap<QString, QVariant>> list = settings().readArray(QLatin1String("timeZones"));
  int i = 0;
  for (const auto& map : list) {
    ui->timeZonesTW->setRowCount(ui->timeZonesTW->rowCount() + 1);

    const QString timeZoneName = map.value(QLatin1String("timeZone"), QString()).toString();
    if (mDefaultTimeZone.isEmpty())
      mDefaultTimeZone = timeZoneName;

    ui->timeZonesTW->setItem(i, 0, new QTableWidgetItem(timeZoneName));
    ui->timeZonesTW->setItem(i, 1, new QTableWidgetItem(map.value(QLatin1String("customName"), QString()).toString()));

    setBold(i, mDefaultTimeZone == timeZoneName);
    ++i;
  }

  ui->timeZonesTW->resizeColumnsToContents();

  ui->autorotateCB->setChecked(settings().value(QStringLiteral("autoRotate"), true).toBool());
  ui->showWeekNumberCB->setChecked(settings().value(QLatin1String("showWeekNumber"), true).toBool());

  ui->wheelCB->setChecked(settings().value(QLatin1String("timeZoneWheel"), true).toBool());

  mLockCascadeSettingChanges = false;
}

void OneG4WorldClockConfiguration::saveSettings() {
  if (mLockCascadeSettingChanges)
    return;

  QString formatType;
  switch (ui->timeFormatCB->currentIndex()) {
    case 0:
      formatType = QLatin1String("short-timeonly");
      break;

    case 1:
      formatType = QLatin1String("long-timeonly");
      break;

    case 2:
      formatType = QLatin1String("custom-timeonly");
      break;
  }
  settings().setValue(QLatin1String("formatType"), formatType);

  settings().setValue(QLatin1String("timeShowSeconds"), ui->timeShowSecondsCB->isChecked());
  settings().setValue(QLatin1String("timePadHour"), ui->timePadHourCB->isChecked());
  settings().setValue(QLatin1String("timeAMPM"), ui->timeAMPMCB->isChecked());

  settings().setValue(QLatin1String("showTimezone"), ui->timezoneGB->isChecked());

  QString timezonePosition;
  switch (ui->timezonePositionCB->currentIndex()) {
    case 0:
      timezonePosition = QLatin1String("below");
      break;

    case 1:
      timezonePosition = QLatin1String("above");
      break;

    case 2:
      timezonePosition = QLatin1String("before");
      break;

    case 3:
      timezonePosition = QLatin1String("after");
      break;
  }
  settings().setValue(QLatin1String("timezonePosition"), timezonePosition);

  QString timezoneFormatType;
  switch (ui->timezoneFormatCB->currentIndex()) {
    case 0:
      timezoneFormatType = QLatin1String("short");
      break;

    case 1:
      timezoneFormatType = QLatin1String("long");
      break;

    case 2:
      timezoneFormatType = QLatin1String("offset");
      break;

    case 3:
      timezoneFormatType = QLatin1String("abbreviation");
      break;

    case 4:
      timezoneFormatType = QLatin1String("iana");
      break;
  }
  settings().setValue(QLatin1String("timezoneFormatType"), timezoneFormatType);

  settings().setValue(QLatin1String("showDate"), ui->dateGB->isChecked());

  QString datePosition;
  switch (ui->datePositionCB->currentIndex()) {
    case 0:
      datePosition = QLatin1String("below");
      break;

    case 1:
      datePosition = QLatin1String("above");
      break;

    case 2:
      datePosition = QLatin1String("before");
      break;

    case 3:
      datePosition = QLatin1String("after");
      break;
  }
  settings().setValue(QLatin1String("datePosition"), datePosition);

  QString dateFormatType;
  switch (ui->dateFormatCB->currentIndex()) {
    case 0:
      dateFormatType = QLatin1String("short");
      break;

    case 1:
      dateFormatType = QLatin1String("long");
      break;

    case 2:
      dateFormatType = QLatin1String("iso");
      break;

    case 3:
      dateFormatType = QLatin1String("custom");
      break;
  }
  settings().setValue(QLatin1String("dateFormatType"), dateFormatType);

  settings().setValue(QLatin1String("dateShowYear"), ui->dateShowYearCB->isChecked());
  settings().setValue(QLatin1String("dateShowDoW"), ui->dateShowDoWCB->isChecked());
  settings().setValue(QLatin1String("datePadDay"), ui->datePadDayCB->isChecked());
  settings().setValue(QLatin1String("dateLongNames"), ui->dateLongNamesCB->isChecked());

  settings().setValue(QLatin1String("customFormat"), mManualFormat);

  settings().remove(QLatin1String("timeZones"));
  QList<QMap<QString, QVariant>> array;
  const int size = ui->timeZonesTW->rowCount();
  array.reserve(size);
  for (int i = 0; i < size; ++i) {
    QMap<QString, QVariant> map;
    map.insert(QLatin1String("timeZone"), ui->timeZonesTW->item(i, 0)->text());
    map.insert(QLatin1String("customName"), ui->timeZonesTW->item(i, 1)->text());
    array.push_back(map);
  }
  settings().setArray(QLatin1String("timeZones"), array);

  settings().setValue(QLatin1String("defaultTimeZone"), mDefaultTimeZone);
  settings().setValue(QLatin1String("useAdvancedManualFormat"), ui->advancedManualGB->isChecked());
  settings().setValue(QLatin1String("autoRotate"), ui->autorotateCB->isChecked());
  settings().setValue(QLatin1String("showWeekNumber"), ui->showWeekNumberCB->isChecked());
  settings().setValue(QLatin1String("showTooltip"), ui->showTooltipCB->isChecked());

  settings().setValue(QLatin1String("timeZoneWheel"), ui->wheelCB->isChecked());
}

void OneG4WorldClockConfiguration::timeFormatChanged(int index) {
  const bool longTimeFormatSelected = index == 1;
  const bool customTimeFormatSelected = index == 2;
  ui->timeCustomW->setEnabled(customTimeFormatSelected);
  ui->timezoneGB->setEnabled(!longTimeFormatSelected);
}

void OneG4WorldClockConfiguration::dateGroupToggled(bool dateIsChecked) {
  const bool customDateFormatSelected = ui->dateFormatCB->currentIndex() == ui->dateFormatCB->count() - 1;
  ui->dateCustomW->setEnabled(dateIsChecked && customDateFormatSelected);
}

void OneG4WorldClockConfiguration::dateFormatChanged(int index) {
  const bool customDateFormatSelected = index == ui->dateFormatCB->count() - 1;
  const bool dateIsChecked = ui->dateGB->isChecked();
  ui->dateCustomW->setEnabled(dateIsChecked && customDateFormatSelected);
}

void OneG4WorldClockConfiguration::advancedFormatToggled(bool on) {
  const bool longTimeFormatSelected = ui->timeFormatCB->currentIndex() == 1;
  ui->timeGB->setEnabled(!on);
  ui->timezoneGB->setEnabled(!on && !longTimeFormatSelected);
  ui->dateGB->setEnabled(!on);
}

void OneG4WorldClockConfiguration::customiseManualFormatClicked() {
  if (!mConfigurationManualFormat) {
    mConfigurationManualFormat = new WorldClockManualFormatConfig(this);
    connect(mConfigurationManualFormat, &WorldClockManualFormatConfig::manualFormatChanged, this,
            &OneG4WorldClockConfiguration::manualFormatChanged);
  }

  mConfigurationManualFormat->setManualFormat(mManualFormat);

  const QString oldManualFormat = mManualFormat;

  mManualFormat = (mConfigurationManualFormat->exec() == QDialog::Accepted) ? mConfigurationManualFormat->manualFormat()
                                                                            : oldManualFormat;

  saveSettings();
}

void OneG4WorldClockConfiguration::manualFormatChanged() {
  mManualFormat = mConfigurationManualFormat->manualFormat();
  saveSettings();
}

void OneG4WorldClockConfiguration::updateTimeZoneButtons() {
  const QList<QTableWidgetItem*> selectedItems = ui->timeZonesTW->selectedItems();
  const int selectedCount = selectedItems.count() / 2;
  const int allCount = ui->timeZonesTW->rowCount();

  ui->removePB->setEnabled(selectedCount != 0);
  bool canSetAsDefault = (selectedCount == 1);
  if (canSetAsDefault) {
    if (selectedItems[0]->column() == 0)
      canSetAsDefault = (selectedItems[0]->text() != mDefaultTimeZone);
    else
      canSetAsDefault = (selectedItems[1]->text() != mDefaultTimeZone);
  }

  bool canMoveUp = false;
  bool canMoveDown = false;
  if ((selectedCount != 0) && (selectedCount != allCount)) {
    bool skipBottom = true;
    for (int i = allCount - 1; i >= 0; --i) {
      if (ui->timeZonesTW->item(i, 0)->isSelected()) {
        if (!skipBottom) {
          canMoveDown = true;
          break;
        }
      }
      else {
        skipBottom = false;
      }
    }

    bool skipTop = true;
    for (int i = 0; i < allCount; ++i) {
      if (ui->timeZonesTW->item(i, 0)->isSelected()) {
        if (!skipTop) {
          canMoveUp = true;
          break;
        }
      }
      else {
        skipTop = false;
      }
    }
  }
  ui->setAsDefaultPB->setEnabled(canSetAsDefault);
  ui->editCustomNamePB->setEnabled(selectedCount == 1);
  ui->moveUpPB->setEnabled(canMoveUp);
  ui->moveDownPB->setEnabled(canMoveDown);
}

int OneG4WorldClockConfiguration::findTimeZone(const QString& timeZone) {
  const QList<QTableWidgetItem*> items = ui->timeZonesTW->findItems(timeZone, Qt::MatchExactly);
  for (const QTableWidgetItem* item : items)
    if (item->column() == 0)
      return item->row();
  return -1;
}

void OneG4WorldClockConfiguration::addTimeZone() {
  if (!mConfigurationTimeZones)
    mConfigurationTimeZones = new OneG4WorldClockConfigurationTimeZones(this);

  if (mConfigurationTimeZones->updateAndExec() == QDialog::Accepted) {
    const QString timeZone = mConfigurationTimeZones->timeZone();
    if (!timeZone.isEmpty()) {
      if (findTimeZone(timeZone) == -1) {
        const int row = ui->timeZonesTW->rowCount();
        ui->timeZonesTW->setRowCount(row + 1);
        auto* item = new QTableWidgetItem(timeZone);
        ui->timeZonesTW->setItem(row, 0, item);
        ui->timeZonesTW->setItem(row, 1, new QTableWidgetItem(QString()));
        if (mDefaultTimeZone.isEmpty())
          setDefault(row);
      }
    }
  }

  saveSettings();
}

void OneG4WorldClockConfiguration::removeTimeZone() {
  const QList<QTableWidgetItem*> selectedItems = ui->timeZonesTW->selectedItems();
  for (const QTableWidgetItem* item : selectedItems) {
    if (item->column() == 0) {
      if (item->text() == mDefaultTimeZone)
        mDefaultTimeZone.clear();
      ui->timeZonesTW->removeRow(item->row());
    }
  }

  if (mDefaultTimeZone.isEmpty() && ui->timeZonesTW->rowCount())
    setDefault(0);

  saveSettings();
}

void OneG4WorldClockConfiguration::setBold(QTableWidgetItem* item, bool value) {
  if (item) {
    QFont font = item->font();
    font.setBold(value);
    item->setFont(font);
  }
}

void OneG4WorldClockConfiguration::setBold(int row, bool value) {
  setBold(ui->timeZonesTW->item(row, 0), value);
  setBold(ui->timeZonesTW->item(row, 1), value);
}

void OneG4WorldClockConfiguration::setDefault(int row) {
  setBold(row, true);
  mDefaultTimeZone = ui->timeZonesTW->item(row, 0)->text();
}

void OneG4WorldClockConfiguration::setTimeZoneAsDefault() {
  setBold(findTimeZone(mDefaultTimeZone), false);

  setDefault(ui->timeZonesTW->selectedItems()[0]->row());

  saveSettings();
}

void OneG4WorldClockConfiguration::editTimeZoneCustomName() {
  const int row = ui->timeZonesTW->selectedItems()[0]->row();

  const QString oldName = ui->timeZonesTW->item(row, 1)->text();

  QInputDialog d(this);
  d.setWindowTitle(tr("Input custom time zone name"));
  d.setLabelText(tr("Custom name"));
  d.setTextValue(oldName);
  d.setWindowModality(Qt::WindowModal);
  if (d.exec()) {
    ui->timeZonesTW->item(row, 1)->setText(d.textValue());

    saveSettings();
  }
}

void OneG4WorldClockConfiguration::moveTimeZoneUp() {
  const int m = ui->timeZonesTW->rowCount();
  bool skipTop = true;
  for (int i = 0; i < m; ++i) {
    if (ui->timeZonesTW->item(i, 0)->isSelected()) {
      if (!skipTop) {
        QTableWidgetItem* itemP0 = ui->timeZonesTW->takeItem(i - 1, 0);
        QTableWidgetItem* itemP1 = ui->timeZonesTW->takeItem(i - 1, 1);
        QTableWidgetItem* itemT0 = ui->timeZonesTW->takeItem(i, 0);
        QTableWidgetItem* itemT1 = ui->timeZonesTW->takeItem(i, 1);

        ui->timeZonesTW->setItem(i - 1, 0, itemT0);
        ui->timeZonesTW->setItem(i - 1, 1, itemT1);
        ui->timeZonesTW->setItem(i, 0, itemP0);
        ui->timeZonesTW->setItem(i, 1, itemP1);

        itemT0->setSelected(true);
        itemT1->setSelected(true);
        itemP0->setSelected(false);
        itemP1->setSelected(false);
      }
    }
    else {
      skipTop = false;
    }
  }

  saveSettings();
}

void OneG4WorldClockConfiguration::moveTimeZoneDown() {
  const int m = ui->timeZonesTW->rowCount();
  bool skipBottom = true;
  for (int i = m - 1; i >= 0; --i) {
    if (ui->timeZonesTW->item(i, 0)->isSelected()) {
      if (!skipBottom) {
        QTableWidgetItem* itemN0 = ui->timeZonesTW->takeItem(i + 1, 0);
        QTableWidgetItem* itemN1 = ui->timeZonesTW->takeItem(i + 1, 1);
        QTableWidgetItem* itemT0 = ui->timeZonesTW->takeItem(i, 0);
        QTableWidgetItem* itemT1 = ui->timeZonesTW->takeItem(i, 1);

        ui->timeZonesTW->setItem(i + 1, 0, itemT0);
        ui->timeZonesTW->setItem(i + 1, 1, itemT1);
        ui->timeZonesTW->setItem(i, 0, itemN0);
        ui->timeZonesTW->setItem(i, 1, itemN1);

        itemT0->setSelected(true);
        itemT1->setSelected(true);
        itemN0->setSelected(false);
        itemN1->setSelected(false);
      }
    }
    else {
      skipBottom = false;
    }
  }

  saveSettings();
}