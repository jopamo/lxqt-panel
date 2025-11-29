/* plugin-sysstat/lxqtsysstatutils.cpp
 * Implementation file for lxqtsysstatutils
 */

#include <QRegularExpression>
#include <qmath.h>

#include "lxqtsysstatutils.h"

namespace PluginSysStat {

QString netSpeedToString(int value) {
  QString prefix;
  static const char prefixes[] = "kMG";
  if (value / 10)
    prefix = QLatin1Char(prefixes[value / 10 - 1]);

  return QStringLiteral("%1 %2B/s").arg(1 << (value % 10)).arg(prefix);
}

int netSpeedFromString(QStringView value) {
  static const QRegularExpression re(QStringLiteral("^(\\d+) ([kMG])B/s$"));
  QRegularExpressionMatch match = re.matchView(value);
  if (match.hasMatch()) {
    int shift = 0;
    switch (match.capturedView(2).at(0).toLatin1()) {
      case 'k':
        shift = 10;
        break;

      case 'M':
        shift = 20;
        break;

      case 'G':
        shift = 30;
        break;
    }

    return qCeil(qLn(match.capturedView(1).toInt()) / qLn(2.)) + shift;
  }

  return 0;
}

}  // namespace PluginSysStat