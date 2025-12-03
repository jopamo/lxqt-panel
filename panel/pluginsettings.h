/* panel/pluginsettings.h
 * Base plugin class implementation
 */

#ifndef PLUGIN_SETTINGS_H
#define PLUGIN_SETTINGS_H

#include <QObject>
#include <QString>
#include <QVariant>
#include "oneg4panelglobals.h"

namespace OneG4 {
class Settings;
}
class PluginSettingsFactory;
class PluginSettingsPrivate;

/*!
 * \brief
 * Settings for particular plugin. This object/class can be used similarly as \sa QSettings.
 * Object cannot be constructed directly (it is the panel's responsibility to construct it for each plugin).
 *
 *
 * \note
 * We are relying here on so called "back linking" (calling a function defined in executable
 * back from an external library)...
 */
class ONEG4_PANEL_API PluginSettings : public QObject {
  Q_OBJECT

  // for instantiation
  friend class PluginSettingsFactory;

 public:
  ~PluginSettings();

  QString group() const;

  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
  void setValue(const QString& key, const QVariant& value);

  void remove(const QString& key);
  bool contains(const QString& key) const;

  QList<QMap<QString, QVariant> > readArray(const QString& prefix);
  void setArray(const QString& prefix, const QList<QMap<QString, QVariant> >& hashList);

  void clear();
  void sync();

  QStringList allKeys() const;
  QStringList childGroups() const;

  void beginGroup(const QString& subGroup);
  void endGroup();

  void loadFromCache();
  void storeToCache();

 signals:
  void settingsChanged();

 private:
  explicit PluginSettings(OneG4::Settings* settings, const QString& group, QObject* parent = nullptr);

 private:
  std::unique_ptr<PluginSettingsPrivate> d_ptr;
  Q_DECLARE_PRIVATE(PluginSettings)
};

#endif