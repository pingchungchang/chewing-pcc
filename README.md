- a self made fcitx-chewing input method which changes to English after a number of error in the chinese bopomofo string.
- to config, change the IntelChewingConfigs namespace variables
- only tested on arch linux, other distros may work if the /usr/lib/fcitx5/, /usr/share/fcitx5/inputmethod, /usr/share/fcitx5/addons or such installation paths can be found by cmake
- to install, run ./install.sh

dependencies:
fcitx5
fcitx5-chinese-addons
fcitx5-table-extra
libchewing
