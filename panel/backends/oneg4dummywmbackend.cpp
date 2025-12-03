/* panel/backends/oneg4dummywmbackend.cpp
 * Window manager backend interfaces
 */

#include "oneg4dummywmbackend.h"

#include <QIcon>

OneG4DummyWMBackend::OneG4DummyWMBackend(QObject* parent) : IOneG4AbstractWMInterface(parent) {}

/************************************************
 *   Windows function
 ************************************************/
bool OneG4DummyWMBackend::supportsAction(WId, OneG4TaskBarBackendAction) const {
  return false;
}

bool OneG4DummyWMBackend::reloadWindows() {
  return false;
}

QVector<WId> OneG4DummyWMBackend::getCurrentWindows() const {
  return {};
}

QString OneG4DummyWMBackend::getWindowTitle(WId) const {
  return QString();
}

bool OneG4DummyWMBackend::applicationDemandsAttention(WId) const {
  return false;
}

QIcon OneG4DummyWMBackend::getApplicationIcon(WId, int) const {
  return QIcon();
}

QString OneG4DummyWMBackend::getWindowClass(WId) const {
  return QString();
}

OneG4TaskBarWindowLayer OneG4DummyWMBackend::getWindowLayer(WId) const {
  return OneG4TaskBarWindowLayer::Normal;
}

bool OneG4DummyWMBackend::setWindowLayer(WId, OneG4TaskBarWindowLayer) {
  return false;
}

OneG4TaskBarWindowState OneG4DummyWMBackend::getWindowState(WId) const {
  return OneG4TaskBarWindowState::Normal;
}

bool OneG4DummyWMBackend::setWindowState(WId, OneG4TaskBarWindowState, bool) {
  return false;
}

bool OneG4DummyWMBackend::isWindowActive(WId) const {
  return false;
}

bool OneG4DummyWMBackend::raiseWindow(WId, bool) {
  return false;
}

bool OneG4DummyWMBackend::closeWindow(WId) {
  return false;
}

WId OneG4DummyWMBackend::getActiveWindow() const {
  return 0;
}

/************************************************
 *   Workspaces
 ************************************************/
int OneG4DummyWMBackend::getWorkspacesCount(QScreen*) const {
  return 1;  // Fake 1 workspace
}

QString OneG4DummyWMBackend::getWorkspaceName(int, QString) const {
  return QString();
}

int OneG4DummyWMBackend::getCurrentWorkspace(QScreen*) const {
  return 0;
}

bool OneG4DummyWMBackend::setCurrentWorkspace(int, QScreen*) {
  return false;
}

int OneG4DummyWMBackend::getWindowWorkspace(WId) const {
  return 0;
}

bool OneG4DummyWMBackend::setWindowOnWorkspace(WId, int) {
  return false;
}

void OneG4DummyWMBackend::moveApplicationToPrevNextMonitor(WId, bool, bool) {
  // No-op
}

int OneG4DummyWMBackend::onAllWorkspacesEnum() const {
  return 0;
}

bool OneG4DummyWMBackend::isWindowOnScreen(QScreen*, WId) const {
  return false;
}

bool OneG4DummyWMBackend::setDesktopLayout(Qt::Orientation, int, int, bool) {
  return false;
}

/************************************************
 *   X11 Specific
 ************************************************/
void OneG4DummyWMBackend::moveApplication(WId) {
  // No-op
}

void OneG4DummyWMBackend::resizeApplication(WId) {
  // No-op
}

void OneG4DummyWMBackend::refreshIconGeometry(WId, QRect const&) {
  // No-op
}

bool OneG4DummyWMBackend::isAreaOverlapped(const QRect&) const {
  return false;
}

bool OneG4DummyWMBackend::isShowingDesktop() const {
  return false;
}

bool OneG4DummyWMBackend::showDesktop(bool) {
  return false;
}