/* plugin-worldclock/oneg4worldclockconfigurationtimezones.cpp
 * Configuration implementation for plugin-worldclock
 */

#include <QTimeZone>

#include "oneg4worldclockconfigurationtimezones.h"

#include "ui_oneg4worldclockconfigurationtimezones.h"

OneG4WorldClockConfigurationTimeZones::OneG4WorldClockConfigurationTimeZones(QWidget* parent)
    : QDialog(parent), ui(new Ui::OneG4WorldClockConfigurationTimeZones) {
  setObjectName(QStringLiteral("WorldClockConfigurationTimeZonesWindow"));
  setWindowModality(Qt::WindowModal);
  ui->setupUi(this);

  connect(ui->timeZonesTW, &QTreeWidget::itemSelectionChanged, this,
          &OneG4WorldClockConfigurationTimeZones::itemSelectionChanged);
  connect(ui->timeZonesTW, &QTreeWidget::itemDoubleClicked, this,
          &OneG4WorldClockConfigurationTimeZones::itemDoubleClicked);
}

OneG4WorldClockConfigurationTimeZones::~OneG4WorldClockConfigurationTimeZones() {
  delete ui;
}

QString OneG4WorldClockConfigurationTimeZones::timeZone() {
  return mTimeZone;
}

void OneG4WorldClockConfigurationTimeZones::itemSelectionChanged() {
  QList<QTreeWidgetItem*> items = ui->timeZonesTW->selectedItems();
  if (!items.empty())
    mTimeZone = items[0]->data(0, Qt::UserRole).toString();
  else
    mTimeZone.clear();
}

void OneG4WorldClockConfigurationTimeZones::itemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/) {
  if (!mTimeZone.isEmpty())
    accept();
}

QTreeWidgetItem* OneG4WorldClockConfigurationTimeZones::makeSureParentsExist(
    const QStringList& parts,
    QMap<QString, QTreeWidgetItem*>& parentItems) {
  if (parts.length() == 1)
    return nullptr;

  QStringList parentParts = parts.mid(0, parts.length() - 1);

  QString parentPath = parentParts.join(QLatin1String("/"));

  QMap<QString, QTreeWidgetItem*>::Iterator I = parentItems.find(parentPath);
  if (I != parentItems.end())
    return I.value();

  QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList() << parts[parts.length() - 2]);

  QTreeWidgetItem* parentItem = makeSureParentsExist(parentParts, parentItems);

  if (!parentItem)
    ui->timeZonesTW->addTopLevelItem(newItem);
  else
    parentItem->addChild(newItem);

  parentItems[parentPath] = newItem;

  return newItem;
}

int OneG4WorldClockConfigurationTimeZones::updateAndExec() {
  QDateTime now = QDateTime::currentDateTime();

  ui->timeZonesTW->clear();

  QMap<QString, QTreeWidgetItem*> parentItems;

  const auto timeZones = QTimeZone::availableTimeZoneIds();
  for (const QByteArray& ba : timeZones) {
    QTimeZone timeZone(ba);
    QString ianaId(QString::fromUtf8(ba));
    QStringList qStrings(QString::fromUtf8((ba)).split(QLatin1Char('/')));

    if ((qStrings.size() == 1) && (qStrings[0].startsWith(QLatin1String("UTC"))))
      qStrings.prepend(tr("UTC"));

    if (qStrings.size() == 1)
      qStrings.prepend(tr("Other"));

    QTreeWidgetItem* tzItem =
        new QTreeWidgetItem(QStringList() << qStrings[qStrings.length() - 1] << timeZone.displayName(now)
                                          << timeZone.comment() << QLocale::territoryToString(timeZone.territory()));
    tzItem->setData(0, Qt::UserRole, ianaId);

    makeSureParentsExist(qStrings, parentItems)->addChild(tzItem);
  }

  QStringList qStrings = QStringList() << tr("Other") << QLatin1String("local");
  QTreeWidgetItem* tzItem = new QTreeWidgetItem(QStringList() << qStrings[qStrings.length() - 1] << QString()
                                                              << tr("Local timezone") << QString());
  tzItem->setData(0, Qt::UserRole, qStrings[qStrings.length() - 1]);
  makeSureParentsExist(qStrings, parentItems)->addChild(tzItem);

  ui->timeZonesTW->sortByColumn(0, Qt::AscendingOrder);

  return exec();
}