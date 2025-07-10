find_package(PkgConfig REQUIRED)
pkg_check_modules(CHEWING REQUIRED chewing)
include_directories(${CHEWING_INCLUDE_DIRS})
link_directories(${CHEWING_LIBRARY_DIRS})
add_definitions(${CHEWING_CFLAGS_OTHER})

# Make sure it produce intel_chewing.so instead of libintel_chewing.so
add_library(intel_chewing SHARED intel_chewing.cpp)
target_link_libraries(intel_chewing PRIVATE Fcitx5::Core ${CHEWING_LIBRARIES})
install(TARGETS intel_chewing DESTINATION "${FCITX_INSTALL_LIBDIR}/fcitx5")
# Addon config file
# We need additional layer of conversion because we want PROJECT_VERSION in it.
configure_file(intel_chewing-addon.conf.in.in intel_chewing-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/intel_chewing-addon.conf.in" intel_chewing-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/intel_chewing-addon.conf" RENAME intel_chewing.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

# Input Method registration file
fcitx5_translate_desktop_file(intel_chewing.conf.in intel_chewing.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/intel_chewing.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")
