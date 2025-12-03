#ifndef ONEG4_PLUGIN_INFO_H
#define ONEG4_PLUGIN_INFO_H

#include <QIcon>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariant>

namespace OneG4 {

class PluginInfo {
 public:
  PluginInfo() = default;
  explicit PluginInfo(const QString& filePath);

  bool isValid() const { return mIsValid; }
  QString filePath() const { return mFilePath; }
  QString id() const;
  QString name() const;
  QString comment() const;
  QIcon icon(const QIcon& fallback = QIcon()) const;
  QVariant value(const QString& key) const;
  QStringList serviceTypes() const;

  static QList<PluginInfo> search(const QStringList& directories,
                                  const QString& serviceType,
                                  const QString& pattern);

 private:
  QString mFilePath;
  QMap<QString, QVariant> mValues;
  QStringList mServiceTypes;
  bool mIsValid = false;
};

using PluginInfoList = QList<PluginInfo>;

}  // namespace OneG4

#endif  // ONEG4_PLUGIN_INFO_H
