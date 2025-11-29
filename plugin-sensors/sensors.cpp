/* plugin-sensors/sensors.cpp
 * System sensors plugin implementation
 */

#include "sensors.h"
#include <QDebug>

QList<Chip> Sensors::mDetectedChips = QList<Chip>();
int Sensors::mInstanceCounter = 0;
bool Sensors::mSensorsInitialized = false;

Sensors::Sensors() {
  // Increase instance counter
  ++mInstanceCounter;

  if (!mSensorsInitialized && sensors_init(nullptr) == 0) {
    // Sensors initialized
    mSensorsInitialized = true;

    sensors_chip_name const* chipName;
    int chipNr = 0;
    while ((chipName = sensors_get_detected_chips(nullptr, &chipNr)) != nullptr) {
      mDetectedChips.push_back(chipName);
    }

    qDebug() << "lm_sensors library initialized";
  }
}

Sensors::~Sensors() {
  // Decrease instance counter
  --mInstanceCounter;

  if (mInstanceCounter == 0 && mSensorsInitialized) {
    mDetectedChips.clear();
    mSensorsInitialized = false;
    sensors_cleanup();

    qDebug() << "lm_sensors library cleanup";
  }
}

const QList<Chip>& Sensors::getDetectedChips() const {
  return mDetectedChips;
}