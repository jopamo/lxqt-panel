/* plugin-statusnotifier/statusnotifierwidget.cpp
 * Implementation file for StatusNotifierWidget
 */

#include "statusnotifierwidget.h"

#include "../panel/ioneg4panelplugin.h"
#include "../panel/pluginsettings.h"
#include "statusnotifierproxy.h"

#include <QAbstractButton>
#include <QEnterEvent>
#include <QEvent>
#include <QToolButton>
#include <QTimer>

StatusNotifierWidget::StatusNotifierWidget(IOneG4PanelPlugin* plugin, QWidget* parent)
    : QWidget(parent),
      mPlugin(plugin),
      mAttentionPeriod(5),
      mForceVisible(false) {
  setLayout(new OneG4::GridLayout(this));

  // The button that shows all hidden items
  mShowBtn = new QToolButton(this);
  mShowBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mShowBtn->setAutoRaise(true);
  mShowBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
  mShowBtn->setText(QStringLiteral("+"));
  layout()->addWidget(mShowBtn);
  mShowBtn->hide();

  connect(mShowBtn, &QAbstractButton::clicked, this, [this] {
    if (mForceVisible)
      return;

    mShowBtn->hide();
    mHideTimer.stop();
    mForceVisible = true;

    const auto allButtons = findChildren<StatusNotifierButton*>(QString(), Qt::FindDirectChildrenOnly);
    for (StatusNotifierButton* btn : allButtons)
      btn->show();
  });

  settingsChanged();

  // Timer that hides auto-hidden items after 2 seconds
  mHideTimer.setSingleShot(true);
  mHideTimer.setInterval(2000);
  connect(&mHideTimer, &QTimer::timeout, this, [this] {
    mShowBtn->show();
    mForceVisible = false;

    const auto allButtons = findChildren<StatusNotifierButton*>(QString(), Qt::FindDirectChildrenOnly);
    for (StatusNotifierButton* btn : allButtons) {
      if (btn->hasAttention()
          || (!mAutoHideList.contains(btn->title()) && !mHideList.contains(btn->title()))) {
        continue;
      }
      btn->hide();
    }
  });

  realign();

  StatusNotifierProxy& proxy = StatusNotifierProxy::registerLifetimeUsage(this);
  connect(&proxy, &StatusNotifierProxy::StatusNotifierItemRegistered, this, &StatusNotifierWidget::itemAdded);
  connect(&proxy, &StatusNotifierProxy::StatusNotifierItemUnregistered, this, &StatusNotifierWidget::itemRemoved);

  for (const QString& service : proxy.RegisteredStatusNotifierItems())
    itemAdded(service);
}

void StatusNotifierWidget::leaveEvent(QEvent* /*event*/) {
  if (mForceVisible)
    mHideTimer.start();
}

void StatusNotifierWidget::enterEvent(QEnterEvent* /*event*/) {
  mHideTimer.stop();
}

void StatusNotifierWidget::itemAdded(QString serviceAndPath) {
  const int slash = serviceAndPath.indexOf(QLatin1Char('/'));
  if (slash <= 0)
    return;

  const QString serv = serviceAndPath.left(slash);
  const QString path = serviceAndPath.mid(slash);
  auto* button = new StatusNotifierButton(serv, path, mPlugin, this);

  mServices.insert(serviceAndPath, button);
  layout()->addWidget(button);
  button->show();

  // show/hide the added item appropriately and show mShowBtn if needed
  connect(button, &StatusNotifierButton::titleFound, this, [this, button](const QString& title) {
    mItemTitles << title;

    if (mAutoHideList.contains(title)) {
      if (!mForceVisible)
        mShowBtn->show();
      button->setAutoHide(true, mAttentionPeriod, mForceVisible);
    }
    else if (mHideList.contains(title)) {
      button->setAutoHide(false);
      if (!mForceVisible) {
        mShowBtn->show();
        button->hide();
      }
    }
  });

  // show/hide mShowBtn if needed whenever an item gets or loses attention
  connect(button, &StatusNotifierButton::attentionChanged, mShowBtn, [this, button] {
    if (button->hasAttention()) {
      if (mShowBtn->isVisible() || mForceVisible) {
        const auto allButtons = findChildren<StatusNotifierButton*>(QString(), Qt::FindDirectChildrenOnly);
        for (StatusNotifierButton* btn : allButtons) {
          if (!btn->isVisible()
              || (mForceVisible && !btn->hasAttention()
                  && (mAutoHideList.contains(btn->title()) || mHideList.contains(btn->title())))) {
            return;
          }
        }

        // there is no item in the hiding list and all auto-hiding items have attention
        mHideTimer.stop();
        mForceVisible = false;
        mShowBtn->hide();
      }
    }
    else {
      if (!mForceVisible)
        mShowBtn->show();
    }
  });
}

