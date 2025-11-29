/* plugin-volume/volumebutton.h
 * Volume control plugin implementation
 */

#ifndef VOLUMEBUTTON_H
#define VOLUMEBUTTON_H

#include <QToolButton>
#include <QTimer>

class VolumePopup;
class ILXQtPanelPlugin;

class VolumeButton : public QToolButton {
  Q_OBJECT
 public:
  VolumeButton(ILXQtPanelPlugin* plugin, QWidget* parent = nullptr);
  ~VolumeButton();

  void setMuteOnMiddleClick(bool state);
  void setMixerCommand(const QString& command);

  VolumePopup* volumePopup() const { return m_volumePopup; }

 public slots:
  void hideVolumeSlider();
  void showVolumeSlider();

 protected:
  void enterEvent(QEnterEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

 private slots:
  void toggleVolumeSlider();
  void handleMixerLaunch();
  void handleStockIconChanged(const QString& iconName);

 private:
  VolumePopup* m_volumePopup;
  ILXQtPanelPlugin* mPlugin;
  QTimer m_popupHideTimer;
  bool m_muteOnMiddleClick;
  QString m_mixerCommand;
  QStringList m_mixerParams;
};

#endif  // VOLUMEBUTTON_H