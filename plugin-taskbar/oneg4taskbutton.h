/* plugin-taskbar/oneg4taskbutton.h
 * Taskbar plugin implementation
 */

#ifndef ONEG4TASKBUTTON_H
#define ONEG4TASKBUTTON_H

#include <QEnterEvent>
#include <QProxyStyle>
#include <QToolButton>

class QVariantAnimation;

#include "../panel/ioneg4panel.h"

class QPainter;
class QPalette;
class QMimeData;
class OneG4TaskBar;

class IOneG4AbstractWMInterface;

class LeftAlignedTextStyle : public QProxyStyle {
  using QProxyStyle::QProxyStyle;

 public:
  virtual void drawItemText(QPainter* painter,
                            const QRect& rect,
                            int flags,
                            const QPalette& pal,
                            bool enabled,
                            const QString& text,
                            QPalette::ColorRole textRole = QPalette::NoRole) const override;
};

class OneG4TaskButton : public QToolButton {
  Q_OBJECT

  Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

 public:
  explicit OneG4TaskButton(const WId window, OneG4TaskBar* taskBar, QWidget* parent = nullptr);
  virtual ~OneG4TaskButton();

  bool isApplicationHidden() const;
  bool isApplicationActive() const;
  WId windowId() const { return mWindow; }

  bool hasUrgencyHint() const { return mUrgencyHint; }
  void setUrgencyHint(bool set);

  bool isOnDesktop(int desktop) const;
  bool isOnCurrentScreen() const;
  bool isMinimized() const;
  void updateText();

  Qt::Corner origin() const;
  virtual void setAutoRotation(bool value, IOneG4Panel::Position position);

  OneG4TaskBar* parentTaskBar() const { return mParentTaskBar; }

  static QString mimeDataFormat() { return QLatin1String("oneg4/oneg4taskbutton"); }
  /*! \return true if this button received DragEnter event (and no DragLeave event yet)
   * */
  bool hasDragAndDropHover() const;

 public slots:
  void raiseApplication();
  void minimizeApplication();
  void maximizeApplication();
  void deMaximizeApplication();
  void shadeApplication();
  void unShadeApplication();
  void closeApplication();
  void moveApplicationToDesktop();
  void moveApplication();
  void resizeApplication();
  void setApplicationLayer();

  void setOrigin(Qt::Corner);

  void updateIcon();
  void setOpacity(qreal opacity);

 protected:
  virtual void changeEvent(QEvent* event);
  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dragMoveEvent(QDragMoveEvent* event);
  virtual void dragLeaveEvent(QDragLeaveEvent* event);
  virtual void dropEvent(QDropEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  virtual void contextMenuEvent(QContextMenuEvent* event);
  void paintEvent(QPaintEvent*);
  bool event(QEvent* event) override;
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;

  void setWindowId(WId wid) { mWindow = wid; }
  virtual QMimeData* mimeData();
  static bool sDraggging;

  inline IOneG4PanelPlugin* plugin() const { return mPlugin; }

  void setTextExplicitly(const QString& str);
  void updateHoverAnimation(bool hovered);

 protected:
  // TODO: public getter instead?
  IOneG4AbstractWMInterface* mBackend;

 private:
  void moveApplicationToPrevNextDesktop(bool next);
  void moveApplicationToPrevNextMonitor(bool next);
  WId mWindow;
  bool mUrgencyHint;
  QPoint mDragStartPosition;
  Qt::Corner mOrigin;
  OneG4TaskBar* mParentTaskBar;
  IOneG4PanelPlugin* mPlugin;
  int mIconSize;
  int mWheelDelta;

  QString mExplicitlySetText;

  // Timer for when draggind something into a button (the button's window
  // must be activated so that the use can continue dragging to the window
  QTimer* mDNDTimer;

  // Timer for distinguishing between separate mouse wheel rotations
  QTimer* mWheelTimer;
  qreal mOpacity = 1.0;
  qreal mHoverProgress = 0.0;
  bool mHoverTarget = false;
  QVariantAnimation* mHoverAnimation = nullptr;
  QPalette mBasePalette;

 signals:
  void dropped(QObject* dragSource, QPoint const& pos);
  void dragging(QObject* dragSource, QPoint const& pos);
};

typedef QHash<WId, OneG4TaskButton*> OneG4TaskButtonHash;

#endif  // ONEG4TASKBUTTON_H
