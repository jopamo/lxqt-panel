/* plugin-statusnotifier/statusnotifierbutton.h
 * Header file for statusnotifierbutton
 */

#ifndef STATUSNOTIFIERBUTTON_H
#define STATUSNOTIFIERBUTTON_H

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QMouseEvent>
#include <QToolButton>
#include <QWheelEvent>
#include <QMenu>
#include <QTimer>

class IOneG4PanelPlugin;
class SniAsync;

class StatusNotifierButton : public QToolButton {
  Q_OBJECT

 public:
  StatusNotifierButton(QString service, QString objectPath, IOneG4PanelPlugin* plugin, QWidget* parent = nullptr);
  ~StatusNotifierButton();

  enum Status { Passive, Active, NeedsAttention };

  QString title() const { return mTitle; }
  bool hasAttention() const;
  void setAutoHide(bool autoHide, int minutes = 5, bool forcedVisible = false);

 signals:
  void titleFound(const QString& title);
  void attentionChanged();

 public slots:
  void newIcon();
  void newAttentionIcon();
  void newOverlayIcon();
  void newToolTip();
  void newStatus(QString status);

 private:
  void onNeedingAttention();

  SniAsync* interface;
  QMenu* mMenu;
  Status mStatus;

  QIcon mIcon, mOverlayIcon, mAttentionIcon, mFallbackIcon;

  IOneG4PanelPlugin* mPlugin;

  QString mTitle;
  bool mAutoHide;
  QTimer mHideTimer;

 protected:
  void contextMenuEvent(QContextMenuEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  void refetchIcon(Status status, const QString& themePath);
  void resetIcon();
};

#endif  // STATUSNOTIFIERBUTTON_H