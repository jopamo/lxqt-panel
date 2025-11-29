/* plugin-backlight/sliderdialog.h
 * Header file for sliderdialog
 */

#ifndef SLIDERDIALOG_H
#define SLIDERDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QToolButton>
#include <LXQt/lxqtbacklight.h>

class SliderDialog : public QDialog {
  Q_OBJECT

 public:
  SliderDialog(QWidget* parent);
  void updateBacklight();

 public Q_SLOTS:
  void downButtonClicked(bool);
  void upButtonClicked(bool);

 Q_SIGNALS:
  void dialogClosed();

 protected:
  bool event(QEvent* event) override;

 private:
  QSlider* m_slider;
  QToolButton *m_upButton, *m_downButton;
  LXQt::Backlight* m_backlight;

 private Q_SLOTS:
  void sliderValueChanged(int value);
};

#endif  // SLIDERDIALOG_H