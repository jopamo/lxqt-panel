/* plugin-kbindicator/src/kbdstateconfig.h
 * Keyboard indicator plugin implementation
 */

#ifndef _KBDSTATECONFIG_H_
#define _KBDSTATECONFIG_H_

#include <QDialog>

namespace Ui {
class KbdStateConfig;
}

class KbdStateConfig : public QDialog {
  Q_OBJECT

 public:
  explicit KbdStateConfig(QWidget* parent = nullptr);
  ~KbdStateConfig();

 private:
  void save();
  void load();

  void configureLayouts();

 private:
  Ui::KbdStateConfig* m_ui;
};

#endif