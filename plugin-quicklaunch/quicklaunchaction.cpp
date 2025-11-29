/* plugin-quicklaunch/quicklaunchaction.cpp
 * Quick launch plugin implementation
 */

#include "quicklaunchaction.h"
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <XdgDesktopFile>
#include <XdgIcon>
#include <XdgMimeType>

QuickLaunchAction::QuickLaunchAction(const QString& name, const QString& exec, const QString& icon, QWidget* parent)
    : QAction(name, parent), m_valid(true) {
  m_type = ActionLegacy;

  m_settingsMap[QStringLiteral("name")] = name;
  m_settingsMap[QStringLiteral("exec")] = exec;
  m_settingsMap[QStringLiteral("icon")] = icon;

  // Since the keys "desktop" and "file" have priority over the above keys
  // (see LXQtQuickLaunch::LXQtQuickLaunch), we prevent their reading
  // from another config file by setting them to empty strings here.
  m_settingsMap[QStringLiteral("desktop")] = QString();
  m_settingsMap[QStringLiteral("file")] = QString();

  if (icon == QLatin1String("") || icon.isNull())
    setIcon(XdgIcon::defaultApplicationIcon());
  else
    setIcon(QIcon(icon));

  setData(exec);
  connect(this, &QAction::triggered, this, [this] { execAction(); });
}

QuickLaunchAction::QuickLaunchAction(const XdgDesktopFile* xdg, QWidget* parent) : QAction(parent), m_valid(true) {
  m_type = ActionXdg;

  m_settingsMap[QStringLiteral("desktop")] = xdg->fileName();

  QString title(xdg->localizedValue(QStringLiteral("Name")).toString());
  QString gn(xdg->localizedValue(QStringLiteral("GenericName")).toString());
  if (!gn.isEmpty())
    title += QLatin1String(" (") + gn + QLatin1String(")");
  setText(title);

  setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));

  setData(xdg->fileName());
  connect(this, &QAction::triggered, this, [this] { execAction(); });

  // populate the additional actions
  for (auto const& action : const_cast<const QStringList&&>(xdg->actions())) {
    QAction* act = new QAction{xdg->actionIcon(action), xdg->actionName(action), this};
    act->setData(action);
    connect(act, &QAction::triggered, this, [this, act] { execAction(act->data().toString()); });
    m_additionalActions.push_back(act);
  }
}

QuickLaunchAction::QuickLaunchAction(const QString& fileName, QWidget* parent) : QAction(parent), m_valid(true) {
  m_type = ActionFile;
  setText(fileName);
  setData(fileName);

  m_settingsMap[QStringLiteral("file")] = fileName;
  // prevent reading of "desktop" from another config file
  m_settingsMap[QStringLiteral("desktop")] = QString();

  QFileInfo fi(fileName);
  if (fi.isDir()) {
    QFileIconProvider ip;
    setIcon(ip.icon(fi));
  }
  else {
    QMimeDatabase db;
    XdgMimeType mi(db.mimeTypeForFile(fi));
    setIcon(mi.icon());
  }

  connect(this, &QAction::triggered, this, [this] { execAction(); });
}

void QuickLaunchAction::execAction(QString additionalAction) {
  QString exec(data().toString());
  qDebug() << "execAction" << exec;
  switch (m_type) {
    case ActionLegacy: {
      QStringList args = QProcess::splitCommand(exec);
      QProcess::startDetached(args.takeFirst(), args);
      break;
    }
    case ActionXdg: {
      XdgDesktopFile xdg;
      if (xdg.load(exec)) {
        if (additionalAction.isEmpty())
          xdg.startDetached();
        else
          xdg.actionActivate(additionalAction, QStringList{});
      }
      break;
    }
    case ActionFile:
      QDesktopServices::openUrl(QUrl(exec));
      break;
  }
}

void QuickLaunchAction::updateXdgAction() {
  if (m_valid && m_type == ActionXdg) {
    XdgDesktopFile xdg;
    if (xdg.load(data().toString()) && xdg.isSuitable()) {
      QString title(xdg.localizedValue(QStringLiteral("Name")).toString());
      QString gn(xdg.localizedValue(QStringLiteral("GenericName")).toString());
      if (!gn.isEmpty())
        title += QLatin1String(" (") + gn + QLatin1String(")");
      setText(title);
      setIcon(xdg.icon(XdgIcon::defaultApplicationIcon()));

      qDeleteAll(m_additionalActions);
      m_additionalActions.clear();
      for (auto const& action : const_cast<const QStringList&&>(xdg.actions())) {
        QAction* act = new QAction{xdg.actionIcon(action), xdg.actionName(action), this};
        act->setData(action);
        connect(act, &QAction::triggered, this, [this, act] { execAction(act->data().toString()); });
        m_additionalActions.push_back(act);
      }
    }
    else {
      qDebug() << "XdgDesktopFile" << data() << "is not valid or applicable";
    }
  }
}