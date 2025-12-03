/* plugin-spacer/spacer.cpp
 * Spacer plugin implementation
 */

#include "spacer.h"
#include "spacerconfiguration.h"
#include <QApplication>

void SpacerWidget::setType(QString const& type) {
  if (type != mType) {
    mType = type;
    QEvent e{QEvent::ThemeChange};
    QApplication::sendEvent(this, &e);
  }
}

void SpacerWidget::setOrientation(QString const& orientation) {
  if (orientation != mOrientation) {
    mOrientation = orientation;
    QEvent e{QEvent::ThemeChange};
    QApplication::sendEvent(this, &e);
  }
}

/************************************************

 ************************************************/
Spacer::Spacer(const IOneG4PanelPluginStartupInfo& startupInfo)
    : QObject(), IOneG4PanelPlugin(startupInfo), mSize(8), mExpandable(false) {
  settingsChanged();
}

/************************************************

 ************************************************/
void Spacer::settingsChanged() {
  mSize = settings()->value(QStringLiteral("size"), 8).toInt();
  const bool old_expandable = mExpandable;
  mExpandable = settings()->value(QStringLiteral("expandable"), false).toBool();
  mSpacer.setType(settings()->value(QStringLiteral("spaceType"), SpacerConfiguration::msTypes[0]).toString());
  setSizes();
  if (old_expandable != mExpandable)
    pluginFlagsChanged();
}

/************************************************

 ************************************************/
QDialog* Spacer::configureDialog() {
  return new SpacerConfiguration(settings());
}

/************************************************

 ************************************************/
void Spacer::setSizes() {
  if (mExpandable) {
    mSpacer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mSpacer.setMinimumSize({1, 1});
    mSpacer.setMaximumSize({QWIDGETSIZE_MAX, QWIDGETSIZE_MAX});
    mSpacer.setOrientation(panel()->isHorizontal() ? QStringLiteral("horizontal") : QStringLiteral("vertical"));
  }
  else {
    if (panel()->isHorizontal()) {
      mSpacer.setOrientation(QStringLiteral("horizontal"));
      mSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
      mSpacer.setFixedWidth(mSize);
      mSpacer.setMinimumHeight(0);
      mSpacer.setMaximumHeight(QWIDGETSIZE_MAX);
    }
    else {
      mSpacer.setOrientation(QStringLiteral("vertical"));
      mSpacer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      mSpacer.setFixedHeight(mSize);
      mSpacer.setMinimumWidth(0);
      mSpacer.setMaximumWidth(QWIDGETSIZE_MAX);
    }
  }
}

/************************************************

 ************************************************/
void Spacer::realign() {
  setSizes();
}