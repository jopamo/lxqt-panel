#include "Settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPointer>

namespace OneG4 {

namespace {

GlobalSettings* globalSettingsInstance() {
  static GlobalSettings* settings = new GlobalSettings(qApp);
  return settings;
}

}  // namespace

GlobalSettings::GlobalSettings(QObject* parent) : QObject(parent), mIconTheme(QIcon::themeName()) {}

void GlobalSettings::setIconTheme(const QString& themeName) {
  if (mIconTheme == themeName)
    return;
  mIconTheme = themeName;
  emit iconThemeChanged();
  emit settingsChanged();
}

void GlobalSettings::notifySettingsChanged() {
  emit settingsChanged();
}

Settings::Settings(const QString& group, QObject* parent) : QSettings(QStringLiteral("oneg4"), group, parent), mWatcher(nullptr) {
  setupWatcher();
}

Settings::Settings(const QString& fileName, QSettings::Format format, QObject* parent) : QSettings(fileName, format, parent), mWatcher(nullptr) {
  setupWatcher();
}

GlobalSettings* Settings::globalSettings() {
  return globalSettingsInstance();
}

void Settings::setupWatcher() {
  const QString cfg = fileName();
  if (cfg.isEmpty())
    return;

  mWatcher = new QFileSystemWatcher(this);
  mWatcher->addPath(cfg);
  connect(mWatcher, &QFileSystemWatcher::fileChanged, this, [this, cfg](const QString&) {
    if (QFile::exists(cfg) && !mWatcher->files().contains(cfg))
      mWatcher->addPath(cfg);
    emit settingsChangedFromExternal();
  });
}

SettingsCache::SettingsCache(QSettings* settings) {
  const QStringList keys = settings->allKeys();
  for (const QString& key : keys)
    mValues.insert(key, settings->value(key));
}

void SettingsCache::loadToSettings(QSettings* settings) {
  for (auto it = mValues.constBegin(); it != mValues.constEnd(); ++it)
    settings->setValue(it.key(), it.value());
}

}  // namespace OneG4
