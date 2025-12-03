/* panel/windownotifier.h
 * Header file for windownotifier
 */

#if !defined(WINDOWNOTIFIER_H)
#define WINDOWNOTIFIER_H

#include <QObject>

class QWidget;

class WindowNotifier : public QObject {
  Q_OBJECT
 public:
  using QObject::QObject;

  void observeWindow(QWidget* w);
  inline bool isAnyWindowShown() const { return !mShownWindows.isEmpty(); }

  virtual bool eventFilter(QObject* watched, QEvent* event) override;
 signals:
  void lastHidden();
  void firstShown();

 private:
  QList<QWidget*> mShownWindows;  //!< known shown windows (sorted)
};

#endif