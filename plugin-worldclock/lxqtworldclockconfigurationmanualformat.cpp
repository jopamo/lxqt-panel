/* plugin-worldclock/lxqtworldclockconfigurationmanualformat.cpp
 * Configuration implementation for plugin-worldclock
 */

#include <QTimeZone>

#include "lxqtworldclockconfigurationmanualformat.h"

#include "ui_lxqtworldclockconfigurationmanualformat.h"

LXQtWorldClockConfigurationManualFormat::LXQtWorldClockConfigurationManualFormat(QWidget* parent)
    : QDialog(parent), ui(new Ui::LXQtWorldClockConfigurationManualFormat) {
  setObjectName(QStringLiteral("WorldClockConfigurationManualFormatWindow"));
  setWindowModality(Qt::WindowModal);
  ui->setupUi(this);

  connect(ui->manualFormatPTE, &QPlainTextEdit::textChanged, this,
          &LXQtWorldClockConfigurationManualFormat::manualFormatChanged);
}

LXQtWorldClockConfigurationManualFormat::~LXQtWorldClockConfigurationManualFormat() {
  delete ui;
}

void LXQtWorldClockConfigurationManualFormat::setManualFormat(const QString& text) {
  ui->manualFormatPTE->setPlainText(text);
}

QString LXQtWorldClockConfigurationManualFormat::manualFormat() const {
  return ui->manualFormatPTE->toPlainText();
}