void StatusNotifierWidget::itemRemoved(const QString& serviceAndPath) {
  StatusNotifierButton* button = mServices.value(serviceAndPath, nullptr);
  if (!button)
    return;

  mItemTitles.removeOne(button->title());

  if (mShowBtn->isVisible() || mForceVisible) {
    bool showBtn = false;
    for (const QString& name : std::as_const(mItemTitles)) {
      if (mAutoHideList.contains(name) || mHideList.contains(name)) {
        showBtn = true;
        break;
      }
    }

    if (!showBtn) {
      mHideTimer.stop();
      mForceVisible = false;
      mShowBtn->hide();
    }
  }

  layout()->removeWidget(button);
  mServices.remove(serviceAndPath);
  button->deleteLater();
}

void StatusNotifierWidget::settingsChanged() {
  auto* grid = qobject_cast<OneG4::GridLayout*>(layout());
  if (!grid)
    return;

  if (mPlugin->settings()->value(QStringLiteral("reverseOrder"), false).toBool())
    grid->setItemsOrder(OneG4::GridLayout::ItemsOrder::LastToFirst);
  else
    grid->setItemsOrder(OneG4::GridLayout::ItemsOrder::FirstToLast);

  mAttentionPeriod = mPlugin->settings()->value(QStringLiteral("attentionPeriod"), 5).toInt();
  mAutoHideList = mPlugin->settings()->value(QStringLiteral("autoHideList")).toStringList();
  mHideList = mPlugin->settings()->value(QStringLiteral("hideList")).toStringList();

  const auto allButtons = findChildren<StatusNotifierButton*>(QString(), Qt::FindDirectChildrenOnly);
  bool showBtn = false;

  for (StatusNotifierButton* btn : allButtons) {
    const QString title = btn->title();

    if (mAutoHideList.contains(title)) {
      btn->setAutoHide(true, mAttentionPeriod);
      if (!btn->isVisible() || !btn->hasAttention())
        showBtn = true;
    }
    else if (mHideList.contains(title)) {
      showBtn = true;
      btn->setAutoHide(false);
      btn->hide();
    }
    else {
      btn->setAutoHide(false);
      btn->show();
    }
  }

  if (!showBtn) {
    mHideTimer.stop();
    mForceVisible = false;
    mShowBtn->hide();
  }
  else if (!mForceVisible) {
    mShowBtn->show();
  }
}

void StatusNotifierWidget::realign() {
  auto* grid = qobject_cast<OneG4::GridLayout*>(layout());
  if (!grid)
    return;

  grid->setEnabled(false);

  IOneG4Panel* panel = mPlugin->panel();
  if (panel->isHorizontal()) {
    grid->setRowCount(panel->lineCount());
    grid->setColumnCount(0);
  }
  else {
    grid->setColumnCount(panel->lineCount());
    grid->setRowCount(0);
  }

  grid->setEnabled(true);
}

QStringList StatusNotifierWidget::itemTitles() const {
  QStringList names = mItemTitles;
  names.removeDuplicates();
  return names;
}
