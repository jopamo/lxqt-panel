# 🖥️ 1g4-panel

> **a sleek, modular taskbar.

---

## ✨ Overview

The **1g4-panel** is built around a flexible plugin architecture. While the source code organizes these as "plugins" (e.g., `plugin-mainmenu`), the configuration interface refers to them as **"Widgets"**.

### 🧩 Available Plugins (Widgets)

Here are some of the key plugins you can add to your panel:

#### 🛠️ Custom Command (`plugin-customcommand`)
Execute custom scripts or commands and display their output directly on the panel. Supports text or image output, and interactive click/scroll actions.

![Custom command plugin settings](customcommand.png)

#### 🌍 World Clock (`plugin-worldclock`)
Stay on time with a comprehensive clock and calendar.
- 🕒 Displays local time and date
- 🌐 Supports multiple time zones
- 📅 Integrated calendar view

#### 🚀 Quick Launch (`plugin-taskbar`)
Launch your favorite apps instantly!
- Drag & drop applications from the main menu to add them.
- Organize your most-used tools for easy access.

#### 🔔 Status Notifier (`plugin-statusnotifier`)
Manage your background applications and notifications.
- Implements the modern [StatusNotifierItem (SNI)](https://www.freedesktop.org/wiki/Specifications/StatusNotifierItem) specification, an area where arbitrary applications can place informational icons.

---

## 📦 Installation

### 🔧 Compiling from Source
Please refer to the generic compilation instructions for **1g4** components to build `1g4-panel` from source.

---

## ⚙️ Configuration & Usage

### 🚀 Launching
`1g4-panel` usually starts automatically with your 1g4 session. You can also launch it manually from the terminal.

### 🎨 Customizing
Make the panel your own!
- **Right-click** anywhere on the panel to access the context menu.
- **"Configure Panel"**: Adjust global settings like position, size, and theme.
- **"Manage Widgets"**: Add, remove, or reorder plugins.

> **Tip:** If a specific plugin's context menu blocks the main panel menu, look for the "handle" on the left side of the widget (in supported themes like `Frost`) or use the "Manage Widgets" dialog to move it.

---

<div align="center">
  <sub>Built with ❤️ by the 1g4 team — <a href="https://1g4.org">1g4.org</a></sub>
</div>
