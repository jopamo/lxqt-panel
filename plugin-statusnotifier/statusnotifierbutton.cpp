/* plugin-statusnotifier/statusnotifierbutton.cpp
 * Implementation file for StatusNotifierButton
 */

#include "statusnotifierbutton.h"

#include <OneG4/XdgIcon.h>

#include <QContextMenuEvent>
#include <QCursor>
#include <QDBusConnection>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QMenu>
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <QToolButton>
#include <QWheelEvent>
#include <QtEndian>
#include <QtGlobal>

#include "../panel/ioneg4panelplugin.h"
#include "sniasync.h"

#include <algorithm>

namespace {

/*!
 * Lightweight stub for DBusMenuImporter to avoid external dependency
 */
class MenuImporter : public QObject {
 public:
  MenuImporter(const QString&, const QString&, QObject* parent)
      : QObject(parent) {
    QWidget* widgetParent = qobject_cast<QWidget*>(parent);
    m_menu = new QMenu(widgetParent);
    m_menu->addAction(tr("Menu not available (Stub)"));
  }

  QMenu* menu() const {
    return m_menu;
  }

 private:
  QMenu* m_menu = nullptr;
};

}  // namespace

StatusNotifierButton::StatusNotifierButton(QString service,
                                           QString objectPath,
                                           IOneG4PanelPlugin* plugin,
                                           QWidget* parent)
    : QToolButton(parent),
      mMenu(nullptr),
      mStatus(Passive),
      mFallbackIcon(QIcon::fromTheme(QLatin1String("application-x-executable"))),
      mPlugin(plugin),
      mAutoHide(false) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setAutoRaise(true);

  interface = new SniAsync(std::move(service), std::move(objectPath), QDBusConnection::sessionBus(), this);

  connect(interface, &SniAsync::NewIcon, this, &StatusNotifierButton::newIcon);
  connect(interface, &SniAsync::NewOverlayIcon, this, &StatusNotifierButton::newOverlayIcon);
  connect(interface, &SniAsync::NewAttentionIcon, this, &StatusNotifierButton::newAttentionIcon);
  connect(interface, &SniAsync::NewToolTip, this, &StatusNotifierButton::newToolTip);
  connect(interface, &SniAsync::NewStatus, this, &StatusNotifierButton::newStatus);

  // get the title only at the start because that title is used for deciding about (auto-)hiding
  interface->propertyGetAsync(QLatin1String("Title"), [this](const QString& value) {
    mTitle = value;
    QTimer::singleShot(0, this, [this] {
      Q_EMIT titleFound(mTitle);
    });
  });

  interface->propertyGetAsync(QLatin1String("Menu"), [this](const QDBusObjectPath& path) {
    if (!path.path().isEmpty()) {
      mMenu = (new MenuImporter(interface->service(), path.path(), this))->menu();
      if (mMenu)
        mMenu->setObjectName(QLatin1String("StatusNotifierMenu"));
    }
  });

  interface->propertyGetAsync(QLatin1String("Status"), [this](const QString& status) {
    newStatus(status);
  });

  interface->propertyGetAsync(QLatin1String("IconThemePath"), [this](const QString& value) {
    // do the logic of icons after we've got the theme path
    refetchIcon(Active, value);
    refetchIcon(Passive, value);
    refetchIcon(NeedsAttention, value);
  });

  newToolTip();

  // timer that hides an auto-hiding button after it gets attention
  mHideTimer.setSingleShot(true);
  mHideTimer.setInterval(300000);
  connect(&mHideTimer, &QTimer::timeout, this, [this] {
    hide();
    Q_EMIT attentionChanged();
  });
}

StatusNotifierButton::~StatusNotifierButton() {
  delete interface;
}

void StatusNotifierButton::newIcon() {
  if (!icon().isNull() && icon().name() != QLatin1String("application-x-executable"))
    onNeedingAttention();

  interface->propertyGetAsync(QLatin1String("IconThemePath"), [this](const QString& value) {
    refetchIcon(Passive, value);
  });
}

void StatusNotifierButton::newOverlayIcon() {
  onNeedingAttention();

  interface->propertyGetAsync(QLatin1String("IconThemePath"), [this](const QString& value) {
    refetchIcon(Active, value);
  });
}

void StatusNotifierButton::newAttentionIcon() {
  onNeedingAttention();

  interface->propertyGetAsync(QLatin1String("IconThemePath"), [this](const QString& value) {
    refetchIcon(NeedsAttention, value);
  });
}

