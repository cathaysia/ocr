find_package(Qt5 COMPONENTS Core)
find_program(
  DeployQt
  NAMES windeployqt macdeployqt
  PATHS "${CMAKE_PREFIX_PATH}/bin" "${_qmake_dir}" "${Qt5Core_DIR}/../../../bin"
)
