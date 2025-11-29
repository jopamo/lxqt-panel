/* plugin-cpuload/lxqtcpuload.h
 * Header file for lxqtcpuload
 */

#ifndef LXQTCPULOAD_H
#define LXQTCPULOAD_H
#include <QLabel>

class ILXQtPanelPlugin;

class LXQtCpuLoad : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor fontColor READ getFontColor WRITE setFontColor)

 public:
  /**
    Describes orientation of cpu load bar
   **/
  enum BarOrientation {
    BottomUpBar,     //! Bar begins at bottom and grows up
    TopDownBar,      //! Bar begins at top and grows down
    RightToLeftBar,  //! Bar begins at right edge and grows to the left
    LeftToRightBar   //! Bar begins at left edge and grows to the right
  };

  LXQtCpuLoad(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~LXQtCpuLoad();

  void settingsChanged();

  void setFontColor(QColor value) { fontColor = value; }
  QColor getFontColor() const { return fontColor; }

 protected:
  void virtual timerEvent(QTimerEvent* event);
  void virtual paintEvent(QPaintEvent* event);
  void virtual resizeEvent(QResizeEvent*);

 private:
  double getLoadCpu() const;
  void setSizes();

  ILXQtPanelPlugin* mPlugin;
  QWidget m_stuff;

  //! average load
  int m_avg;

  bool m_showText;
  int m_barWidth;
  BarOrientation m_barOrientation;
  int m_updateInterval;
  int m_timerID;

  QFont m_font;

  QColor fontColor;
};

#endif  // LXQTCPULOAD_H