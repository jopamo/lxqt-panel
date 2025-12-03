# 1g4-panel

## Overview

`1g4-panel` represents the taskbar of OneG4.

The elements available in 1g4-panel are called "plugin" technically. This applies e. g. to the source code where they reside in directories `./plugin-<plugin>` like `plugin-mainmenu`. In contrast to this they are called "widgets" by the configuration GUI so far. Also, a more descriptive term is used to refer to distinct plugins within the GUI. E. g. the aforementioned `plugin-mainmenu` is called "Application menu" that way.
Configuration dialogue "Add Plugins", see [below](https://github.com/jopamo/1g4-panel#customizing), is listing all available plugins plus a short description and hence provides an overview of the available ones.
Notes on some of the plugins, sorted by terms used within the GUI in alphabetical order, technical term in parenthesis:

#### Custom Command (plugin-customcommand)

Allows showing the output of commands or scripts on the panel, as text or images, and executing commands on click and/or wheel up/down.

![Custom command plugin settings](customcommand.png)

#### Date & time / World clock (plugin-worldclock)

Provides clock and calendar functionality and can display various time zones in addition.

#### Quick launch (plugin-quicklaunch)

A plugin to launch applications from the panel. By default it is empty and displays a message "Drop application icons here". Applications need to be available in fancy menu or main menu and can be included into plugin-quicklaunch by drag & drop.

#### Status Notifier Plugin (plugin-statusnotifier) / System Tray (plugin-tray)

Status notifier plugin provides a notification area within the panel, that is an area where arbitrary applications can place informational icons. This is frequently used e. g. by chat or mail clients to inform about incoming messages or tools configuring the network to inform about connections. (So it's some kind of counterpart to the desktop notifications displayed by [oneg4-notificationd](https://github.com/oneg4/oneg4-notificationd)).
The difference between the two plugins is a technical one:
* **plugin-tray** is implementing the so-called [System Tray Protocol](https://www.freedesktop.org/wiki/Specifications/systemtray-spec). It's a specification that has been around for years but has some serious technical limitations. This plugin only translates "System Tray Protocol" entities into SNI ones, so it does not provide any visible area in panel.
* **plugin-statusnotifier** is implementing the so-called [StatusNotifierItem (SNI)](https://www.freedesktop.org/wiki/Specifications/StatusNotifierItem) specification which can be considered a successor of the System Tray Protocol.

Both plugins are maintained in parallel as not all relevant applications are compatible with SNI so far. In particular both Qt 4 and all GTK applications need some kind of wrapper to deal with it. Both plugins can be used in parallel without any issue, applications supporting both specifications will normally chose to display their icons in plugin-statusnotifier.

#### Volume control (plugin-volume)

As indicated by the name, a volume control. Technically Alsa, OSS and PulseAudio can be used as backend. The plugin itself is providing a control to adjust the main volume only but it allows for launching specific UIs of the backend in use like e. g. [1g4-mixer](https://github.com/oneg4/1g4-mixer) to adjust PulseAudio.


## Installation

### Compiling source code


### Binary packages

Official binary packages are provided by all major Linux and BSD distributions. Just use your package manager to search for string  `1g4-panel`.

## Configuration, Usage

### Launching

### Customizing

To customize the panel itself there's a context menu, that is a menu opened by right-clicking the pointer device. It is comprising sections "\<plugin\>" and "Panel" which allow for configuring the plugin the pointer is currently over and the panel as a whole respectively.

In section "Panel" topics "Configure Panel" and "Manage Widgets" open different panes of a dialogue "Configure Panel" which allow for configuring the panel as a whole and the various plugins respectively.
Pane "Widgets" allows for configuring and removing all plugins currently included in 1g4-panel. The plus sign opens another dialogue "Add plugins" which is used to add plugins. It comes with a list of all plugins plus some short descriptions and can hence serve as overview what plugins are available.

Sometimes right-clicking over particular plugins may bring up a context menu dealing with the respective plugin's functionality *only* which means the plugin in question cannot be configured the usual way. This affects e. g. plugin-quicklaunch as soon as items were added (the context menu is limited to topics dealing with the items included in plugin-quicklaunch).
Currently there are two ways to deal with this. Some themes like e. g. `Frost` come with handles at the plugins' left end providing the regular context menu. Also, it can be assumed at least one plugin is included in the panel that's always featuring the regular context menu like e. g. plugin-mainmenu. Either way pane "Widgets" of "Configure Panel" can be accessed and used to configure and move the particpaneular plugin by DND or using the arrows.
