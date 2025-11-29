/* plugin-fancymenu/lxqtfancymenuappmodel.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUAPPMODEL_H
#define LXQTFANCYMENUAPPMODEL_H

#include <QAbstractListModel>

#include "lxqtfancymenutypes.h"

class LXQtFancyMenuAppMap;
struct LXQtFancyMenuAppItem;

class LXQtFancyMenuAppModel : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit LXQtFancyMenuAppModel(QObject* parent = nullptr);

  // Basic functionality:
  int rowCount(const QModelIndex& p = QModelIndex()) const override;

  QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const override;

  // Drag support
  Qt::ItemFlags flags(const QModelIndex& idx) const override;

  virtual QStringList mimeTypes() const override;
  virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
  virtual bool dropMimeData(const QMimeData* data_,
                            Qt::DropAction action,
                            int row,
                            int column,
                            const QModelIndex& p) override;
  virtual Qt::DropActions supportedDragActions() const override;

  void reloadAppMap(bool end);
  void setCurrentCategory(int category);
  void showSearchResults(const QList<const LXQtFancyMenuAppItem*>& matches);
  void endSearch();

  LXQtFancyMenuAppMap* appMap() const;
  void setAppMap(LXQtFancyMenuAppMap* newAppMap);

  const LXQtFancyMenuAppItem* getAppAt(int idx) const;
  LXQtFancyMenuItemType getItemTypeAt(int idx) const;

  bool isInSearch() const;

 signals:
  void favoritesChanged();

 private:
  LXQtFancyMenuAppMap* mAppMap;
  int mCurrentCategory;

  QList<const LXQtFancyMenuAppItem*> mSearchMatches;
  bool mInSearch;
};

#endif  // LXQTFANCYMENUAPPMODEL_H