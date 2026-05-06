{
  description = "Fcitx5 Intel Chewing";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" ];
      
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f (
        import nixpkgs { inherit system; }
      ));
    in
    {
      packages = forAllSystems (pkgs: {
        default = pkgs.callPackage ./default.nix { };
      });
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          inputsFrom = [ self.packages.${pkgs.system}.default ];
          packages = with pkgs; [
          fcitx5
          qt6Packages.fcitx5-configtool
          fcitx5
          qt6Packages.fcitx5-configtool
          qt6Packages.fcitx5-chinese-addons
          gettext
          libchewing
          fcitx5-table-extra
          cmake
          pkg-config
          gettext
          ];
          shellHook = ''
          export FCITX_ADDON_DIRS="$PWD/build/lib:$FCITX_ADDON_DIRS"
          '';
        };
      });
    };
}
