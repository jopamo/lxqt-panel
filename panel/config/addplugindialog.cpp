/* panel/config/addplugindialog.cpp
 * Configuration dialogs and widgets
 */

#include "ui_addplugindialog.h"
#include "addplugindialog.h"
#include "plugin.h"
#include "../oneg4panel.h"
#include "../oneg4panelapplication.h"

#include <OneG4/HtmlDelegate.h>
#include <XdgIcon.h>

#include <QString>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QIcon>

#define SEARCH_ROLE Qt::UserRole
#define INDEX_ROLE SEARCH_ROLE + 1

AddPluginDialog::AddPluginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AddPluginDialog) {
  ui->setupUi(this);

  const QStringList desktopFilesDirs = pluginDesktopDirs();
  mPlugins = OneG4::PluginInfo::search(desktopFilesDirs, QLatin1String("OneG4Panel/Plugin"), QLatin1String("*"));
  std::sort(mPlugins.begin(), mPlugins.end(), [](const OneG4::PluginInfo& p1, const OneG4::PluginInfo& p2) {
    return p1.name() < p2.name() || (p1.name() == p2.name() && p1.comment() < p2.comment());
  });

  ui->pluginList->setItemDelegate(new OneG4::HtmlDelegate(QSize(32, 32), ui->pluginList));
  ui->pluginList->setContextMenuPolicy(Qt::CustomContextMenu);

  filter();

  // search
  mSearchTimer.setInterval(300);
  mSearchTimer.setSingleShot(true);
  connect(ui->searchEdit, &QLineEdit::textEdited, &mSearchTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
  connect(&mSearchTimer, &QTimer::timeout, this, &AddPluginDialog::filter);
  connect(ui->pluginList, &QListWidget::doubleClicked, this, &AddPluginDialog::emitPluginSelected);
  connect(ui->addButton, &QPushButton::clicked, this, &AddPluginDialog::emitPluginSelected);

  connect(dynamic_cast<OneG4PanelApplication*>(qApp), &OneG4PanelApplication::pluginAdded, this,
          &AddPluginDialog::filter);
  connect(dynamic_cast<OneG4PanelApplication*>(qApp), &OneG4PanelApplication::pluginRemoved, this,
          &AddPluginDialog::filter);
}

AddPluginDialog::~AddPluginDialog() {
  delete ui;
}

void AddPluginDialog::filter() {
  QListWidget* pluginList = ui->pluginList;

  const int curr_item = 0 < pluginList->count() ? pluginList->currentRow() : 0;
  pluginList->clear();

  static QIcon fallIco = XdgIcon::fromTheme(QStringLiteral("preferences-plugin"));

  int pluginCount = mPlugins.length();
  for (int i = 0; i < pluginCount; ++i) {
    const OneG4::PluginInfo& plugin = mPlugins.at(i);

    QString s = QStringLiteral("%1 %2 %3 %4 %5")
                    .arg(plugin.name(), plugin.comment(), plugin.value(QStringLiteral("Name")).toString(),
                         plugin.value(QStringLiteral("Comment")).toString(), plugin.id());
    if (!s.contains(ui->searchEdit->text(), Qt::CaseInsensitive))
      continue;

    QListWidgetItem* item = new QListWidgetItem(ui->pluginList);
    // disable single-instances plugins already in use
    if (dynamic_cast<OneG4PanelApplication const*>(qApp)->isPluginSingletonAndRunning(plugin.id())) {
      item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
      item->setBackground(palette().brush(QPalette::Disabled, QPalette::Text));
      item->setText(
          QStringLiteral("<b>%1</b> (%2)<br>%3<br><small>%4</small>")
              .arg(plugin.name(), plugin.id(), plugin.comment(), tr("(only one instance can run at a time)")));
    }
    else
      item->setText(QStringLiteral("<b>%1</b> (%2)<br>%3").arg(plugin.name(), plugin.id(), plugin.comment()));
    item->setIcon(plugin.icon(fallIco));
    item->setData(INDEX_ROLE, i);
  }

  if (pluginCount > 0)
    ui->pluginList->setCurrentRow(curr_item < pluginCount ? curr_item : pluginCount - 1);
}

void AddPluginDialog::emitPluginSelected() {
  QListWidget* pluginList = ui->pluginList;
  if (pluginList->currentItem() && pluginList->currentItem()->isSelected()) {
    OneG4::PluginInfo plugin = mPlugins.at(pluginList->currentItem()->data(INDEX_ROLE).toInt());
    emit pluginSelected(plugin);
  }
}
