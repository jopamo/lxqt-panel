/* plugin-sensors/sensors.h
 * System sensors plugin implementation
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "chip.h"
#include <QList>
#include <sensors/sensors.h>

class Chip;

/**
 * @brief Sensors class is providing RAII-style for lm_sensors library
 */

class Sensors {
 public:
  Sensors();
  ~Sensors();
  const QList<Chip>& getDetectedChips() const;

 private:
  static QList<Chip> mDetectedChips;

  /**
   * lm_sensors library can be initialized only once so this will tell us when to init
   * and when to clean up.
   */
  static int mInstanceCounter;
  static bool mSensorsInitialized;
};

#endif  // SENSORS_H