#ifndef ONEG4_APPLICATION_H
#define ONEG4_APPLICATION_H

#include <QApplication>

namespace OneG4 {

class Application : public QApplication {
  Q_OBJECT

 public:
  Application(int& argc, char** argv, bool enableHighDpiScaling = true);

 signals:
  void themeChanged();

 protected:
  bool event(QEvent* event) override;
};

}  // namespace OneG4

#endif  // ONEG4_APPLICATION_H
