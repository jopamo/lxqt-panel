/* plugin-worldclock/oneg4worldclockconfigurationmanualformat.cpp
 * Configuration implementation for plugin-worldclock
 */

#include <QTimeZone>

#include "oneg4worldclockconfigurationmanualformat.h"

#include "ui_oneg4worldclockconfigurationmanualformat.h"

OneG4WorldClockConfigurationManualFormat::OneG4WorldClockConfigurationManualFormat(QWidget* parent)
    : QDialog(parent), ui(new Ui::OneG4WorldClockConfigurationManualFormat) {
  setObjectName(QStringLiteral("WorldClockConfigurationManualFormatWindow"));
  setWindowModality(Qt::WindowModal);
  ui->setupUi(this);

  connect(ui->manualFormatPTE, &QPlainTextEdit::textChanged, this,
          &OneG4WorldClockConfigurationManualFormat::manualFormatChanged);
}

OneG4WorldClockConfigurationManualFormat::~OneG4WorldClockConfigurationManualFormat() {
  delete ui;
}

void OneG4WorldClockConfigurationManualFormat::setManualFormat(const QString& text) {
  ui->manualFormatPTE->setPlainText(text);
}

QString OneG4WorldClockConfigurationManualFormat::manualFormat() const {
  return ui->manualFormatPTE->toPlainText();
}