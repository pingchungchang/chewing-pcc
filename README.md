- a self made fcitx-chewing input method which changes to English after a number of error in the chinese bopomofo string.
- to config, change the IntelChewingConfigs namespace variables in src/intel_chewing.h
- only tested on arch linux, other distros may work if the /usr/lib/fcitx5/, /usr/share/fcitx5/inputmethod, /usr/share/fcitx5/addons or such installation paths can be found by cmake
    - to install, run ./install.sh

- for nixos users, add the following (for NixOS 25.11 users only, use the current branch; for NixOS 25.05 users, use the revision "d58be9c5ea9937fb92339418633ea830f5774fa4"

example usage: 
```
let
src = pkgs.fetchFromGitHub {
  owner = "pingchungchang";
  repo = "chewing-pcc";
  rev = "47fa80ba92f781ae84f42db9f35d76471a87cef1";
  hash = "sha256-GcDh9vHLw5EmHwGsISu78dDiwabbVd8DljD6ntbXgU0=";
};
fcitx5-intel_chewing = pkgs.callPackage (src + "/default.nix") { };
in
{
  i18n.inputMethod = {
    enable = true;
    type = "fcitx5";
    fcitx5.waylandFrontend = true;
    fcitx5.addons = with pkgs; [
		qt6Packages.fcitx5-chinese-addons
        fcitx5-table-extra
        fcitx5-intel_chewing
    ];
};
```

dependencies:
fcitx5
fcitx5-chinese-addons
fcitx5-table-extra
libchewing
pkgconfig

# Usage
The input method switches between Chewing and English. First, it assumes the user is typing Chinese via chewing. After a number of mistakes, which are keypresses that are simple but do not change the length of the bopomofo buffer (e.g. pressing ㄇ after pressing ㄆ, which changes the bopomofo buffer from "ㄆ" into "ㄇ", having the same length). Then, after the number of errors, the input method considers the user to be typing English, hence clears the bopomofo buffer, changes the mode into English, and enters all words typed in the bopomofo buffer in English. 

For example, consider typeing "ㄆㄇㄈ" ，which the bopomofo buffer lenght always 1, if the configuration is allowing 0 errors, then after typing "ㄇ", the buffer is cleared and "qa" is typed (or being sent to the preedit buffer, depending on whether there are other uncommited words in front of it).

Besides using default language changing methods, to change from Chewing to English, press "Tab" once and the language temporary switches from Chewing to English. Due to the implementation, this causes an issue that entering "\t" is this input method requires pressing Tab twice when the preedit buffer is empty.

After an non-simple key press(space, left/right keys etc.), the input method turns back to Chewing mode for users to type Chinese again.
