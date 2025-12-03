/* OneG4/HtmlDelegate.cpp
 * HtmlDelegate class implementation
 */

#include "HtmlDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QTextDocument>

namespace OneG4 {

HtmlDelegate::HtmlDelegate(const QSize& iconSize, QObject* parent)
    : QStyledItemDelegate(parent),
      mIconSize(iconSize) {}

void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  if (!painter)
    return;

  painter->save();

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  QTextDocument doc;
  doc.setDefaultFont(opt.font);
  doc.setHtml(opt.text);

  opt.text.clear();

  QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
  if (style)
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

  QRect textRect = opt.rect;
  const int iconSpace = mIconSize.width() + opt.decorationSize.width() / 2;
  textRect.adjust(iconSpace, 0, 0, 0);
  if (textRect.width() > 0) {
    painter->translate(textRect.topLeft());
    QRect clip(0, 0, textRect.width(), textRect.height());
    doc.setTextWidth(textRect.width());
    doc.drawContents(painter, clip);
  }

  painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  QTextDocument doc;
  doc.setDefaultFont(opt.font);
  doc.setHtml(opt.text);

  QSize size = doc.size().toSize();
  size.setHeight(qMax(size.height(), mIconSize.height()));
  size.rwidth() += mIconSize.width() + 8;
  return size;
}

}  // namespace OneG4
