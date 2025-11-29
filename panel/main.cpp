/* panel/main.cpp
 * Implementation file for main
 */

#include "lxqtpanelapplication.h"

/*! The lxqt-panel is the panel of LXQt.
  Usage: lxqt-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.config/lxqt-panel/panel.conf
                   (default main)
 */

int main(int argc, char* argv[]) {
  LXQtPanelApplication app(argc, argv);

  return app.exec();
}