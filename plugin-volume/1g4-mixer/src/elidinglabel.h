/* plugin-volume/1g4-mixer/src/elidinglabel.h
 * Implementation of elidinglabel.h
 */

#ifndef elidinglabel_h
#define elidinglabel_h

#include <QLabel>

class ElidingLabel : public QLabel {
  Q_OBJECT

 public:
  explicit ElidingLabel(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QString elidedText_;
  QString lastText_;
  int lastWidth_;
};

#endif  // elidinglabel_h
