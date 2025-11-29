/* plugin-desktopswitch/desktopswitchbutton.h
 * Desktop switcher plugin implementation
 */

#ifndef DESKTOPSWITCHBUTTON_H
#define DESKTOPSWITCHBUTTON_H

#include <QToolButton>
#include <QSet>

namespace GlobalKeyShortcut {
class Action;
}

class DesktopSwitchButton : public QToolButton {
  Q_OBJECT

 public:
  enum LabelType {  // Must match with combobox indexes
    LABEL_TYPE_INVALID = -1,
    LABEL_TYPE_NUMBER = 0,
    LABEL_TYPE_NAME = 1,
    LABEL_TYPE_NONE = 2
  };

  DesktopSwitchButton(QWidget* parent, int index, LabelType labelType, const QString& title = QString());
  void update(int index, LabelType labelType, const QString& title);

  void setUrgencyHint(WId, bool);

 private:
  // for urgency hint handling
  bool mUrgencyHint;
  QSet<WId> mUrgentWIds;
};

#endif