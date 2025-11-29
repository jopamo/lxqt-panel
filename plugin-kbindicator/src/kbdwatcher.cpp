/* plugin-kbindicator/src/kbdwatcher.cpp
 * Keyboard indicator plugin implementation
 */

#include <QDebug>
#include "kbdwatcher.h"

KbdWatcher::KbdWatcher() {
  connect(&m_layout, &KbdLayout::modifierChanged, this, &KbdWatcher::modifierStateChanged);
  m_layout.init();
}

void KbdWatcher::setup() {
  emit modifierStateChanged(Controls::Caps, m_layout.isModifierLocked(Controls::Caps));
  emit modifierStateChanged(Controls::Num, m_layout.isModifierLocked(Controls::Num));
  emit modifierStateChanged(Controls::Scroll, m_layout.isModifierLocked(Controls::Scroll));

  if (!m_keeper || m_keeper->type() != Settings::instance().keeperType()) {
    createKeeper(Settings::instance().keeperType());
  }
  else {
    keeperChanged();
  }
}

void KbdWatcher::createKeeper(KeeperType type) {
  switch (type) {
    case KeeperType::Global:
      m_keeper.reset(new KbdKeeper(m_layout));
      break;
    case KeeperType::Window:
      m_keeper.reset(new WinKbdKeeper(m_layout));
      break;
    case KeeperType::Application:
      m_keeper.reset(new AppKbdKeeper(m_layout));
      break;
  }

  connect(m_keeper.get(), &KbdKeeper::changed, this, &KbdWatcher::keeperChanged);

  m_keeper->setup();
  keeperChanged();
}

void KbdWatcher::keeperChanged() {
  emit layoutChanged(m_keeper->sym(), m_keeper->name(), m_keeper->variant());
}

void KbdWatcher::controlClicked(Controls cnt) {
  switch (cnt) {
    case Controls::Layout:
      m_keeper->switchToNext();
      break;
    default:
      m_layout.lockModifier(cnt, !m_layout.isModifierLocked(cnt));
      break;
  }
}