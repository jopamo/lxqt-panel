/* plugin-sensors/chip.h
 * System sensors plugin implementation
 */

#ifndef CHIP_H
#define CHIP_H

#include "feature.h"
#include "sensors.h"
#include <QList>
#include <QString>

/**
 * @brief Chip class is providing RAII-style for lm_sensors library
 */

class Chip {
 public:
  Chip(const sensors_chip_name*);
  const QString& getName() const;
  const QList<Feature>& getFeatures() const;

 private:
  // Do not try to change these chip names, as they point to internal structures of lm_sensors!
  const sensors_chip_name* mSensorsChipName;

  // "Printable" chip name
  QString mName;

  QList<Feature> mFeatures;
};

#endif  // CHIP_H