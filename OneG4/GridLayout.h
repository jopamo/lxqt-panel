#ifndef ONEG4_GRID_LAYOUT_H
#define ONEG4_GRID_LAYOUT_H

#include <QLayout>
#include <QVector>

namespace OneG4 {

class GridLayout : public QLayout {
  Q_OBJECT

 public:
  enum StretchFlag {
    StretchHorizontal = 0x1,
    StretchVertical = 0x2,
  };
  Q_DECLARE_FLAGS(StretchFlags, StretchFlag)

  enum Direction {
    LeftToRight,
    RightToLeft,
    TopToBottom,
    BottomToTop,
  };

  enum ItemsOrder {
    FirstToLast,
    LastToFirst,
  };

  explicit GridLayout(QWidget* parent = nullptr);
  ~GridLayout() override;

  void addItem(QLayoutItem* item) override;
  void addWidget(QWidget* widget);

  int count() const override;
  QLayoutItem* itemAt(int index) const override;
  QLayoutItem* takeAt(int index) override;

  QSize sizeHint() const override;
  QSize minimumSize() const override;
  void setGeometry(const QRect& rect) override;

  int indexOf(const QWidget* widget) const override;
  int indexOf(const QLayoutItem* item) const override;
  QRect occupiedGeometry() const;

  void setStretch(StretchFlags stretch);
  void setDirection(Direction direction);
  void setItemsOrder(ItemsOrder order);
  void setRowCount(int rows);
  void setColumnCount(int columns);
  void setCellMinimumSize(const QSize& size);
  void setCellMaximumSize(const QSize& size);

  void moveItem(int from, int to, bool animate);
  bool animatedMoveInProgress() const;

 private:
  QVector<QLayoutItem*> mItems;
  StretchFlags mStretch;
  Direction mDirection;
  ItemsOrder mOrder;
  int mRowCount;
  int mColumnCount;
  QSize mCellMin;
  QSize mCellMax;
};

}  // namespace OneG4

Q_DECLARE_OPERATORS_FOR_FLAGS(OneG4::GridLayout::StretchFlags)

#endif  // ONEG4_GRID_LAYOUT_H
