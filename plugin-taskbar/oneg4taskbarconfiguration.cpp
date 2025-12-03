/* plugin-taskbar/oneg4taskbarconfiguration.cpp
 * Taskbar plugin implementation
 */

#include "oneg4taskbarconfiguration.h"
#include "ui_oneg4taskbarconfiguration.h"

#include "../panel/oneg4panelapplication.h"
#include "../panel/backends/ioneg4abstractwmiface.h"

OneG4TaskbarConfiguration::OneG4TaskbarConfiguration(PluginSettings* settings, QWidget* parent)
    : OneG4PanelPluginConfigDialog(settings, parent), ui(new Ui::OneG4TaskbarConfiguration) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("TaskbarConfigurationWindow"));
  ui->setupUi(this);

  connect(ui->buttons, &QDialogButtonBox::clicked, this, &OneG4TaskbarConfiguration::dialogButtonsAction);

  ui->buttonStyleCB->addItem(tr("Icon and text"), QLatin1String("IconText"));
  ui->buttonStyleCB->addItem(tr("Only icon"), QLatin1String("Icon"));
  ui->buttonStyleCB->addItem(tr("Only text"), QLatin1String("Text"));

  ui->wheelEventsActionCB->addItem(tr("Disabled"), 0);
  ui->wheelEventsActionCB->addItem(tr("Cycle windows on wheel scrolling"), 1);
  ui->wheelEventsActionCB->addItem(tr("Scroll up to raise, down to minimize"), 2);
  ui->wheelEventsActionCB->addItem(tr("Scroll up to minimize, down to raise"), 3);
  ui->wheelEventsActionCB->addItem(tr("Scroll up to move to next desktop, down to previous"), 4);
  ui->wheelEventsActionCB->addItem(tr("Scroll up to move to previous desktop, down to next"), 5);

  OneG4PanelApplication* a = reinterpret_cast<OneG4PanelApplication*>(qApp);
  auto wmBackend = a->getWMBackend();

  ui->showDesktopNumCB->addItem(tr("Current"), 0);
  // Note: in KWindowSystem desktops are numbered from 1..N
  const int desk_cnt = wmBackend->getWorkspacesCount();
  for (int i = 1; desk_cnt >= i; ++i) {
    auto deskName = wmBackend->getWorkspaceName(i);
    if (deskName.isEmpty())
      deskName = tr("Desktop %1").arg(i);
    ui->showDesktopNumCB->addItem(QString(QStringLiteral("%1 - %2")).arg(i).arg(deskName), i);
  }

  loadSettings();
  ui->ungroupedNextToExistingCB->setEnabled(!(ui->groupingGB->isChecked()));
  /* We use clicked() and activated(int) because these signals aren't emitting after programmatically
      change of state */
  connect(ui->limitByDesktopCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
  connect(ui->limitByDesktopCB, &QCheckBox::checkStateChanged, ui->showDesktopNumCB, &QWidget::setEnabled);
#else
  connect(ui->limitByDesktopCB, &QCheckBox::stateChanged, ui->showDesktopNumCB, &QWidget::setEnabled);
#endif
  connect(ui->showDesktopNumCB, &QComboBox::activated, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->limitByScreenCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->limitByMinimizedCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->raiseOnCurrentDesktopCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->buttonStyleCB, &QComboBox::activated, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->buttonWidthSB, &QAbstractSpinBox::editingFinished, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->buttonHeightSB, &QAbstractSpinBox::editingFinished, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->autoRotateCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->middleClickCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->groupingGB, &QGroupBox::clicked, this, [this] {
    saveSettings();
    ui->ungroupedNextToExistingCB->setEnabled(!(ui->groupingGB->isChecked()));
  });
  connect(ui->showGroupOnHoverCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->ungroupedNextToExistingCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->iconByClassCB, &QAbstractButton::clicked, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->wheelEventsActionCB, &QComboBox::activated, this, &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->wheelDeltaThresholdSB, &QAbstractSpinBox::editingFinished, this,
          &OneG4TaskbarConfiguration::saveSettings);
  connect(ui->excludeLE, &QLineEdit::editingFinished, this, &OneG4TaskbarConfiguration::saveSettings);
}

OneG4TaskbarConfiguration::~OneG4TaskbarConfiguration() {
  delete ui;
}

