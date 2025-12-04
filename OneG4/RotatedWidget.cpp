/* OneG4/RotatedWidget.cpp
 * RotatedWidget class implementation
 */

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

QSize RotatedWidget::sizeHint() const {
  return mContent ? mContent->sizeHint() : QWidget::sizeHint();
}

QSize RotatedWidget::minimumSizeHint() const {
  return mContent ? mContent->minimumSizeHint() : QWidget::minimumSizeHint();
}

void RotatedWidget::adjustContentSize() {
  if (!mContent)
    return;

  mContent->adjustSize();
  updateGeometry();
}

void RotatedWidget::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  if (mContent)
    mContent->resize(event->size());
}

}  // namespace OneG4
