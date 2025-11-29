/* plugin-customcommand/custombutton.h
 * Header file for custombutton
 */

#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QToolButton>

#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"

class CustomButton : public QToolButton {
  Q_OBJECT

 public:
  CustomButton(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~CustomButton();

 public slots:
  void setAutoRotation(bool value);
  void setMaxWidth(int maxWidth);
  void updateWidth();

 protected:
  void wheelEvent(QWheelEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private slots:
  void setOrigin(Qt::Corner newOrigin);

 private:
  ILXQtPanelPlugin* mPlugin;
  ILXQtPanel* mPanel;
  Qt::Corner mOrigin;
  int mMaxWidth;

 signals:
  void wheelScrolled(int);
};

#endif  // CUSTOMBUTTON_H