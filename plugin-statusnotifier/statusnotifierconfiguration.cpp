/* plugin-statusnotifier/statusnotifierconfiguration.cpp
 * Configuration implementation for plugin-statusnotifier
 */

#include "statusnotifierconfiguration.h"
#include "ui_statusnotifierconfiguration.h"
#include <QPushButton>
#include <QComboBox>

StatusNotifierConfiguration::StatusNotifierConfiguration(PluginSettings* settings, QWidget* parent)
    : OneG4PanelPluginConfigDialog(settings, parent), ui(new Ui::StatusNotifierConfiguration) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("StatusNotifierConfigurationWindow"));
  ui->setupUi(this);

  if (QPushButton* closeBtn = ui->buttons->button(QDialogButtonBox::Close))
    closeBtn->setDefault(true);
  connect(ui->buttons, &QDialogButtonBox::clicked, this, &StatusNotifierConfiguration::dialogButtonsAction);

  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
  ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);

  loadSettings();

  connect(ui->orderCB, &QCheckBox::toggled, this, &StatusNotifierConfiguration::saveSettings);
  connect(ui->attentionSB, &QAbstractSpinBox::editingFinished, this, &StatusNotifierConfiguration::saveSettings);
}

StatusNotifierConfiguration::~StatusNotifierConfiguration() {
  delete ui;
}

void StatusNotifierConfiguration::loadSettings() {
  ui->orderCB->setChecked(settings().value(QStringLiteral("reverseOrder"), false).toBool());
  ui->attentionSB->setValue(settings().value(QStringLiteral("attentionPeriod"), 5).toInt());
  mAutoHideList = settings().value(QStringLiteral("autoHideList")).toStringList();
  mHideList = settings().value(QStringLiteral("hideList")).toStringList();
}

void StatusNotifierConfiguration::saveSettings() {
  settings().setValue(QStringLiteral("reverseOrder"), ui->orderCB->isChecked());
  settings().setValue(QStringLiteral("attentionPeriod"), ui->attentionSB->value());
  settings().setValue(QStringLiteral("autoHideList"), mAutoHideList);
  settings().setValue(QStringLiteral("hideList"), mHideList);
}

void StatusNotifierConfiguration::addItems(const QStringList& items) {
  ui->tableWidget->setRowCount(items.size());
  ui->tableWidget->setSortingEnabled(false);
  int index = 0;
  for (const auto& item : items) {
    // first column
    QTableWidgetItem* widgetItem = new QTableWidgetItem(item);
    widgetItem->setFlags(widgetItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    ui->tableWidget->setItem(index, 0, widgetItem);
    // second column
    QComboBox* cb = new QComboBox();
    cb->addItems(QStringList() << tr("Always show") << tr("Auto-hide") << tr("Always hide"));
    if (mAutoHideList.contains(item))
      cb->setCurrentIndex(1);
    else if (mHideList.contains(item))
      cb->setCurrentIndex(2);
    connect(cb, &QComboBox::currentIndexChanged, this, [this, item](int indx) {
      if (indx == 0) {
        mAutoHideList.removeAll(item);
        mHideList.removeAll(item);
      }
      else if (indx == 1) {
        mHideList.removeAll(item);
        if (!mAutoHideList.contains(item))
          mAutoHideList << item;
      }
      else if (indx == 2) {
        mAutoHideList.removeAll(item);
        if (!mHideList.contains(item))
          mHideList << item;
      }
      saveSettings();
    });
    ui->tableWidget->setCellWidget(index, 1, cb);
    ++index;
  }
  ui->tableWidget->setSortingEnabled(true);
  ui->tableWidget->horizontalHeader()->setSortIndicatorShown(false);
  ui->tableWidget->setCurrentCell(0, 1);
}

void StatusNotifierConfiguration::dialogButtonsAction(QAbstractButton* btn) {
  OneG4PanelPluginConfigDialog::dialogButtonsAction(btn);
  // also, apply the changes to the visibilities list if the Reset button is clicked
  QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(btn->parent());
  if (box && box->buttonRole(btn) == QDialogButtonBox::ResetRole) {
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
      if (auto cb = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 1))) {
        if (QTableWidgetItem* widgetItem = ui->tableWidget->item(i, 0)) {
          cb->blockSignals(true);  // we neither change visibility lists nor save settings here
          if (mAutoHideList.contains(widgetItem->text()))
            cb->setCurrentIndex(1);
          else if (mHideList.contains(widgetItem->text()))
            cb->setCurrentIndex(2);
          else
            cb->setCurrentIndex(0);
          cb->blockSignals(false);
        }
      }
    }
  }
}