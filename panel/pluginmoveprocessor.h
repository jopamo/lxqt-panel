/* panel/pluginmoveprocessor.h
 * Base plugin class implementation
 */

#ifndef PLUGINMOVEPROCESSOR_H
#define PLUGINMOVEPROCESSOR_H

#include <QWidget>
#include <QVariantAnimation>
#include <QEvent>
#include "plugin.h"
#include "lxqtpanelglobals.h"

class LXQtPanelLayout;
class QLayoutItem;

class LXQT_PANEL_API PluginMoveProcessor : public QWidget {
  Q_OBJECT
 public:
  explicit PluginMoveProcessor(LXQtPanelLayout* layout, Plugin* plugin);
  ~PluginMoveProcessor();

  Plugin* plugin() const { return mPlugin; }

 signals:
  void finished();

 public slots:
  void start();

 protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);

 private slots:
  void doStart();
  void doFinish(bool cancel);

 private:
  enum MarkType { TopMark, BottomMark, LeftMark, RightMark };

  struct MousePosInfo {
    int index;
    QLayoutItem* item;
    bool after;
  };

  LXQtPanelLayout* mLayout;
  Plugin* mPlugin;
  int mDestIndex;

  MousePosInfo itemByMousePos(const QPoint mouse) const;
  void drawMark(QLayoutItem* item, MarkType markType);
};

class LXQT_PANEL_API CursorAnimation : public QVariantAnimation {
  Q_OBJECT
 public:
  void updateCurrentValue(const QVariant& value) { QCursor::setPos(value.toPoint()); }
};

#endif  // PLUGINMOVEPROCESSOR_H