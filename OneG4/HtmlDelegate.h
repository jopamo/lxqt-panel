#ifndef ONEG4_HTML_DELEGATE_H
#define ONEG4_HTML_DELEGATE_H

#include <QStyledItemDelegate>

namespace OneG4 {

class HtmlDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  explicit HtmlDelegate(const QSize& iconSize, QObject* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

 private:
  QSize mIconSize;
};

}  // namespace OneG4

#endif  // ONEG4_HTML_DELEGATE_H
