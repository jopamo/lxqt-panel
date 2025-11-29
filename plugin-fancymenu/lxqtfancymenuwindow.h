/* plugin-fancymenu/lxqtfancymenuwindow.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUWINDOW_H
#define LXQTFANCYMENUWINDOW_H

#include <QWidget>
#include <QTimer>

#include "lxqtfancymenutypes.h"

class QLineEdit;
class QToolButton;
class QLabel;
class QListView;
class QModelIndex;

class QHBoxLayout;
class QVBoxLayout;

class XdgMenu;

class LXQtFancyMenuAppMap;
class LXQtFancyMenuAppModel;
class LXQtFancyMenuCategoriesModel;

class LXQtFancyMenuWindow : public QWidget {
  Q_OBJECT
 public:
  explicit LXQtFancyMenuWindow(QWidget* parent = nullptr);
  ~LXQtFancyMenuWindow();

  virtual QSize sizeHint() const override;
  virtual QSize minimumSizeHint() const override;

  bool rebuildMenu(const XdgMenu& menu);

  void setCurrentCategory(int cat);

  bool eventFilter(QObject* watched, QEvent* e) override;

  QStringList favorites() const;
  void setFavorites(const QStringList& newFavorites);

  void setFilterClear(bool newFilterClear);

  void setButtonPosition(LXQtFancyMenuButtonPosition pos);
  void setCategoryPosition(LXQtFancyMenuCategoryPosition pos);

  void updateButtonIconSize();

  void setSearchEditFocus();

  void setCustomFont(const QFont& f);

  void setAutoSelection(bool autoSel) {
    mAutoSel = autoSel;
    if (!mAutoSel)
      mAutoSelTimer.stop();
  }
  void setAutoSelectionDelay(int delay) { mAutoSelTimer.setInterval(delay); }

 signals:
  void aboutToShow();
  void aboutToHide();
  void favoritesChanged();

 public slots:
  void doSearch();
  void setSearchQuery(const QString& text);

 protected:
  void hideEvent(QHideEvent* e) override;
  void showEvent(QShowEvent* e) override;
  void keyPressEvent(QKeyEvent* e) override;
  void paintEvent(QPaintEvent* e) override;

 private slots:
  void activateCategory(const QModelIndex& idx);
  void activateAppAtIndex(const QModelIndex& idx);
  void activateCurrentApp();

  void runPowerDialog();
  void runSystemConfigDialog();
  void runAboutgDialog();

  void onAppViewCustomMenu(const QPoint& p);

  void autoSelect();

 private:
  void runCommandHelper(const QString& cmd);

  void addToFavorites(const QString& desktopFile);
  void removeFromFavorites(const QString& desktopFile);

 private:
  // Use 3:2 stretch factors so app view is slightly wider than category view
  static const int APP_VIEW_STRETCH = 3;
  static const int CAT_VIEW_STRETCH = 2;

  QVBoxLayout* mMainLayout;
  QHBoxLayout* mButtonsLayout;
  QHBoxLayout* mViewLayout;

  QToolButton* mSettingsButton;
  QToolButton* mPowerButton;
  QToolButton* mAboutButton;
  QLineEdit* mSearchEdit;
  QListView* mAppView;
  QListView* mCategoryView;

  QLabel* mFavoritesLabel;

  LXQtFancyMenuAppMap* mAppMap;
  LXQtFancyMenuAppModel* mAppModel;
  LXQtFancyMenuCategoriesModel* mCategoryModel;

  QTimer mSearchTimer;
  QTimer mAutoSelTimer;
  bool mAutoSel = false;
  bool mFilterClear = false;

  enum class FocusedItem { SearchEdit = 0, AppView, CategoryView };

  FocusedItem mFocusedItem;
};

#endif  // LXQTFANCYMENUWINDOW_H