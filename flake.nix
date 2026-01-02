{
	description = "A development shell for intel chewing";

	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
	};

	outputs = { self, nixpkgs }:

	let
		system = "x86_64-linux";
		pkgs = import nixpkgs {
			inherit system;
			config.allowUnfree = true;
		};
	in
	{
		devShells.${system}.default = pkgs.mkShell {
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
	};
}

