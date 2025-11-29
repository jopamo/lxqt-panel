/* plugin-statusnotifier/statusnotifierwidget.h
 * Header file for statusnotifierwidget
 */

#pragma once

#include <QTimer>

#include <LXQt/GridLayout>

#include "statusnotifierbutton.h"

class StatusNotifierProxy;

class StatusNotifierWidget : public QWidget {
  Q_OBJECT

 public:
  StatusNotifierWidget(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~StatusNotifierWidget() = default;

  void settingsChanged();
  QStringList itemTitles() const;

 signals:

 public slots:
  void itemAdded(QString serviceAndPath);
  void itemRemoved(const QString& serviceAndPath);

  void realign();

 protected:
  void leaveEvent(QEvent* event) override;
  void enterEvent(QEnterEvent* event) override;

 private:
  ILXQtPanelPlugin* mPlugin;

  QTimer mHideTimer;

  QHash<QString, StatusNotifierButton*> mServices;

  QStringList mItemTitles;
  QStringList mAutoHideList;
  QStringList mHideList;
  QToolButton* mShowBtn;
  int mAttentionPeriod;
  bool mForceVisible;
};