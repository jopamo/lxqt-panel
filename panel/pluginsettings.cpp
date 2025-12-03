/* panel/pluginsettings.cpp
 * Base plugin class implementation
 */

#include "pluginsettings.h"
#include "pluginsettings_p.h"
#include <OneG4/Settings.h>
#include <memory>

class PluginSettingsPrivate {
 public:
  PluginSettingsPrivate(OneG4::Settings* settings, const QString& group) : mSettings(settings), mGroup(group) {
    mSettings->beginGroup(mGroup);
    mOldSettings = std::make_unique<OneG4::SettingsCache>(mSettings);
    mSettings->endGroup();
  }

  QString prefix() const;
  inline QString fullPrefix() const { return mGroup + QStringLiteral("/") + prefix(); }

  OneG4::Settings* mSettings;
  std::unique_ptr<OneG4::SettingsCache> mOldSettings;
  QString mGroup;
  QStringList mSubGroups;
};

QString PluginSettingsPrivate::prefix() const {
  if (!mSubGroups.empty())
    return mSubGroups.join(QLatin1Char('/'));
  return QString();
}

PluginSettings::PluginSettings(OneG4::Settings* settings, const QString& group, QObject* parent)
    : QObject(parent), d_ptr(new PluginSettingsPrivate{settings, group}) {
  Q_D(PluginSettings);
  connect(d->mSettings, &OneG4::Settings::settingsChangedFromExternal, this, &PluginSettings::settingsChanged);
}

QString PluginSettings::group() const {
  Q_D(const PluginSettings);
  return d->mGroup;
}

PluginSettings::~PluginSettings() = default;

QVariant PluginSettings::value(const QString& key, const QVariant& defaultValue) const {
  Q_D(const PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  QVariant value = d->mSettings->value(key, defaultValue);
  d->mSettings->endGroup();
  return value;
}

void PluginSettings::setValue(const QString& key, const QVariant& value) {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  d->mSettings->setValue(key, value);
  d->mSettings->endGroup();
  emit settingsChanged();
}

void PluginSettings::remove(const QString& key) {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  d->mSettings->remove(key);
  d->mSettings->endGroup();
  emit settingsChanged();
}

bool PluginSettings::contains(const QString& key) const {
  Q_D(const PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  bool ret = d->mSettings->contains(key);
  d->mSettings->endGroup();
  return ret;
}

QList<QMap<QString, QVariant> > PluginSettings::readArray(const QString& prefix) {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  QList<QMap<QString, QVariant> > array;
  int size = d->mSettings->beginReadArray(prefix);
  for (int i = 0; i < size; ++i) {
    d->mSettings->setArrayIndex(i);
    QMap<QString, QVariant> hash;
    const auto keys = d->mSettings->childKeys();
    for (const QString& key : keys)
      hash[key] = d->mSettings->value(key);
    array << hash;
  }
  d->mSettings->endArray();
  d->mSettings->endGroup();
  return array;
}

void PluginSettings::setArray(const QString& prefix, const QList<QMap<QString, QVariant> >& hashList) {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  d->mSettings->beginWriteArray(prefix);
  int size = hashList.size();
  for (int i = 0; i < size; ++i) {
    d->mSettings->setArrayIndex(i);
    QMapIterator<QString, QVariant> it(hashList.at(i));
    while (it.hasNext()) {
      it.next();
      d->mSettings->setValue(it.key(), it.value());
    }
  }
  d->mSettings->endArray();
  d->mSettings->endGroup();
  emit settingsChanged();
}

void PluginSettings::clear() {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->mGroup);
  d->mSettings->clear();
  d->mSettings->endGroup();
  emit settingsChanged();
}

void PluginSettings::sync() {
  Q_D(PluginSettings);
  d->mSettings->sync();
  storeToCache();
  emit settingsChanged();
}

QStringList PluginSettings::allKeys() const {
  Q_D(const PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  QStringList keys = d->mSettings->allKeys();
  d->mSettings->endGroup();
  return keys;
}

QStringList PluginSettings::childGroups() const {
  Q_D(const PluginSettings);
  d->mSettings->beginGroup(d->fullPrefix());
  QStringList groups = d->mSettings->childGroups();
  d->mSettings->endGroup();
  return groups;
}

void PluginSettings::beginGroup(const QString& subGroup) {
  Q_D(PluginSettings);
  d->mSubGroups.append(subGroup);
}

void PluginSettings::endGroup() {
  Q_D(PluginSettings);
  if (!d->mSubGroups.empty())
    d->mSubGroups.removeLast();
}

void PluginSettings::loadFromCache() {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->mGroup);
  d->mSettings->remove(QString{});
  d->mOldSettings->loadToSettings(d->mSettings);
  d->mSettings->endGroup();
  emit settingsChanged();
}

void PluginSettings::storeToCache() {
  Q_D(PluginSettings);
  d->mSettings->beginGroup(d->mGroup);
  d->mOldSettings = std::make_unique<OneG4::SettingsCache>(d->mSettings);
  d->mSettings->endGroup();
}

PluginSettings* PluginSettingsFactory::create(OneG4::Settings* settings,
                                              const QString& group,
                                              QObject* parent /* = nullptr*/) {
  return new PluginSettings{settings, group, parent};
}
