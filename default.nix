{ lib, stdenv, cmake, fcitx5, fcitx5-configtool, fcitx5-chinese-addons, gettext, libchewing, fcitx5-table-extra, pkg-config, src
  # 確保所有外部依賴都被明確地列在函式簽名中
  # ... 你可能有的其他依賴，例如 libime, qtbase 等
}:
stdenv.mkDerivation {
	name = "env";
	src = lib.cleanSource ./.;
	nativeBuildInputs = [ cmake ];
	buildInputs = [ fcitx5 fcitx5-configtool fcitx5-chinese-addons gettext libchewing fcitx5-table-extra pkg-config ];
	cmakeFlags = [
		"-DFCITX_INSTALL_LIBDIR=${placeholder "out"}/lib/fcitx5"
		"-DFCITX_INSTALL_PKGDIR=${placeholder "out"}/share/fcitx5"
	];
	propagatedBuildInputs = [
		fcitx5-chinese-addons
		fcitx5-table-extra
		libchewing
	];
}
