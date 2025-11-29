/* plugin-kbindicator/src/kbdstateconfig.cpp
 * Keyboard indicator plugin implementation
 */

#include <LXQt/Globals>

#include <QDebug>
#include <QProcess>

#include "kbdstateconfig.h"
#include "ui_kbdstateconfig.h"
#include "settings.h"

KbdStateConfig::KbdStateConfig(QWidget* parent) : QDialog(parent), m_ui(new Ui::KbdStateConfig) {
  setAttribute(Qt::WA_DeleteOnClose);
  m_ui->setupUi(this);

  connect(m_ui->showCaps, &QCheckBox::clicked, this, &KbdStateConfig::save);
  connect(m_ui->showNum, &QCheckBox::clicked, this, &KbdStateConfig::save);
  connect(m_ui->showScroll, &QCheckBox::clicked, this, &KbdStateConfig::save);
  connect(m_ui->showLayout, &QGroupBox::clicked, this, &KbdStateConfig::save);
  connect(m_ui->layoutFlagPattern, &QLineEdit::textEdited, this, &KbdStateConfig::save);

  connect(m_ui->modes, &QButtonGroup::buttonClicked, this, [this] { KbdStateConfig::save(); });

  connect(m_ui->btns, &QDialogButtonBox::clicked, this, [this](QAbstractButton* btn) {
    if (m_ui->btns->buttonRole(btn) == QDialogButtonBox::ResetRole) {
      Settings::instance().restore();
      load();
    }
  });

  connect(m_ui->configureLayouts, &QPushButton::clicked, this, &KbdStateConfig::configureLayouts);

  load();
}

KbdStateConfig::~KbdStateConfig() {
  delete m_ui;
}

void KbdStateConfig::load() {
  Settings& sets = Settings::instance();

  m_ui->showCaps->setChecked(sets.showCapLock());
  m_ui->showNum->setChecked(sets.showNumLock());
  m_ui->showScroll->setChecked(sets.showScrollLock());
  m_ui->showLayout->setChecked(sets.showLayout());
  m_ui->layoutFlagPattern->setText(sets.layoutFlagPattern());

  switch (sets.keeperType()) {
    case KeeperType::Global:
      m_ui->switchGlobal->setChecked(true);
      break;
    case KeeperType::Window:
      m_ui->switchWindow->setChecked(true);
      break;
    case KeeperType::Application:
      m_ui->switchApplication->setChecked(true);
      break;
  }
}

void KbdStateConfig::save() {
  Settings& sets = Settings::instance();

  sets.setShowCapLock(m_ui->showCaps->isChecked());
  sets.setShowNumLock(m_ui->showNum->isChecked());
  sets.setShowScrollLock(m_ui->showScroll->isChecked());
  sets.setShowLayout(m_ui->showLayout->isChecked());
  sets.setLayoutFlagPattern(m_ui->layoutFlagPattern->text());

  if (m_ui->switchGlobal->isChecked())
    sets.setKeeperType(KeeperType::Global);
  if (m_ui->switchWindow->isChecked())
    sets.setKeeperType(KeeperType::Window);
  if (m_ui->switchApplication->isChecked())
    sets.setKeeperType(KeeperType::Application);
}

void KbdStateConfig::configureLayouts() {
  QProcess::startDetached(QL1S("lxqt-config-input"), QStringList() << QL1S("--show-page") << QL1S("Keyboard Layout"));
}