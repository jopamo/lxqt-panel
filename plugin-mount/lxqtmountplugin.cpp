/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QGuiApplication>

#include "lxqtmountplugin.h"
#include "configuration.h"

#include <LXQt/Notification>

#include <Solid/DeviceNotifier>


LXQtMountPlugin::LXQtMountPlugin(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mPopup(nullptr),
    mDeviceAction(nullptr),
    mEjectAction(nullptr)
{
    mButton = new Button;
    mPopup = new Popup(this);

    connect(mButton, &QToolButton::clicked, mPopup, &Popup::showHide);
    connect(mPopup, &Popup::visibilityChanged, mButton, &QToolButton::setDown);
    // Note: postpone creation of the mDeviceAction to not fire it in startup time
    QTimer::singleShot(0, this, &LXQtMountPlugin::settingsChanged);
}

LXQtMountPlugin::~LXQtMountPlugin()
{
    delete mButton;
    delete mPopup;
}



QDialog *LXQtMountPlugin::configureDialog()
{
    if (mPopup)
        mPopup->hide();

    Configuration *configWindow = new Configuration(settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    return configWindow;
}

void LXQtMountPlugin::realign()
{
    //nothing to do
}

void LXQtMountPlugin::settingsChanged()
{
    QString s = settings()->value(QLatin1String(CFG_KEY_ACTION)).toString();
    DeviceAction::ActionId devActionId = DeviceAction::stringToActionId(s, DeviceAction::ActionMenu);


    if (mDeviceAction == nullptr || mDeviceAction->Type() != devActionId)
    {
        delete mDeviceAction;
        mDeviceAction = DeviceAction::create(devActionId, this, this);

        connect(mPopup, &Popup::deviceAdded, mDeviceAction, &DeviceAction::onDeviceAdded);
        connect(mPopup, &Popup::deviceRemoved, mDeviceAction, &DeviceAction::onDeviceRemoved);
    }

    s = settings()->value(QLatin1String(CFG_EJECT_ACTION)).toString();
    EjectAction::ActionId ejActionId = EjectAction::stringToActionId(s, EjectAction::ActionNothing);

    if (mEjectAction == nullptr || mEjectAction->Type() != ejActionId)
    {
         delete mEjectAction;
         mEjectAction = EjectAction::create(ejActionId, this, this);
    }
}
