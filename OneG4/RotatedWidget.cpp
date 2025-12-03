#include "RotatedWidget.h"

#include <QBoxLayout>
#include <QResizeEvent>

namespace OneG4 {

RotatedWidget::RotatedWidget(QWidget& content, QWidget* parent)
    : QWidget(parent), mContent(&content), mOrigin(Qt::TopLeftCorner) {
  auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(&content);
  setLayout(layout);
}

void RotatedWidget::setOrigin(Qt::Corner corner) {
  mOrigin = corner;
  Q_UNUSED(mOrigin);
}

void RotatedWidget::adjustContentSize() {
  if (mContent)
    mContent->adjustSize();
}

void RotatedWidget::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  if (mContent)
    mContent->setFixedSize(event->size());
}

}  // namespace OneG4
