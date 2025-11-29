/* plugin-sysstat/lxqtsysstatutils.h
 * Header file for lxqtsysstatutils
 */

#ifndef LXQTSYSSTATUTILS_HPP
#define LXQTSYSSTATUTILS_HPP

#include <QString>

namespace PluginSysStat {

QString netSpeedToString(int value);
int netSpeedFromString(QStringView value);

}  // namespace PluginSysStat

#endif  // LXQTSYSSTATUTILS_HPP