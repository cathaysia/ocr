set(QHOTKEY_INSTALL OFF CACHE BOOL "启用安装规则")
add_subdirectory(QHotkey)
set(SW_BUILD OFF CACHE BOOL "使用 SW BUILD")
add_subdirectory(leptonica)

add_subdirectory(fmt)

set(SPDLOG_ENABLE_PCH ON CACHE BOOL "启用预编译头")
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "使用外部的 fmt 库")
add_subdirectory(spdlog)

if(${${PROJECT_NAME}_BUILD_TEST})
  set(INSTALL_GTEST OFF CACHE BOOL "安装 gtest 库")
  set(gtest_force_shared_crt ON CACHE BOOL "使用动态 crt 库")
  add_subdirectory(googletest)
  include(GoogleTest)
endif()
