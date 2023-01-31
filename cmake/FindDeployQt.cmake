find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS Core)
find_program(
  DeployQt
  NAMES windeployqt macdeployqt
  PATHS "${CMAKE_PREFIX_PATH}/bin" "${_qmake_dir}" "${Qt${QT_DEFAULT_MAJOR_VERSION}Core_DIR}/../../../bin"
)
