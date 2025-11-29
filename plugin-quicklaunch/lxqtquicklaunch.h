/* plugin-quicklaunch/lxqtquicklaunch.h
 * Quick launch plugin implementation
 */

#ifndef LXQTQUICKLAUNCH_H
#define LXQTQUICKLAUNCH_H

#include "../panel/lxqtpanel.h"
#include <QHash>
#include <QString>

class XdgDesktopFile;
class QuickLaunchAction;
class QDragEnterEvent;
class QuickLaunchButton;
class QSettings;
class QLabel;

namespace LXQt {
class GridLayout;
}

/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class LXQtQuickLaunch : public QFrame {
  Q_OBJECT

 public:
  LXQtQuickLaunch(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~LXQtQuickLaunch();

  int indexOfButton(QuickLaunchButton* button) const;
  int countOfButtons() const;

  void realign();

 private:
  LXQt::GridLayout* mLayout;
  ILXQtPanelPlugin* mPlugin;
  QLabel* mPlaceHolder;

  void dragEnterEvent(QDragEnterEvent* e);
  void dropEvent(QDropEvent* e);

  void saveSettings();
  void showPlaceHolder();

 private slots:
  void addButton(QuickLaunchAction* action);
  void switchButtons(QuickLaunchButton* button1, QuickLaunchButton* button2);
  void buttonDeleted();
  void buttonMoveLeft();
  void buttonMoveRight();
};

#endif