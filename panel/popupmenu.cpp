/* panel/popupmenu.cpp
 * Implementation file for popupmenu
 */

#include "popupmenu.h"
#include <QWidgetAction>
#include <QToolButton>
#include <QEvent>
#include <QKeyEvent>

static const char POPUPMENU_TITLE[] = "POPUP_MENU_TITLE_OBJECT_NAME";

/************************************************

 ************************************************/
QAction* PopupMenu::addTitle(const QIcon& icon, const QString& text) {
  QAction* buttonAction = new QAction(this);
  QFont font = buttonAction->font();
  font.setBold(true);
  buttonAction->setText(QString(text).replace(QLatin1String("&"), QLatin1String("&&")));
  buttonAction->setFont(font);
  buttonAction->setIcon(icon);

  QWidgetAction* action = new QWidgetAction(this);
  action->setObjectName(QLatin1String(POPUPMENU_TITLE));
  QToolButton* titleButton = new QToolButton(this);
  titleButton->installEventFilter(this);  // prevent clicks on the title of the menu
  titleButton->setDefaultAction(buttonAction);
  titleButton->setDown(true);  // prevent hover style changes in some styles
  titleButton->setCheckable(true);
  titleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  action->setDefaultWidget(titleButton);

  addAction(action);
  return action;
}

/************************************************

 ************************************************/
QAction* PopupMenu::addTitle(const QString& text) {
  return addTitle(QIcon(), text);
}

/************************************************

 ************************************************/
bool PopupMenu::eventFilter(QObject* object, QEvent* event) {
  Q_UNUSED(object);

  if (event->type() == QEvent::Paint || event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
    return false;
  }

  event->accept();
  return true;
}

/************************************************

 ************************************************/
void PopupMenu::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
    QMenu::keyPressEvent(e);

    QWidgetAction* action = qobject_cast<QWidgetAction*>(this->activeAction());
    QWidgetAction* firstAction = action;

    while (action && action->objectName() == QLatin1String(POPUPMENU_TITLE)) {
      this->keyPressEvent(e);
      action = qobject_cast<QWidgetAction*>(this->activeAction());

      if (firstAction == action)  // we looped and only found titles
      {
        this->setActiveAction(nullptr);
        break;
      }
    }

    return;
  }

  QMenu::keyPressEvent(e);
}