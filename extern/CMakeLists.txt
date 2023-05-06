include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-w CXX_SupportDisableWarning)
if(${CXX_SupportDisableWarning})
    add_compile_options(-w)
endif()

set(QHOTKEY_INSTALL OFF CACHE BOOL "启用安装规则")
add_subdirectory(QHotkey)

if(WIN32)
    add_subdirectory(kvantum-on-windows)
endif()
