/* plugin-customcommand/colorLabel.cpp
 * Implementation file for colorLabel
 */

#include "colorLabel.h"
#include <QColorDialog>
#include <QStyleOptionFrame>
#include <QPainter>

ColorLabel::ColorLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
  setFrameStyle(QFrame::Panel | QFrame::Sunken);
  setLineWidth(1);
  setToolTip(tr("Click to change color"));
}

ColorLabel::~ColorLabel() {}

void ColorLabel::setColor(const QColor& color, bool announceChange) {
  if (!color.isValid() || color == color_)
    return;
  color_ = color;
  color_.setAlpha(255);  // ignore translucency
  update();
  if (announceChange)
    emit colorChanged();
}

QColor ColorLabel::getColor() const {
  return color_;
}

void ColorLabel::reset() {
  color_ = QColor();
  update();
}

void ColorLabel::mousePressEvent(QMouseEvent* /*event*/) {
  QColor color = QColorDialog::getColor(color_, window(), tr("Select Color"));
  setColor(color, true);
}

void ColorLabel::paintEvent(QPaintEvent* /*event*/) {
  QPainter p(this);
  if (color_.isValid())
    p.fillRect(contentsRect(), color_);
  QStyleOptionFrame opt;
  initStyleOption(&opt);
  style()->drawControl(QStyle::CE_ShapedFrame, &opt, &p, this);
}