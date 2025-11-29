/* plugin-sensors/feature.cpp
 * System sensors plugin implementation
 */

#include "feature.h"
#include <QDebug>

Feature::Feature(const sensors_chip_name* sensorsChipName, const sensors_feature* sensorsFeature)
    : mSensorsChipName(sensorsChipName), mSensorsFeature(sensorsFeature) {
  char* featureLabel = nullptr;

  if ((featureLabel = sensors_get_label(mSensorsChipName, mSensorsFeature))) {
    mLabel = QString::fromUtf8(featureLabel);
    free(featureLabel);
  }

  qDebug() << "Detected feature:" << QString::fromLatin1(sensorsFeature->name) << "(" << mLabel << ")";
}

const QString& Feature::getLabel() const {
  return mLabel;
}

double Feature::getValue(sensors_subfeature_type subfeature_type) const {
  double result = 0;

  const sensors_subfeature* subfeature;

  // Find feature
  subfeature = sensors_get_subfeature(mSensorsChipName, mSensorsFeature, subfeature_type);

  if (subfeature) {
    sensors_get_value(mSensorsChipName, subfeature->number, &result);
  }

  return result;
}

sensors_feature_type Feature::getType() const {
  return mSensorsFeature->type;
}