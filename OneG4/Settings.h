#ifndef ONEG4_SETTINGS_H
#define ONEG4_SETTINGS_H

#include <QFileSystemWatcher>
#include <QIcon>
#include <QMap>
#include <QSettings>
#include <QObject>

namespace OneG4 {

class GlobalSettings : public QObject {
  Q_OBJECT

 public:
  explicit GlobalSettings(QObject* parent = nullptr);

  QString iconTheme() const { return mIconTheme; }

 public slots:
  void setIconTheme(const QString& themeName);
  void notifySettingsChanged();

 signals:
  void settingsChanged();
  void iconThemeChanged();

 private:
  QString mIconTheme;
};

class Settings : public QSettings {
  Q_OBJECT

 public:
  Settings(const QString& group, QObject* parent = nullptr);
  Settings(const QString& fileName, QSettings::Format format, QObject* parent = nullptr);

  static GlobalSettings* globalSettings();

 signals:
  void settingsChangedFromExternal();

 private:
  void setupWatcher();

  QFileSystemWatcher* mWatcher;
};

class SettingsCache {
 public:
  explicit SettingsCache(QSettings* settings);

  void loadToSettings(QSettings* settings);

 private:
  QMap<QString, QVariant> mValues;
};

}  // namespace OneG4

#endif  // ONEG4_SETTINGS_H
