#include "Application.h"

#include <QEvent>

namespace OneG4 {

Application::Application(int& argc, char** argv, bool)
    : QApplication(argc, argv) {}

bool Application::event(QEvent* event) {
  if (event && (event->type() == QEvent::ApplicationPaletteChange ||
                event->type() == QEvent::ApplicationFontChange)) {
    emit themeChanged();
  }
  return QApplication::event(event);
}

}  // namespace OneG4
