/* plugin-showdesktop/showdesktop.cpp
 * Show desktop plugin implementation
 */

#include <QAction>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <XdgIcon>
#include <LXQt/Notification>
#include "showdesktop.h"
#include "../panel/pluginsettings.h"

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqtabstractwmiface.h"

ShowDesktop::ShowDesktop(const ILXQtPanelPluginStartupInfo& startupInfo) : QObject(), ILXQtPanelPlugin(startupInfo) {
  QAction* act = new QAction(XdgIcon::fromTheme(QStringLiteral("user-desktop")), tr("Show Desktop"), this);
  connect(act, &QAction::triggered, this, &ShowDesktop::toggleShowingDesktop);

  mDNDTimer.setSingleShot(true);
  connect(&mDNDTimer, &QTimer::timeout, this, &ShowDesktop::toggleShowingDesktop, Qt::QueuedConnection);
  mDNDTimer.setInterval(700);

  mButton.setDefaultAction(act);
  mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mButton.setAutoRaise(true);
  mButton.installEventFilter(this);
  mButton.setAcceptDrops(true);
}

bool ShowDesktop::eventFilter(QObject* watched, QEvent* event) {
  if (watched == &mButton) {
    if (event->type() == QEvent::DragEnter) {
      static_cast<QDragEnterEvent*>(event)->acceptProposedAction();
      mDNDTimer.start();
    }
    else if (event->type() == QEvent::DragLeave) {
      mDNDTimer.stop();
    }
    return false;
  }
  return QObject::eventFilter(watched, event);
}

void ShowDesktop::toggleShowingDesktop() {
  LXQtPanelApplication* a = reinterpret_cast<LXQtPanelApplication*>(qApp);
  auto wmBackend = a->getWMBackend();
  wmBackend->showDesktop(!wmBackend->isShowingDesktop());
}