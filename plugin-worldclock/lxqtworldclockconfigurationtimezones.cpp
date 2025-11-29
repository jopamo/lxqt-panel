/* plugin-worldclock/lxqtworldclockconfigurationtimezones.cpp
 * Configuration implementation for plugin-worldclock
 */

#include <QTimeZone>

#include "lxqtworldclockconfigurationtimezones.h"

#include "ui_lxqtworldclockconfigurationtimezones.h"

LXQtWorldClockConfigurationTimeZones::LXQtWorldClockConfigurationTimeZones(QWidget* parent)
    : QDialog(parent), ui(new Ui::LXQtWorldClockConfigurationTimeZones) {
  setObjectName(QStringLiteral("WorldClockConfigurationTimeZonesWindow"));
  setWindowModality(Qt::WindowModal);
  ui->setupUi(this);

  connect(ui->timeZonesTW, &QTreeWidget::itemSelectionChanged, this,
          &LXQtWorldClockConfigurationTimeZones::itemSelectionChanged);
  connect(ui->timeZonesTW, &QTreeWidget::itemDoubleClicked, this,
          &LXQtWorldClockConfigurationTimeZones::itemDoubleClicked);
}

LXQtWorldClockConfigurationTimeZones::~LXQtWorldClockConfigurationTimeZones() {
  delete ui;
}

QString LXQtWorldClockConfigurationTimeZones::timeZone() {
  return mTimeZone;
}

void LXQtWorldClockConfigurationTimeZones::itemSelectionChanged() {
  QList<QTreeWidgetItem*> items = ui->timeZonesTW->selectedItems();
  if (!items.empty())
    mTimeZone = items[0]->data(0, Qt::UserRole).toString();
  else
    mTimeZone.clear();
}

void LXQtWorldClockConfigurationTimeZones::itemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/) {
  if (!mTimeZone.isEmpty())
    accept();
}

QTreeWidgetItem* LXQtWorldClockConfigurationTimeZones::makeSureParentsExist(
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

int LXQtWorldClockConfigurationTimeZones::updateAndExec() {
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