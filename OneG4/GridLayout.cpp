/* OneG4/GridLayout.cpp
 * GridLayout class implementation
 */

#include "GridLayout.h"

#include <QSizePolicy>
#include <QStyle>
#include <QWidget>
#include <QWidgetItem>

namespace {

int effectiveSpacing(const QLayout* layout, Qt::Orientation orientation) {
  if (!layout)
    return 0;

  const int spacing = layout->spacing();
  if (spacing >= 0)
    return spacing;

  QWidget* parentWidget = layout->parentWidget();
  if (!parentWidget)
    return 0;

  QStyle* style = parentWidget->style();
  return style->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, orientation, nullptr, parentWidget);
}

}  // namespace

namespace OneG4 {

GridLayout::GridLayout(QWidget* parent)
    : QLayout(parent),
      mStretch(StretchHorizontal | StretchVertical),
      mDirection(LeftToRight),
      mOrder(FirstToLast),
      mRowCount(1),
      mColumnCount(0),
      mCellMin(QSize(0, 0)),
      mCellMax(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)) {}

GridLayout::~GridLayout() {
  while (!mItems.isEmpty())
    delete takeAt(0);
}

void GridLayout::addItem(QLayoutItem* item) {
  mItems.append(item);
  invalidate();
}

void GridLayout::addWidget(QWidget* widget) {
  addChildWidget(widget);
  addItem(new QWidgetItem(widget));
}

int GridLayout::count() const {
  return mItems.count();
}

QLayoutItem* GridLayout::itemAt(int index) const {
  if (index < 0 || index >= mItems.count())
    return nullptr;
  return mItems.at(index);
}

QLayoutItem* GridLayout::takeAt(int index) {
  if (index < 0 || index >= mItems.count())
    return nullptr;

  QLayoutItem* item = mItems.takeAt(index);
  invalidate();
  return item;
}

QSize GridLayout::sizeHint() const {
  int cellW = 0;
  int cellH = 0;

  for (QLayoutItem* item : mItems) {
    const QSize hint = item->sizeHint();
    cellW = qMax(cellW, hint.width());
    cellH = qMax(cellH, hint.height());
  }

  const int itemCount = mItems.count();
  const int rows = qMax(1, mRowCount > 0 ? mRowCount
                                         : ((itemCount + qMax(1, mColumnCount) - 1) / qMax(1, mColumnCount)));
  const int columns = qMax(1, mColumnCount > 0 ? mColumnCount : ((itemCount + rows - 1) / rows));

  const int spacingX = effectiveSpacing(this, Qt::Horizontal);
  const int spacingY = effectiveSpacing(this, Qt::Vertical);

  const int totalW = columns * cellW + (columns - 1) * spacingX;
  const int totalH = rows * cellH + (rows - 1) * spacingY;

  return QSize(totalW, totalH);
}

QSize GridLayout::minimumSize() const {
  return sizeHint();
}

static QVector<int> orderedIndices(int count, OneG4::GridLayout::ItemsOrder order) {
  QVector<int> indexes;
  indexes.reserve(count);

  if (order == OneG4::GridLayout::FirstToLast) {
    for (int i = 0; i < count; ++i)
      indexes.append(i);
  }
  else {
    for (int i = count - 1; i >= 0; --i)
      indexes.append(i);
  }

  return indexes;
}

void GridLayout::setGeometry(const QRect& rect) {
  QLayout::setGeometry(rect);

  if (mItems.isEmpty())
    return;

  const QVector<int> indexes = orderedIndices(mItems.count(), mOrder);
  const bool verticalFlow = (mDirection == TopToBottom || mDirection == BottomToTop);

  int rows = mRowCount;
  int columns = mColumnCount;

  if (verticalFlow) {
    if (columns <= 0)
      columns = qMax(1, rows > 0 ? rows : 1);
    rows = qMax(1, (mItems.count() + columns - 1) / columns);
  }
  else {
    if (rows <= 0)
      rows = qMax(1, columns > 0 ? columns : 1);
    columns = qMax(1, (mItems.count() + rows - 1) / rows);
  }

  const int spacingX = effectiveSpacing(this, Qt::Horizontal);
  const int spacingY = effectiveSpacing(this, Qt::Vertical);

  const QSize cellMin = mCellMin.expandedTo(QSize(0, 0));
  const QSize cellMax = mCellMax;

  const int totalSpacingX = spacingX * qMax(0, columns - 1);
  const int totalSpacingY = spacingY * qMax(0, rows - 1);

  const int rawCellWidth = columns > 0 ? (rect.width() - totalSpacingX) / columns : 0;
  const int rawCellHeight = rows > 0 ? (rect.height() - totalSpacingY) / rows : 0;

  const int cellWidth = qBound(cellMin.width(), rawCellWidth, cellMax.width());
  const int cellHeight = qBound(cellMin.height(), rawCellHeight, cellMax.height());

  if (cellWidth <= 0 || cellHeight <= 0)
    return;

  for (int i = 0; i < indexes.size(); ++i) {
    const int logicalIndex = indexes.at(i);
    const int row = i / columns;
    const int column = i % columns;

    QRect cell(rect.x() + column * (cellWidth + spacingX),
               rect.y() + row * (cellHeight + spacingY),
               cellWidth,
               cellHeight);

    if (QLayoutItem* item = mItems.at(logicalIndex))
      item->setGeometry(cell);
  }
}

int GridLayout::indexOf(const QWidget* widget) const {
  if (!widget)
    return -1;

  for (int i = 0; i < mItems.count(); ++i) {
    if (mItems.at(i)->widget() == widget)
      return i;
  }

  return -1;
}

int GridLayout::indexOf(const QLayoutItem* item) const {
  if (!item)
    return -1;

  for (int i = 0; i < mItems.count(); ++i) {
    if (mItems.at(i) == item)
      return i;
  }

  return -1;
}

QRect GridLayout::occupiedGeometry() const {
  QRect area;

  for (QLayoutItem* item : mItems) {
    if (item->widget() && item->widget()->isVisible())
      area = area.united(item->geometry());
  }

  if (area.isNull())
    area = geometry();

  return area;
}

void GridLayout::setStretch(StretchFlags stretch) {
  mStretch = stretch;
}

void GridLayout::setDirection(Direction direction) {
  if (mDirection == direction)
    return;

  mDirection = direction;
  invalidate();
}

void GridLayout::setItemsOrder(ItemsOrder order) {
  if (mOrder == order)
    return;

  mOrder = order;
  invalidate();
}

void GridLayout::setRowCount(int rows) {
  if (mRowCount == rows)
    return;

  mRowCount = rows;
  invalidate();
}

void GridLayout::setColumnCount(int columns) {
  if (mColumnCount == columns)
    return;

  mColumnCount = columns;
  invalidate();
}

void GridLayout::setCellMinimumSize(const QSize& size) {
  if (mCellMin == size)
    return;

  mCellMin = size;
  invalidate();
}

void GridLayout::setCellMaximumSize(const QSize& size) {
  if (mCellMax == size)
    return;

  mCellMax = size;
  invalidate();
}

void GridLayout::moveItem(int from, int to, bool) {
  if (from < 0 || to < 0 || from >= mItems.count() || to >= mItems.count() || from == to)
    return;

  mItems.move(from, to);
  invalidate();
}

bool GridLayout::animatedMoveInProgress() const {
  return false;
}

}  // namespace OneG4
