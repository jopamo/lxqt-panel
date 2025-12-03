/* plugin-volume/wireplumberpolicy.h
 * Helper for writing WirePlumber SPA-JSON rules and restarting the daemon.
 */

#ifndef WIREPLUMPERPOLICY_H
#define WIREPLUMPERPOLICY_H

#include <QJsonDocument>
#include <QSet>
#include <QString>
#include <QStringList>

class WirePlumberPolicy {
 public:
  explicit WirePlumberPolicy(const QString& configDir = defaultConfigDir(),
                             const QString& fileName = defaultFileName());

  QString filePath() const;

  bool load();
  bool write() const;
  bool restart(QString* errorString = nullptr) const;

  void setDeviceDisabled(const QString& name, bool disabled);
  void setNodeDisabled(const QString& name, bool disabled);
  bool deviceDisabled(const QString& name) const;
  bool nodeDisabled(const QString& name) const;

  void setRestartCommand(const QStringList& command);

  const QSet<QString>& disabledDevices() const { return m_disabledDevices; }
  const QSet<QString>& disabledNodes() const { return m_disabledNodes; }

  static QString defaultConfigDir();
  static QString defaultFileName();

 private:
  QJsonDocument buildDocument() const;
  void ensureParentDir() const;

  QString m_configDir;
  QString m_fileName;
  QStringList m_restartCommand;
  QSet<QString> m_disabledDevices;
  QSet<QString> m_disabledNodes;
};

#endif  // WIREPLUMPERPOLICY_H
