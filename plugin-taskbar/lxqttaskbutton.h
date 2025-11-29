/* plugin-taskbar/lxqttaskbutton.h
 * Taskbar plugin implementation
 */

#ifndef LXQTTASKBUTTON_H
#define LXQTTASKBUTTON_H

#include <QToolButton>
#include <QProxyStyle>

#include "../panel/ilxqtpanel.h"

class QPainter;
class QPalette;
class QMimeData;
class LXQtTaskBar;

class ILXQtAbstractWMInterface;

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

class LXQtTaskButton : public QToolButton {
  Q_OBJECT

  Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

 public:
  explicit LXQtTaskButton(const WId window, LXQtTaskBar* taskBar, QWidget* parent = nullptr);
  virtual ~LXQtTaskButton();

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
  virtual void setAutoRotation(bool value, ILXQtPanel::Position position);

  LXQtTaskBar* parentTaskBar() const { return mParentTaskBar; }

  static QString mimeDataFormat() { return QLatin1String("lxqt/lxqttaskbutton"); }
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

  void setWindowId(WId wid) { mWindow = wid; }
  virtual QMimeData* mimeData();
  static bool sDraggging;

  inline ILXQtPanelPlugin* plugin() const { return mPlugin; }

  void setTextExplicitly(const QString& str);

 protected:
  // TODO: public getter instead?
  ILXQtAbstractWMInterface* mBackend;

 private:
  void moveApplicationToPrevNextDesktop(bool next);
  void moveApplicationToPrevNextMonitor(bool next);
  WId mWindow;
  bool mUrgencyHint;
  QPoint mDragStartPosition;
  Qt::Corner mOrigin;
  LXQtTaskBar* mParentTaskBar;
  ILXQtPanelPlugin* mPlugin;
  int mIconSize;
  int mWheelDelta;

  QString mExplicitlySetText;

  // Timer for when draggind something into a button (the button's window
  // must be activated so that the use can continue dragging to the window
  QTimer* mDNDTimer;

  // Timer for distinguishing between separate mouse wheel rotations
  QTimer* mWheelTimer;

 signals:
  void dropped(QObject* dragSource, QPoint const& pos);
  void dragging(QObject* dragSource, QPoint const& pos);
};

typedef QHash<WId, LXQtTaskButton*> LXQtTaskButtonHash;

#endif  // LXQTTASKBUTTON_H