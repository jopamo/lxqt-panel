#ifndef XDGICON_WRAPPER_H
#define XDGICON_WRAPPER_H

#include <QIcon>
#include <QString>

namespace XdgIcon {

QIcon fromTheme(const QString& name, const QIcon& fallback = QIcon());
QIcon defaultApplicationIcon();

}  // namespace XdgIcon

#endif  // XDGICON_WRAPPER_H
