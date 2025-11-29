/* plugin-fancymenu/lxqtfancymenucategoriesmodel.cpp
 * Fancy menu plugin implementation
 */

#include "lxqtfancymenucategoriesmodel.h"
#include "lxqtfancymenuappmap.h"

LXQtFancyMenuCategoriesModel::LXQtFancyMenuCategoriesModel(QObject* parent)
    : QAbstractListModel(parent), mAppMap(nullptr) {}

int LXQtFancyMenuCategoriesModel::rowCount(const QModelIndex& p) const {
  if (!mAppMap || p.isValid())
    return 0;

  return mAppMap->getCategoriesCount();
}

QVariant LXQtFancyMenuCategoriesModel::data(const QModelIndex& idx, int role) const {
  if (!mAppMap || !idx.isValid() || idx.row() >= mAppMap->getCategoriesCount())
    return QVariant();

  const LXQtFancyMenuAppMap::Category& item = mAppMap->getCategoryAt(idx.row());

  switch (role) {
    case Qt::DisplayRole:
      return item.menuTitle;
    case Qt::EditRole:
      return item.menuName;
    case Qt::DecorationRole:
      return item.icon;
    case LXQtFancyMenuItemIsSeparatorRole:
      if (item.type == LXQtFancyMenuItemType::SeparatorItem)
        return 1;
    default:
      break;
  }

  return QVariant();
}

Qt::ItemFlags LXQtFancyMenuCategoriesModel::flags(const QModelIndex& idx) const {
  if (!mAppMap || !idx.isValid() || idx.row() >= mAppMap->getCategoriesCount())
    return Qt::NoItemFlags;

  const LXQtFancyMenuAppMap::Category& item = mAppMap->getCategoryAt(idx.row());
  if (item.type == LXQtFancyMenuItemType::SeparatorItem)
    return Qt::NoItemFlags;

  return QAbstractListModel::flags(idx);
}

void LXQtFancyMenuCategoriesModel::reloadAppMap(bool end) {
  if (!end)
    beginResetModel();
  else
    endResetModel();
}

LXQtFancyMenuAppMap* LXQtFancyMenuCategoriesModel::appMap() const {
  return mAppMap;
}

void LXQtFancyMenuCategoriesModel::setAppMap(LXQtFancyMenuAppMap* newAppMap) {
  mAppMap = newAppMap;
}