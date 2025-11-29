/* plugin-kbindicator/src/kbdkeeper.cpp
 * Keyboard indicator plugin implementation
 */

#include <QDebug>

#include <KX11Extras>
#include <KWindowInfo>
#include <netwm_def.h>

#include "kbdkeeper.h"

//--------------------------------------------------------------------------------------------------

KbdKeeper::KbdKeeper(const KbdLayout& layout, KeeperType type) : m_layout(layout), m_type(type) {
  m_layout.readKbdInfo(m_info);
}

KbdKeeper::~KbdKeeper() = default;

bool KbdKeeper::setup() {
  connect(&m_layout, &KbdLayout::keyboardChanged, this, &KbdKeeper::keyboardChanged);
  connect(&m_layout, &KbdLayout::layoutChanged, this, &KbdKeeper::layoutChanged);
  connect(&m_layout, &KbdLayout::checkState, this, &KbdKeeper::checkState);
  return true;
}

void KbdKeeper::keyboardChanged() {
  m_layout.readKbdInfo(m_info);
  emit changed();
}

void KbdKeeper::layoutChanged(uint group) {
  m_info.setCurrentGroup(group);
  emit changed();
}

void KbdKeeper::checkState() {}

void KbdKeeper::switchToNext() {
  uint index = m_info.currentGroup();
  if (index < m_info.size() - 1)
    ++index;
  else
    index = 0;

  switchToGroup(index);
}

void KbdKeeper::switchToGroup(uint group) {
  m_layout.lockGroup(group);
  emit changed();
}

//--------------------------------------------------------------------------------------------------

WinKbdKeeper::WinKbdKeeper(const KbdLayout& layout) : KbdKeeper(layout, KeeperType::Window) {}

WinKbdKeeper::~WinKbdKeeper() = default;

void WinKbdKeeper::layoutChanged(uint group) {
  WId win = KX11Extras::activeWindow();

  if (m_active == win) {
    m_mapping[win] = group;
    m_info.setCurrentGroup(group);
  }
  else {
    if (!m_mapping.contains(win))
      m_mapping.insert(win, 0);
    m_layout.lockGroup(m_mapping[win]);
    m_active = win;
    m_info.setCurrentGroup(m_mapping[win]);
  }
  emit changed();
}

void WinKbdKeeper::checkState() {
  WId win = KX11Extras::activeWindow();

  if (!m_mapping.contains(win))
    m_mapping.insert(win, 0);
  m_layout.lockGroup(m_mapping[win]);
  m_active = win;
  m_info.setCurrentGroup(m_mapping[win]);
  emit changed();
}

void WinKbdKeeper::switchToGroup(uint group) {
  WId win = KX11Extras::activeWindow();
  m_mapping[win] = group;
  m_layout.lockGroup(group);
  m_info.setCurrentGroup(group);
  emit changed();
}

//--------------------------------------------------------------------------------------------------

AppKbdKeeper::AppKbdKeeper(const KbdLayout& layout) : KbdKeeper(layout, KeeperType::Window) {}

AppKbdKeeper::~AppKbdKeeper() = default;

void AppKbdKeeper::layoutChanged(uint group) {
  KWindowInfo info = KWindowInfo(KX11Extras::activeWindow(), NET::Properties(), NET::WM2WindowClass);
  QString app = QString::fromUtf8(info.windowClassName());

  if (m_active == app) {
    m_mapping[app] = group;
    m_info.setCurrentGroup(group);
  }
  else {
    if (!m_mapping.contains(app))
      m_mapping.insert(app, 0);

    m_layout.lockGroup(m_mapping[app]);
    m_active = app;
    m_info.setCurrentGroup(m_mapping[app]);
  }
  emit changed();
}

void AppKbdKeeper::checkState() {
  KWindowInfo info = KWindowInfo(KX11Extras::activeWindow(), NET::Properties(), NET::WM2WindowClass);
  QString app = QString::fromUtf8(info.windowClassName());

  if (!m_mapping.contains(app))
    m_mapping.insert(app, 0);

  m_layout.lockGroup(m_mapping[app]);
  m_active = app;
  m_info.setCurrentGroup(m_mapping[app]);
  emit changed();
}

void AppKbdKeeper::switchToGroup(uint group) {
  KWindowInfo info = KWindowInfo(KX11Extras::activeWindow(), NET::Properties(), NET::WM2WindowClass);
  QString app = QString::fromUtf8(info.windowClassName());

  m_mapping[app] = group;
  m_layout.lockGroup(group);
  m_info.setCurrentGroup(group);
  emit changed();
}