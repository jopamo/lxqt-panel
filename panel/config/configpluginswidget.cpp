/* panel/config/configpluginswidget.cpp
 * Configuration dialogs and widgets
 */

#include "configpluginswidget.h"
#include "ui_configpluginswidget.h"
#include "addplugindialog.h"
#include "panelpluginsmodel.h"
#include "../plugin.h"
#include "../ioneg4panelplugin.h"

#include <OneG4/HtmlDelegate.h>
#include <QPushButton>
#include <QItemSelectionModel>

ConfigPluginsWidget::ConfigPluginsWidget(OneG4Panel* panel, QWidget* parent)
    : QWidget(parent), ui(new Ui::ConfigPluginsWidget), mPanel(panel) {
  ui->setupUi(this);

  PanelPluginsModel* plugins = mPanel->mPlugins.get();
  {
    std::unique_ptr<QItemSelectionModel> m(ui->listView_plugins->selectionModel());
    ui->listView_plugins->setModel(plugins);
  }
  {
    std::unique_ptr<QAbstractItemDelegate> d(ui->listView_plugins->itemDelegate());
    ui->listView_plugins->setItemDelegate(new OneG4::HtmlDelegate(QSize(16, 16), ui->listView_plugins));
  }
  ui->listView_plugins->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->listView_plugins->setDragEnabled(true);
  ui->listView_plugins->viewport()->setAcceptDrops(true);
  ui->listView_plugins->setDragDropMode(QAbstractItemView::InternalMove);
  ui->listView_plugins->setDropIndicatorShown(true);

  resetButtons();

  connect(ui->listView_plugins->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          &ConfigPluginsWidget::resetButtons);

  connect(ui->pushButton_moveUp, &QToolButton::clicked, this,
          [this, plugins] { plugins->onMovePluginUp(ui->listView_plugins->currentIndex()); });
  connect(ui->pushButton_moveDown, &QToolButton::clicked, this,
          [this, plugins] { plugins->onMovePluginDown(ui->listView_plugins->currentIndex()); });

  connect(ui->pushButton_addPlugin, &QPushButton::clicked, this, &ConfigPluginsWidget::showAddPluginDialog);
  connect(ui->pushButton_removePlugin, &QToolButton::clicked, this,
          [this, plugins] { plugins->onRemovePlugin(ui->listView_plugins->currentIndex()); });

  connect(ui->pushButton_pluginConfig, &QToolButton::clicked, this,
          [this, plugins] { plugins->onConfigurePlugin(ui->listView_plugins->currentIndex()); });
  connect(ui->listView_plugins, &QAbstractItemView::doubleClicked, plugins, &PanelPluginsModel::onConfigurePlugin);

  connect(plugins, &PanelPluginsModel::pluginAdded, this, &ConfigPluginsWidget::resetButtons);
  connect(plugins, &PanelPluginsModel::pluginRemoved, this, &ConfigPluginsWidget::resetButtons);
  connect(plugins, &PanelPluginsModel::pluginMoved, this, &ConfigPluginsWidget::resetButtons);
}

ConfigPluginsWidget::~ConfigPluginsWidget() {
  delete ui;
}

void ConfigPluginsWidget::reset() {}

void ConfigPluginsWidget::showAddPluginDialog() {
  if (!mAddPluginDialog) {
    mAddPluginDialog.reset(new AddPluginDialog);
    connect(mAddPluginDialog.get(), &AddPluginDialog::pluginSelected, this,
            [this](const OneG4::PluginInfo& desktopFile) { mPanel->mPlugins->addPlugin(mPanel, desktopFile); });
  }
  mAddPluginDialog->show();
  mAddPluginDialog->raise();
  mAddPluginDialog->activateWindow();
}

void ConfigPluginsWidget::resetButtons() {
  PanelPluginsModel* model = mPanel->mPlugins.get();
  QItemSelectionModel* selectionModel = ui->listView_plugins->selectionModel();
  bool hasSelection = selectionModel->hasSelection();
  bool isFirstSelected = selectionModel->isSelected(model->index(0));
  bool isLastSelected = selectionModel->isSelected(model->index(model->rowCount() - 1));

  bool hasConfigDialog = false;
  if (hasSelection) {
    Plugin const* plugin =
        ui->listView_plugins->model()->data(selectionModel->currentIndex(), Qt::UserRole).value<Plugin const*>();
    if (nullptr != plugin)
      hasConfigDialog = plugin->iPlugin()->flags().testFlag(IOneG4PanelPlugin::HaveConfigDialog);
  }

  ui->pushButton_removePlugin->setEnabled(hasSelection);
  ui->pushButton_moveUp->setEnabled(hasSelection && !isFirstSelected);
  ui->pushButton_moveDown->setEnabled(hasSelection && !isLastSelected);
  ui->pushButton_pluginConfig->setEnabled(hasSelection && hasConfigDialog);
}
