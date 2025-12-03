/* plugin-worldclock/worldclockmanualformatconfig.cpp
 * Configuration implementation for plugin-worldclock
 */

#include <QTimeZone>

#include "worldclockmanualformatconfig.h"

#include "ui_worldclockmanualformatconfig.h"

WorldClockManualFormatConfig::WorldClockManualFormatConfig(QWidget* parent)
    : QDialog(parent), ui(new Ui::WorldClockManualFormatConfig) {
  setObjectName(QStringLiteral("WorldClockManualFormatConfigWindow"));
  setWindowModality(Qt::WindowModal);
  ui->setupUi(this);

  connect(ui->manualFormatPTE, &QPlainTextEdit::textChanged, this, &WorldClockManualFormatConfig::manualFormatChanged);
}

WorldClockManualFormatConfig::~WorldClockManualFormatConfig() {
  delete ui;
}

void WorldClockManualFormatConfig::setManualFormat(const QString& text) {
  ui->manualFormatPTE->setPlainText(text);
}

QString WorldClockManualFormatConfig::manualFormat() const {
  return ui->manualFormatPTE->toPlainText();
}
