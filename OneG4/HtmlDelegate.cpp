#include "HtmlDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QTextDocument>

namespace OneG4 {

HtmlDelegate::HtmlDelegate(const QSize& iconSize, QObject* parent) : QStyledItemDelegate(parent), mIconSize(iconSize) {}

void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  painter->save();

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  QTextDocument doc;
  doc.setDefaultFont(opt.font);
  doc.setHtml(opt.text);

  opt.text = QString();
  QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

  const QRect textRect = opt.rect.adjusted(mIconSize.width() + opt.decorationSize.width() / 2, 0, 0, 0);
  painter->translate(textRect.topLeft());
  QRect clip(0, 0, textRect.width(), textRect.height());
  doc.drawContents(painter, clip);

  painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QTextDocument doc;
  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);
  doc.setDefaultFont(opt.font);
  doc.setHtml(opt.text);

  QSize size = doc.size().toSize();
  size.setHeight(qMax(size.height(), mIconSize.height()));
  size.rwidth() += mIconSize.width() + 8;
  return size;
}

}  // namespace OneG4
