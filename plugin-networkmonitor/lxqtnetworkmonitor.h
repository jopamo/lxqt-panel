/* plugin-networkmonitor/lxqtnetworkmonitor.h
 * Network monitor plugin implementation
 */

#ifndef LXQTNETWORKMONITOR_H
#define LXQTNETWORKMONITOR_H
#include <QFrame>

class ILXQtPanelPlugin;

/*!
  TODO: How to define cable is not connected?
  */
class LXQtNetworkMonitor : public QFrame {
  Q_OBJECT
 public:
  LXQtNetworkMonitor(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~LXQtNetworkMonitor();
  virtual void settingsChanged();

 protected:
  void virtual timerEvent(QTimerEvent* event);
  void virtual paintEvent(QPaintEvent* event);
  void virtual resizeEvent(QResizeEvent*);
  bool virtual event(QEvent* event);

 private:
  static QString convertUnits(double num);
  QString iconName(const QString& state) const {
    return QStringLiteral(":/images/knemo-%1-%2.png").arg(m_iconList[m_iconIndex], state);
  }

  QWidget m_stuff;

  QStringList m_iconList;

  int m_iconIndex;

  QString m_interface;
  QPixmap m_pic;
  ILXQtPanelPlugin* mPlugin;
};

#endif  // LXQTNETWORKMONITOR_H