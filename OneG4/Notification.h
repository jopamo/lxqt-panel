#ifndef ONEG4_NOTIFICATION_H
#define ONEG4_NOTIFICATION_H

#include <QObject>
#include <QString>

namespace OneG4 {

class Notification : public QObject {
  Q_OBJECT

 public:
  explicit Notification(const QString& iconName, QObject* parent = nullptr);

  void setSummary(const QString& summary);
  void update();

 private:
  QString mSummary;
  QString mIconName;
};

}  // namespace OneG4

#endif  // ONEG4_NOTIFICATION_H
