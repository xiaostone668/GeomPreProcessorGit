# FindParasolid.cmake
# 查找Parasolid SDK
#
# 定义以下变量:
#   PARASOLID_FOUND - 是否找到Parasolid
#   PARASOLID_INCLUDE_DIR - 头文件目录
#   PARASOLID_LIBRARIES - 库文件
#
# 可以设置PARASOLID_ROOT环境变量或CMake变量来指定安装路径

include(FindPackageHandleStandardArgs)

# 搜索路径
set(_PARASOLID_SEARCH_PATHS
    ${PARASOLID_ROOT}
    $ENV{PARASOLID_ROOT}
    "C:/Program Files/Siemens/Parasolid"
    "C:/Parasolid"
    "/opt/parasolid"
    "/usr/local/parasolid"
    "/usr/lib/parasolid"
)

# 查找头文件
find_path(PARASOLID_INCLUDE_DIR
    NAMES kernel.h pk_kernel.h
    PATHS ${_PARASOLID_SEARCH_PATHS}
    PATH_SUFFIXES include includes
    DOC "Parasolid include directory"
)

# 查找库文件
if(WIN32)
    find_library(PARASOLID_LIBRARY
        NAMES pskernel ps_kernel libpskernel
        PATHS ${_PARASOLID_SEARCH_PATHS}
        PATH_SUFFIXES lib lib64 dll
        DOC "Parasolid library"
    )
else()
    find_library(PARASOLID_LIBRARY
        NAMES pskernel ps_kernel
        PATHS ${_PARASOLID_SEARCH_PATHS}
        PATH_SUFFIXES lib lib64
        DOC "Parasolid library"
    )
endif()

set(PARASOLID_LIBRARIES ${PARASOLID_LIBRARY})

# 处理结果
find_package_handle_standard_args(Parasolid
    REQUIRED_VARS PARASOLID_INCLUDE_DIR PARASOLID_LIBRARY
    FAIL_MESSAGE "Could not find Parasolid. Please set PARASOLID_ROOT."
)

if(PARASOLID_FOUND)
    if(NOT Parasolid_FIND_QUIETLY)
        message(STATUS "Found Parasolid:")
        message(STATUS "  Include: ${PARASOLID_INCLUDE_DIR}")
        message(STATUS "  Library: ${PARASOLID_LIBRARY}")
    endif()

    # 创建导入目标
    if(NOT TARGET Parasolid::Parasolid)
        add_library(Parasolid::Parasolid UNKNOWN IMPORTED)
        set_target_properties(Parasolid::Parasolid PROPERTIES
            IMPORTED_LOCATION "${PARASOLID_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${PARASOLID_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(PARASOLID_INCLUDE_DIR PARASOLID_LIBRARY)
