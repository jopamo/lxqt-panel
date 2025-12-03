/* panel/main.cpp
 * Implementation file for main
 */

#include "oneg4panelapplication.h"

/*! The 1g4-panel is the panel of OneG4.
  Usage: 1g4-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.config/1g4-panel/panel.conf
                   (default main)
 */

int main(int argc, char* argv[]) {
  OneG4PanelApplication app(argc, argv);

  return app.exec();
}