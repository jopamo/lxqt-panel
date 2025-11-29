/* plugin-sensors/feature.h
 * System sensors plugin implementation
 */

#ifndef FEATURE_H
#define FEATURE_H

#include <QList>
#include <QString>
#include <sensors/sensors.h>

/**
 * @brief Feature class is providing RAII-style for lm_sensors library
 */

class Feature {
 public:
  Feature(const sensors_chip_name*, const sensors_feature*);
  const QString& getName() const;
  const QString& getLabel() const;
  double getValue(sensors_subfeature_type) const;
  sensors_feature_type getType() const;

 private:
  // Do not try to change these chip names, as they point to internal structures of lm_sensors!
  const sensors_chip_name* mSensorsChipName;

  const sensors_feature* mSensorsFeature;

  // "Printable" feature label
  QString mLabel;

  QList<const sensors_subfeature*> mSubFeatures;
};

#endif  // CHIP_H