void OneG4TaskbarConfiguration::loadSettings() {
  const bool showOnlyOneDesktopTasks = settings().value(QStringLiteral("showOnlyOneDesktopTasks"), false).toBool();
  ui->limitByDesktopCB->setChecked(showOnlyOneDesktopTasks);
  ui->showDesktopNumCB->setCurrentIndex(
      ui->showDesktopNumCB->findData(settings().value(QStringLiteral("showDesktopNum"), 0).toInt()));
  ui->showDesktopNumCB->setEnabled(showOnlyOneDesktopTasks);
  ui->limitByScreenCB->setChecked(settings().value(QStringLiteral("showOnlyCurrentScreenTasks"), false).toBool());
  ui->limitByMinimizedCB->setChecked(settings().value(QStringLiteral("showOnlyMinimizedTasks"), false).toBool());

  ui->autoRotateCB->setChecked(settings().value(QStringLiteral("autoRotate"), true).toBool());
  ui->middleClickCB->setChecked(settings().value(QStringLiteral("closeOnMiddleClick"), true).toBool());
  ui->raiseOnCurrentDesktopCB->setChecked(settings().value(QStringLiteral("raiseOnCurrentDesktop"), false).toBool());
  ui->buttonStyleCB->setCurrentIndex(
      ui->buttonStyleCB->findData(settings().value(QStringLiteral("buttonStyle"), QLatin1String("IconText"))));
  ui->buttonWidthSB->setValue(settings().value(QStringLiteral("buttonWidth"), 220).toInt());
  ui->buttonHeightSB->setValue(settings().value(QStringLiteral("buttonHeight"), 100).toInt());
  ui->groupingGB->setChecked(settings().value(QStringLiteral("groupingEnabled"), true).toBool());
  ui->showGroupOnHoverCB->setChecked(settings().value(QStringLiteral("showGroupOnHover"), true).toBool());
  ui->ungroupedNextToExistingCB->setChecked(
      settings().value(QStringLiteral("ungroupedNextToExisting"), false).toBool());
  ui->iconByClassCB->setChecked(settings().value(QStringLiteral("iconByClass"), false).toBool());
  ui->wheelEventsActionCB->setCurrentIndex(
      ui->wheelEventsActionCB->findData(settings().value(QStringLiteral("wheelEventsAction"), 1).toInt()));
  ui->wheelDeltaThresholdSB->setValue(settings().value(QStringLiteral("wheelDeltaThreshold"), 300).toInt());
  ui->excludeLE->setText(settings().value(QStringLiteral("excludedList")).toString());
}

void OneG4TaskbarConfiguration::saveSettings() {
  settings().setValue(QStringLiteral("showOnlyOneDesktopTasks"), ui->limitByDesktopCB->isChecked());
  settings().setValue(QStringLiteral("showDesktopNum"),
                      ui->showDesktopNumCB->itemData(ui->showDesktopNumCB->currentIndex()));
  settings().setValue(QStringLiteral("showOnlyCurrentScreenTasks"), ui->limitByScreenCB->isChecked());
  settings().setValue(QStringLiteral("showOnlyMinimizedTasks"), ui->limitByMinimizedCB->isChecked());
  settings().setValue(QStringLiteral("buttonStyle"), ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
  settings().setValue(QStringLiteral("buttonWidth"), ui->buttonWidthSB->value());
  settings().setValue(QStringLiteral("buttonHeight"), ui->buttonHeightSB->value());
  settings().setValue(QStringLiteral("autoRotate"), ui->autoRotateCB->isChecked());
  settings().setValue(QStringLiteral("closeOnMiddleClick"), ui->middleClickCB->isChecked());
  settings().setValue(QStringLiteral("raiseOnCurrentDesktop"), ui->raiseOnCurrentDesktopCB->isChecked());
  settings().setValue(QStringLiteral("groupingEnabled"), ui->groupingGB->isChecked());
  settings().setValue(QStringLiteral("showGroupOnHover"), ui->showGroupOnHoverCB->isChecked());
  settings().setValue(QStringLiteral("ungroupedNextToExisting"), ui->ungroupedNextToExistingCB->isChecked());
  settings().setValue(QStringLiteral("iconByClass"), ui->iconByClassCB->isChecked());
  settings().setValue(QStringLiteral("wheelEventsAction"),
                      ui->wheelEventsActionCB->itemData(ui->wheelEventsActionCB->currentIndex()));
  settings().setValue(QStringLiteral("wheelDeltaThreshold"), ui->wheelDeltaThresholdSB->value());
  settings().setValue(QStringLiteral("excludedList"), ui->excludeLE->text());
}