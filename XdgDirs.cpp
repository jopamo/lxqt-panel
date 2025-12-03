#include "XdgDirs.h"

#include <QDir>
#include <QStandardPaths>

namespace XdgDirs {

QString dataHome() {
  const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  return path.isEmpty() ? QDir::homePath() + QStringLiteral("/.local/share") : path;
}

}  // namespace XdgDirs
