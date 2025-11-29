/* plugin-sensors/chip.cpp
 * System sensors plugin implementation
 */

#include "chip.h"
#include <QDebug>

Chip::Chip(const sensors_chip_name* sensorsChipName) : mSensorsChipName(sensorsChipName) {
  const int BUF_SIZE = 256;
  char buf[BUF_SIZE];
  if (sensors_snprintf_chip_name(buf, BUF_SIZE, mSensorsChipName) > 0) {
    mName = QString::fromLatin1(buf);
  }

  qDebug() << "Detected chip:" << mName;

  const sensors_feature* feature;
  int featureNr = 0;

  while ((feature = sensors_get_features(mSensorsChipName, &featureNr))) {
    mFeatures.push_back(Feature(mSensorsChipName, feature));
  }
}

const QString& Chip::getName() const {
  return mName;
}

const QList<Feature>& Chip::getFeatures() const {
  return mFeatures;
}