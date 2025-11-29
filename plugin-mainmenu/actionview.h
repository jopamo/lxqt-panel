/* plugin-mainmenu/actionview.h
 * Main menu plugin implementation
 */

#if !defined(ACTION_VIEW_H)
#define ACTION_VIEW_H

#include <QListView>
#include <QPoint>

class QStandardItemModel;

//==============================
#include <QSortFilterProxyModel>
class FilterProxyModel : public QSortFilterProxyModel {
  Q_OBJECT
 public:
  explicit FilterProxyModel(QObject* parent = nullptr);
  virtual ~FilterProxyModel();

  void setfilerString(const QString& str) {
    filterStr_ = str;
    invalidateFilter();
  }

 protected:
  bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

 private:
  QString filterStr_;
};
//==============================
class ActionView : public QListView {
  Q_OBJECT
 public:
  enum Role { ActionRole = Qt::UserRole, FilterRole = ActionRole + 1 };

 public:
  ActionView(QWidget* parent = nullptr);

  /*! \brief Remove all items from model
   */
  void clear();
  /*! \brief Add action proxy to the underlying model
   */
  void addAction(QAction* action);
  /*! \brief Check if action already exists in the view/model.
   *
   * \note The equality is evaluated just on text() & toolTip()
   */
  bool existsAction(QAction const* action) const;
  /*! \brief Fill the view with all actions from \param menu
   */
  void fillActions(QMenu* menu);
  /*! \brief Sets the filter for entries to be presented
   */
  void setFilter(QString const& filter);
  /*! \brief Set the maximum number of items/results to show
   */
  void setMaxItemsToShow(int max);
  /*! \brief Set the maximum width of item to show
   */
  void setMaxItemWidth(int max);

 public slots:
  /*! \brief Trigger action on currently active item
   */
  void activateCurrent();

 protected:
  virtual QSize viewportSizeHint() const override;
  virtual QSize minimumSizeHint() const override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

 signals:
  void requestShowHideMenu();

 private slots:
  void onActivated(QModelIndex const& index);
  void onActionDestroyed();

 private:
  void fillActionsRecursive(QMenu* menu);

 private:
  QStandardItemModel* mModel;
  QPoint mDragStartPosition;
  FilterProxyModel* mProxy;
  int mMaxItemsToShow;
};

#endif  // ACTION_VIEW_H