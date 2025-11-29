/* plugin-desktopswitch/desktopswitchbutton.cpp
 * Desktop switcher plugin implementation
 */

#include <QToolButton>
#include <QStyle>
#include <QVariant>
#include <QTimer>
#include <lxqt-globalkeys.h>

#include "desktopswitchbutton.h"

DesktopSwitchButton::DesktopSwitchButton(QWidget* parent, int index, LabelType labelType, const QString& title)
    : QToolButton(parent), mUrgencyHint(false) {
  update(index, labelType, title);

  setCheckable(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DesktopSwitchButton::update(int index, LabelType labelType, const QString& title) {
  switch (labelType) {
    case LABEL_TYPE_NAME:
      setText(title);
      break;

    // A blank space was used in NONE Label Type as it uses less space
    // for each desktop button at the panel
    case LABEL_TYPE_NONE:
      setText(QStringLiteral(" "));
      break;

    default:  // LABEL_TYPE_NUMBER
      setText(QString::number(index + 1));
  }

  if (!title.isEmpty()) {
    setToolTip(title);
  }
}

void DesktopSwitchButton::setUrgencyHint(WId id, bool urgent) {
  if (urgent)
    mUrgentWIds.insert(id);
  else
    mUrgentWIds.remove(id);

  // Add a small delay because, under some circumstances, urgencies may
  // be added and removed instantly, while repolishing can be costly.
  QTimer::singleShot(50, this, [this]() {
    if (mUrgencyHint != !mUrgentWIds.empty()) {
      mUrgencyHint = !mUrgentWIds.empty();
      setProperty("urgent", mUrgencyHint);
      style()->unpolish(this);
      style()->polish(this);
      QToolButton::update();
    }
  });
}