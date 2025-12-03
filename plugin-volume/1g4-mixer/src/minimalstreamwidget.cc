/* plugin-volume/1g4-mixer/src/minimalstreamwidget.cc
 * Implementation of minimalstreamwidget.cc
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "minimalstreamwidget.h"
#include <QGridLayout>
#include <QProgressBar>
#include <QDebug>

/*** MinimalStreamWidget ***/
MinimalStreamWidget::MinimalStreamWidget(QWidget* parent)
    : QWidget(parent),
      peakProgressBar(new QProgressBar(this)),
      lastPeak(0),
      peak(nullptr),
      updating(false),
      volumeMeterEnabled(false),
      volumeMeterVisible(true) {
  peakProgressBar->setTextVisible(false);
  peakProgressBar->hide();
}

MinimalStreamWidget::~MinimalStreamWidget() {
  if (peak != nullptr) {
    pa_stream_disconnect(peak);
    pa_stream_unref(peak);
    peak = nullptr;
  }
}

void MinimalStreamWidget::initPeakProgressBar(QGridLayout* channelsGrid) {
  channelsGrid->addWidget(peakProgressBar, channelsGrid->rowCount(), 0, 1, -1);
}

#define DECAY_STEP .04

void MinimalStreamWidget::updatePeak(double v) {
  if (lastPeak >= DECAY_STEP)
    if (v < lastPeak - DECAY_STEP)
      v = lastPeak - DECAY_STEP;

  lastPeak = v;

  if (v >= 0) {
    peakProgressBar->setEnabled(TRUE);
    int value = qRound(v * peakProgressBar->maximum());
    peakProgressBar->setValue(value);
  }
  else {
    peakProgressBar->setEnabled(FALSE);
    peakProgressBar->setValue(0);
  }

  enableVolumeMeter();
}

void MinimalStreamWidget::enableVolumeMeter() {
  if (volumeMeterEnabled)
    return;

  volumeMeterEnabled = true;
  if (volumeMeterVisible) {
    peakProgressBar->show();
  }
}

void MinimalStreamWidget::setVolumeMeterVisible(bool v) {
  volumeMeterVisible = v;
  if (v) {
    if (volumeMeterEnabled) {
      peakProgressBar->show();
    }
  }
  else {
    peakProgressBar->hide();
  }
}
