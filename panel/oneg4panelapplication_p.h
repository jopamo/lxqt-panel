/* panel/oneg4panelapplication_p.h
 * Main panel implementation, window management
 */

/*
 * OneG4 - a lightweight, Qt based, desktop toolset
 * Copyright (C) 2016  Luís Pereira <luis.artur.pereira@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef ONEG4PANELAPPLICATION_P_H
#define ONEG4PANELAPPLICATION_P_H

#include "oneg4panelapplication.h"

namespace OneG4 {
class Settings;
}

class IOneG4AbstractWMInterface;

class OneG4PanelApplicationPrivate {
  Q_DECLARE_PUBLIC(OneG4PanelApplication)
 public:
  OneG4PanelApplicationPrivate(OneG4PanelApplication* q);
  ~OneG4PanelApplicationPrivate() {};

  OneG4::Settings* mSettings;
  IOneG4AbstractWMInterface* mWMBackend;

  IOneG4Panel::Position computeNewPanelPosition(const OneG4Panel* p, const int screenNum);

  void loadBackend();

 private:
  OneG4PanelApplication* const q_ptr;
};

#endif  // ONEG4PANELAPPLICATION_P_H