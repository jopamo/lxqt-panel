#include "ConfigDialog.h"

#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include "Settings.h"
#include "XdgIcon.h"

namespace OneG4 {

ConfigDialog::ConfigDialog(const QString& title, Settings*, QWidget* parent) : QDialog(parent) {
  setWindowTitle(title);
  setAttribute(Qt::WA_DeleteOnClose, true);

  mTabWidget = new QTabWidget(this);

  mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, this);
  connect(mButtonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
  connect(mButtonBox, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);
  connect(mButtonBox, &QDialogButtonBox::clicked, this, &ConfigDialog::handleButton);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mTabWidget);
  layout->addWidget(mButtonBox);
}

void ConfigDialog::addPage(QWidget* page, const QString& title, const QString& iconName) {
  if (!page)
    return;

  mPages.push_back({page, title, iconName});
  mTabWidget->addTab(page, XdgIcon::fromTheme(iconName), title);
}

void ConfigDialog::showPage(QWidget* page) {
  const int index = mTabWidget->indexOf(page);
  if (index >= 0)
    mTabWidget->setCurrentIndex(index);
}

void ConfigDialog::updateIconThemeSettings() {
  for (int i = 0; i < mPages.size(); ++i) {
    const auto& page = mPages.at(i);
    if (!page.iconName.isEmpty())
      mTabWidget->setTabIcon(i, XdgIcon::fromTheme(page.iconName));
  }
}

void ConfigDialog::handleButton(QAbstractButton* button) {
  if (mButtonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
    emit reset();
}

}  // namespace OneG4
