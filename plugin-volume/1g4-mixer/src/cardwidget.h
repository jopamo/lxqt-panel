/* plugin-volume/1g4-mixer/src/cardwidget.h
 * Implementation of cardwidget.h
 */

#ifndef cardwidget_h
#define cardwidget_h

#include "1g4-mixer.h"
#include "ui_cardwidget.h"
#include <QWidget>

class PortInfo {
 public:
  QByteArray name;
  QByteArray description;
  uint32_t priority;
  int available;
  int direction;
  int64_t latency_offset;
  std::vector<QByteArray> profiles;
};

class CardWidget : public QWidget, public Ui::CardWidget {
  Q_OBJECT
 public:
  CardWidget(QWidget* parent = nullptr);

  QByteArray name;
  uint32_t index;
  bool updating;

  std::vector<std::pair<QByteArray, QByteArray> > profiles;
  std::map<QByteArray, PortInfo> ports;
  QByteArray activeProfile;
  QByteArray noInOutProfile;
  QByteArray lastActiveProfile;
  bool hasSinks;
  bool hasSources;

  void prepareMenu();

 protected:
  void changeProfile(const QByteArray& name);
  void onProfileChange(int active);
  void onProfileCheck(bool on);
};

#endif
