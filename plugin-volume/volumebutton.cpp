/* plugin-volume/volumebutton.cpp
 * Volume control plugin implementation
 */

#include "volumebutton.h"

#include "volumepopup.h"
#include "audiodevice.h"

#include <QToolButton>
#include <QSlider>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QEnterEvent>
#include <QProcess>
#include <QToolTip>
#include <QTimer>
#include <QRect>
#include <QSizePolicy>

#include <XdgIcon>
#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"

VolumeButton::VolumeButton(ILXQtPanelPlugin* plugin, QWidget* parent)
    : QToolButton(parent), mPlugin(plugin), m_muteOnMiddleClick(true) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setAutoRaise(true);
  setMouseTracking(true);

  // initial icon for button, replaced after devices scan
  // if no sound device is found it remains so the button is not blank
  handleStockIconChanged(QStringLiteral("dialog-error"));

  m_volumePopup = new VolumePopup(this);

  m_popupHideTimer.setInterval(1000);
  connect(this, &VolumeButton::clicked, this, &VolumeButton::toggleVolumeSlider);
  connect(&m_popupHideTimer, &QTimer::timeout, this, &VolumeButton::hideVolumeSlider);

  connect(m_volumePopup, &VolumePopup::mouseEntered, &m_popupHideTimer, &QTimer::stop);
  connect(m_volumePopup, &VolumePopup::mouseLeft, this, [this] { m_popupHideTimer.start(); });

  connect(m_volumePopup, &VolumePopup::launchMixer, this, &VolumeButton::handleMixerLaunch);
  connect(m_volumePopup, &VolumePopup::stockIconChanged, this, &VolumeButton::handleStockIconChanged);
}

VolumeButton::~VolumeButton() = default;

void VolumeButton::setMuteOnMiddleClick(bool state) {
  m_muteOnMiddleClick = state;
}

void VolumeButton::setMixerCommand(const QString& command) {
  m_mixerParams = QProcess::splitCommand(command);
  m_mixerCommand = m_mixerParams.empty() ? QString{} : m_mixerParams.takeFirst();
}

void VolumeButton::enterEvent(QEnterEvent* event) {
  // show tooltip immediately on entering widget
  QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void VolumeButton::mouseMoveEvent(QMouseEvent* event) {
  QToolButton::mouseMoveEvent(event);

  // show tooltip immediately on moving the mouse
  if (!QToolTip::isVisible())  // prevent sliding of tooltip
    QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void VolumeButton::wheelEvent(QWheelEvent* event) {
  if (!m_volumePopup)
    return;

  m_volumePopup->handleWheelEvent(event);
}

void VolumeButton::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::MiddleButton && m_muteOnMiddleClick) {
    if (m_volumePopup && m_volumePopup->device()) {
      m_volumePopup->device()->toggleMute();
      return;
    }
  }

  QToolButton::mouseReleaseEvent(event);
}

void VolumeButton::toggleVolumeSlider() {
  if (!m_volumePopup)
    return;

  if (m_volumePopup->isVisible())
    hideVolumeSlider();
  else
    showVolumeSlider();
}

void VolumeButton::showVolumeSlider() {
  if (!m_volumePopup || m_volumePopup->isVisible())
    return;

  m_popupHideTimer.stop();
  m_volumePopup->updateGeometry();
  m_volumePopup->adjustSize();

  const QRect pos = mPlugin->calculatePopupWindowPos(m_volumePopup->size());
  mPlugin->willShowWindow(m_volumePopup);
  m_volumePopup->openAt(pos.topLeft(), Qt::TopLeftCorner);
  m_volumePopup->activateWindow();
}

void VolumeButton::hideVolumeSlider() {
  // qDebug() << "hideVolumeSlider";
  m_popupHideTimer.stop();

  if (m_volumePopup)
    m_volumePopup->hide();
}

void VolumeButton::handleMixerLaunch() {
  QProcess::startDetached(m_mixerCommand, m_mixerParams);
}

void VolumeButton::handleStockIconChanged(const QString& iconName) {
  setIcon(XdgIcon::fromTheme(iconName));
}