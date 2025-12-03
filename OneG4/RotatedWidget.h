#ifndef ONEG4_ROTATED_WIDGET_H
#define ONEG4_ROTATED_WIDGET_H

#include <QWidget>

namespace OneG4 {

class RotatedWidget : public QWidget {
  Q_OBJECT

 public:
  RotatedWidget(QWidget& content, QWidget* parent = nullptr);

  void setOrigin(Qt::Corner corner);
  void adjustContentSize();

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  QWidget* mContent;
  Qt::Corner mOrigin;
};

}  // namespace OneG4

#endif  // ONEG4_ROTATED_WIDGET_H
