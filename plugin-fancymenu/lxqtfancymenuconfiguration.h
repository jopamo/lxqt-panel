/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef LXQTFANCYMENUCONFIGURATION_H
#define LXQTFANCYMENUCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

class QAbstractButton;

namespace Ui {
class LXQtFancyMenuConfiguration;
}

class LXQtFancyMenuConfiguration : public LXQtPanelPluginConfigDialog {
  Q_OBJECT

 public:
  explicit LXQtFancyMenuConfiguration(PluginSettings* settings, QWidget* parent = nullptr);
  ~LXQtFancyMenuConfiguration();

 private:
  void fillButtonPositionComboBox();
  void fillCategoryPositionComboBox();

 private:
  Ui::LXQtFancyMenuConfiguration* ui;
  QString mDefaultShortcut;
  bool mLockSettingChanges;

 private slots:
  /*
    Saves settings in conf file.
  */
  void loadSettings();
  void textButtonChanged(const QString& value);
  void showTextChanged(bool value);
  void chooseIcon();
  void chooseMenuFile();
  void customFontChanged(bool value);
  void customFontSizeChanged(int value);
  void buttonRowPositionChanged(int idx);
  void categoryPositionChanged(int idx);
};

#endif  // LXQTFANCYMENUCONFIGURATION_H