void StatusNotifierButton::refetchIcon(Status status, const QString& themePath) {
  QString nameProperty;
  QString pixmapProperty;

  switch (status) {
    case Active:
      nameProperty = QLatin1String("OverlayIconName");
      pixmapProperty = QLatin1String("OverlayIconPixmap");
      break;
    case NeedsAttention:
      nameProperty = QLatin1String("AttentionIconName");
      pixmapProperty = QLatin1String("AttentionIconPixmap");
      break;
    case Passive:
    default:
      nameProperty = QLatin1String("IconName");
      pixmapProperty = QLatin1String("IconPixmap");
      break;
  }

  interface->propertyGetAsync(nameProperty,
                              [this, status, pixmapProperty, themePath](const QString& iconName) {
                                if (!iconName.isEmpty()) {
                                  QIcon nextIcon = QIcon::fromTheme(iconName);
                                  if (nextIcon.isNull()) {
                                    QDir themeDir(themePath);
                                    if (themeDir.exists()) {
                                      const bool hasExtension =
                                          iconName.endsWith(QStringLiteral(".png"), Qt::CaseInsensitive) ||
                                          iconName.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive) ||
                                          iconName.endsWith(QStringLiteral(".xpm"), Qt::CaseInsensitive);

                                      if (hasExtension) {
                                        if (themeDir.exists(iconName))
                                          nextIcon.addFile(themeDir.filePath(iconName));
                                      }
                                      else {
                                        const QString basePng = iconName + QStringLiteral(".png");
                                        const QString baseSvg = iconName + QStringLiteral(".svg");
                                        const QString baseXpm = iconName + QStringLiteral(".xpm");

                                        if (themeDir.exists(basePng))
                                          nextIcon.addFile(themeDir.filePath(basePng));
                                        if (themeDir.exists(baseSvg))
                                          nextIcon.addFile(themeDir.filePath(baseSvg));
                                        if (themeDir.exists(baseXpm))
                                          nextIcon.addFile(themeDir.filePath(baseXpm));
                                      }

                                      if (themeDir.cd(QStringLiteral("hicolor")) ||
                                          (themeDir.cd(QStringLiteral("icons")) &&
                                           themeDir.cd(QStringLiteral("hicolor")))) {
                                        const QStringList sizes =
                                            themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                                        for (const QString& dir : sizes) {
                                          const QDir sizeDir(themeDir.filePath(dir));
                                          const QStringList innerDirs =
                                              sizeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                                          for (const QString& innerDir : innerDirs) {
                                            QString path = themeDir.absolutePath() + QLatin1Char('/') + dir +
                                                           QLatin1Char('/') + innerDir + QLatin1Char('/') + iconName;
                                            if (hasExtension) {
                                              if (QFile::exists(path))
                                                nextIcon.addFile(path);
                                            }
                                            else {
                                              const QString pngPath = path + QStringLiteral(".png");
                                              const QString svgPath = path + QStringLiteral(".svg");
                                              const QString xpmPath = path + QStringLiteral(".xpm");

                                              if (QFile::exists(pngPath))
                                                nextIcon.addFile(pngPath);
                                              if (QFile::exists(svgPath))
                                                nextIcon.addFile(svgPath);
                                              if (QFile::exists(xpmPath))
                                                nextIcon.addFile(xpmPath);
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }

                                  switch (status) {
                                    case Active:
                                      mOverlayIcon = nextIcon;
                                      break;
                                    case NeedsAttention:
                                      mAttentionIcon = nextIcon;
                                      break;
                                    case Passive:
                                    default:
                                      mIcon = nextIcon;
                                      break;
                                  }

                                  resetIcon();
                                }
                                else {
                                  interface->propertyGetAsync(
                                      pixmapProperty,
                                      [this, status](const IconPixmapList& iconPixmaps) {
                                        if (iconPixmaps.empty())
                                          return;

                                        QIcon nextIcon;

                                        for (const IconPixmap& iconPixmap : iconPixmaps) {
                                          if (iconPixmap.bytes.isNull())
                                            continue;

                                          QByteArray normalizedBytes(iconPixmap.bytes);
                                          const uchar* src =
                                              reinterpret_cast<const uchar*>(iconPixmap.bytes.constData());
                                          const uchar* end = src + iconPixmap.bytes.size();
                                          uchar* dest = reinterpret_cast<uchar*>(normalizedBytes.data());

                                          while (src < end) {
                                            const quint32 px = qFromBigEndian<quint32>(src);
                                            qToUnaligned(px, dest);
                                            src += 4;
                                            dest += 4;
                                          }

                                          QImage image(reinterpret_cast<const uchar*>(normalizedBytes.constData()),
                                                       iconPixmap.width,
                                                       iconPixmap.height,
                                                       QImage::Format_ARGB32);

                                          nextIcon.addPixmap(QPixmap::fromImage(image));
                                        }

                                        switch (status) {
                                          case Active:
                                            mOverlayIcon = nextIcon;
                                            break;
                                          case NeedsAttention:
                                            mAttentionIcon = nextIcon;
                                            break;
                                          case Passive:
                                          default:
                                            mIcon = nextIcon;
                                            break;
                                        }

                                        resetIcon();
                                      });
                                }
                              });
}

void StatusNotifierButton::newToolTip() {
  interface->propertyGetAsync(QLatin1String("ToolTip"), [this](const ToolTip& tooltip) {
    const QString toolTipTitle = tooltip.title;
    if (!toolTipTitle.isEmpty()) {
      setToolTip(toolTipTitle);
    }
    else {
      interface->propertyGetAsync(QLatin1String("Title"), [this](const QString& title) {
        if (!title.isEmpty())
          setToolTip(title);
      });
    }
  });
}

void StatusNotifierButton::newStatus(QString status) {
  Status newStatusValue;

  if (status == QLatin1String("Passive"))
    newStatusValue = Passive;
  else if (status == QLatin1String("Active"))
    newStatusValue = Active;
  else
    newStatusValue = NeedsAttention;

  if (mStatus == newStatusValue)
    return;

  mStatus = newStatusValue;

  if (mStatus == NeedsAttention)
    onNeedingAttention();

  resetIcon();
}

void StatusNotifierButton::contextMenuEvent(QContextMenuEvent* /*event*/) {
  // avoid showing parent's context menu, we (optionally) provide our own in mouseReleaseEvent
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent* event) {
  if (!event) {
    QToolButton::mouseReleaseEvent(event);
    return;
  }

  if (event->button() == Qt::LeftButton) {
    interface->Activate(QCursor::pos().x(), QCursor::pos().y());
  }
  else if (event->button() == Qt::MiddleButton) {
    interface->SecondaryActivate(QCursor::pos().x(), QCursor::pos().y());
  }
  else if (event->button() == Qt::RightButton) {
    if (mMenu) {
      mPlugin->willShowWindow(mMenu);
      const QRect posRect = mPlugin->panel()->calculatePopupWindowPos(QCursor::pos(), mMenu->sizeHint());
      mMenu->popup(posRect.topLeft());
    }
    else {
      interface->ContextMenu(QCursor::pos().x(), QCursor::pos().y());
    }
  }

  QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent* event) {
  if (!event)
    return;

  const QPoint angleDelta = event->angleDelta();
  const Qt::Orientation orient =
      (qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
  const int delta = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());

  interface->Scroll(delta, QStringLiteral("vertical"));
}

void StatusNotifierButton::resetIcon() {
  if (mStatus == Active && !mOverlayIcon.isNull())
    setIcon(mOverlayIcon);
  else if (mStatus == NeedsAttention && !mAttentionIcon.isNull())
    setIcon(mAttentionIcon);
  else if (!mIcon.isNull())
    setIcon(mIcon);
  else if (!mOverlayIcon.isNull())
    setIcon(mOverlayIcon);
  else if (!mAttentionIcon.isNull())
    setIcon(mAttentionIcon);
  else
    setIcon(mFallbackIcon);
}

void StatusNotifierButton::setAutoHide(bool autoHide, int minutes, bool forcedVisible) {
  if (autoHide)
    mHideTimer.setInterval(std::clamp(minutes, 1, 60) * 60000);

  if (mAutoHide != autoHide) {
    mAutoHide = autoHide;
    setVisible(!mAutoHide || forcedVisible);
    if (!mAutoHide)
      mHideTimer.stop();
  }
}

void StatusNotifierButton::onNeedingAttention() {
  if (!mAutoHide)
    return;

  show();
  mHideTimer.start();
  Q_EMIT attentionChanged();
}

bool StatusNotifierButton::hasAttention() const {
  return mHideTimer.isActive();
}
