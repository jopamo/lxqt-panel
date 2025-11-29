/* plugin-tray/snidbus.h
 * Header file for snidbus
 */

#pragma once

#include <QByteArray>
#include <QDBusArgument>
#include <QImage>
#include <QString>
#include <QList>

// Custom message type for DBus
struct KDbusImageStruct {
  KDbusImageStruct();
  KDbusImageStruct(const QImage& image);
  int width;
  int height;
  QByteArray data;
};

typedef QList<KDbusImageStruct> KDbusImageVector;

struct KDbusToolTipStruct {
  QString icon;
  KDbusImageVector image;
  QString title;
  QString subTitle;
};

const QDBusArgument& operator<<(QDBusArgument& argument, const KDbusImageStruct& icon);
const QDBusArgument& operator>>(const QDBusArgument& argument, KDbusImageStruct& icon);

Q_DECLARE_METATYPE(KDbusImageStruct)

const QDBusArgument& operator<<(QDBusArgument& argument, const KDbusImageVector& iconVector);
const QDBusArgument& operator>>(const QDBusArgument& argument, KDbusImageVector& iconVector);

Q_DECLARE_METATYPE(KDbusImageVector)

const QDBusArgument& operator<<(QDBusArgument& argument, const KDbusToolTipStruct& toolTip);
const QDBusArgument& operator>>(const QDBusArgument& argument, KDbusToolTipStruct& toolTip);

Q_DECLARE_METATYPE(KDbusToolTipStruct)