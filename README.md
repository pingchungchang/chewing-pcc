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
