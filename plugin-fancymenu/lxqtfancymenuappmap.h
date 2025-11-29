/* plugin-fancymenu/lxqtfancymenuappmap.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUAPPMAP_H
#define LXQTFANCYMENUAPPMAP_H

#include <QMap>
#include <QList>
#include <QStringList>
#include <QIcon>

#include <XdgDesktopFile>

#include "lxqtfancymenutypes.h"

class XdgMenu;
class QDomElement;

struct LXQtFancyMenuAppItem {
  QString desktopFile;
  QString title;
  QString comment;
  QStringList keywords;
  QStringList exec;
  QIcon icon;
  XdgDesktopFile desktopFileCache;
};

class LXQtFancyMenuAppMap {
 public:
  enum SpecialCategory { FavoritesCategory = 0, AllAppsCategory = 1 };

  typedef LXQtFancyMenuAppItem AppItem;

  struct Category {
    QString menuName;
    QString menuTitle;
    QIcon icon;

    struct Item {
      AppItem* appItem = nullptr;
      LXQtFancyMenuItemType type = LXQtFancyMenuItemType::AppItem;
    };

    QList<Item> apps;
    LXQtFancyMenuItemType type;
  };

  LXQtFancyMenuAppMap();
  ~LXQtFancyMenuAppMap();

  void clear();
  void clearFavorites();
  bool rebuildModel(const XdgMenu& menu);

  void setFavorites(const QStringList& favorites);
  QStringList getFavorites() const;

  int getFavoriteIndex(const QString& desktopFile) const;

  inline int getFavoriteCount() const { return mCategories[0].apps.count(); }

  inline bool isFavorite(const QString& desktopFile) const { return getFavoriteIndex(desktopFile) != -1; }

  void addToFavorites(const QString& desktopFile);
  void removeFromFavorites(const QString& desktopFile);
  void moveFavoriteItem(int oldPos, int newPos);

  inline int getCategoriesCount() const { return mCategories.size(); }
  inline const Category& getCategoryAt(int index) { return mCategories.at(index); }

  inline int getTotalAppCount() const { return mAppSortedByName.size(); }

  AppItem* getAppAt(int index);

  QList<const AppItem*> getMatchingApps(const QString& query) const;

 private:
  void parseMenu(const QDomElement& menu, const QString& topLevelCategory);
  void parseAppLink(const QDomElement& app, const QString& topLevelCategory);
  void parseSeparator(const QDomElement& sep, const QString& topLevelCategory);

  AppItem* loadAppItem(const QString& desktopFile);

 private:
  QMap<QString, AppItem*> mAppSortedByDesktopFile;
  QList<AppItem*> mAppSortedByName;
  QList<Category> mCategories;

  // Cache sort by name map access
  QList<AppItem*>::const_iterator mCachedIterator;
  int mCachedIndex;
};

#endif  // LXQTFANCYMENUAPPMAP_H