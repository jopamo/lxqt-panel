#include "GridLayout.h"

#include <QStyle>
#include <QWidgetItem>
#include <QWidget>

namespace {

int effectiveSpacing(const QLayout* layout, Qt::Orientation orientation) {
  if (!layout)
    return 0;
  int spacing = layout->spacing();
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
  int w = 0;
  int h = 0;
  for (QLayoutItem* item : mItems) {
    const QSize hint = item->sizeHint();
    w = qMax(w, hint.width());
    h = qMax(h, hint.height());
  }
  const int rows = qMax(1, mRowCount > 0 ? mRowCount : ((mItems.count() + qMax(1, mColumnCount) - 1) / qMax(1, mColumnCount)));
  const int columns = qMax(1, mColumnCount > 0 ? mColumnCount : ((mItems.count() + rows - 1) / rows));
  const int spacingX = effectiveSpacing(this, Qt::Horizontal);
  const int spacingY = effectiveSpacing(this, Qt::Vertical);
  return QSize(columns * w + (columns - 1) * spacingX, rows * h + (rows - 1) * spacingY);
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

  const int cellWidth = qBound(cellMin.width(), (rect.width() - totalSpacingX) / columns, cellMax.width());
  const int cellHeight = qBound(cellMin.height(), (rect.height() - totalSpacingY) / rows, cellMax.height());

  for (int i = 0; i < indexes.size(); ++i) {
    const int logicalIndex = indexes.at(i);
    const int row = i / columns;
    const int column = i % columns;
    QRect cell(rect.x() + column * (cellWidth + spacingX), rect.y() + row * (cellHeight + spacingY), cellWidth,
               cellHeight);
    mItems.at(logicalIndex)->setGeometry(cell);
  }
}

int GridLayout::indexOf(const QWidget* widget) const {
  if (!widget)
    return -1;
  for (int i = 0; i < mItems.count(); ++i)
    if (mItems.at(i)->widget() == widget)
      return i;
  return -1;
}

int GridLayout::indexOf(const QLayoutItem* item) const {
  if (!item)
    return -1;
  for (int i = 0; i < mItems.count(); ++i)
    if (mItems.at(i) == item)
      return i;
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
  Q_UNUSED(mStretch);
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
  mCellMin = size;
  invalidate();
}

void GridLayout::setCellMaximumSize(const QSize& size) {
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
