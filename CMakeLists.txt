cmake_minimum_required(VERSION 3.20)
project(ParasolidGeometryRepair VERSION 1.0.0 LANGUAGES CXX)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt6配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGLWidgets
    3DCore
    3DRender
    3DInput
    3DExtras
)

# 查找Parasolid
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
find_package(Parasolid REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PARASOLID_INCLUDE_DIR}
)

# 源文件
set(CORE_SOURCES
    src/core/PsKernel.cpp
    src/core/SewingEngine.cpp
    src/core/CheckRepairEngine.cpp
    src/core/PatchingEngine.cpp
    src/core/FaceRemoveEngine.cpp
    src/core/BeamExtractEngine.cpp
)

set(GEOMETRY_SOURCES
    src/geometry/PsBody.cpp
    src/geometry/PsFace.cpp
    src/geometry/PsEdge.cpp
    src/geometry/PsVertex.cpp
    src/geometry/PsGeometryUtils.cpp
)

set(IO_SOURCES
    src/io/PsSTEPImporter.cpp
    src/io/PsXTImporter.cpp
    src/io/PsSTEPExporter.cpp
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/Viewport3D.cpp
    src/ui/ModelTree.cpp
    src/ui/PropertyPanel.cpp
    src/ui/CheckResultDialog.cpp
    src/ui/IssueTableModel.cpp
    src/ui/SewingDialog.cpp
    src/ui/PatchingDialog.cpp
    src/ui/FaceRemoveDialog.cpp
    src/ui/BeamExtractDialog.cpp
    src/ui/ProgressDialog.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${GEOMETRY_SOURCES}
    ${IO_SOURCES}
    ${UI_SOURCES}
)

# 头文件
set(HEADERS
    include/core/PsKernel.h
    include/core/SewingEngine.h
    include/core/CheckRepairEngine.h
    include/core/PatchingEngine.h
    include/core/FaceRemoveEngine.h
    include/core/BeamExtractEngine.h
    include/geometry/PsTypes.h
    include/geometry/PsBody.h
    include/geometry/PsFace.h
    include/geometry/PsEdge.h
    include/geometry/PsVertex.h
    include/io/PsImporter.h
    include/io/PsSTEPImporter.h
    include/io/PsXTImporter.h
    include/io/PsExporter.h
    include/io/PsSTEPExporter.h
    include/ui/MainWindow.h
    include/ui/Viewport3D.h
    include/ui/ModelTree.h
    include/ui/PropertyPanel.h
    include/ui/CheckResultDialog.h
    include/ui/IssueTableModel.h
    include/ui/SewingDialog.h
    include/ui/PatchingDialog.h
    include/ui/FaceRemoveDialog.h
    include/ui/BeamExtractDialog.h
    include/ui/ProgressDialog.h
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 可执行文件
add_executable(${PROJECT_NAME}
    ${ALL_SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::3DCore
    Qt6::3DRender
    Qt6::3DInput
    Qt6::3DExtras
    ${PARASOLID_LIBRARIES}
)

# Windows特定配置
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE FALSE)
endif()

# 编译选项
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# 定义宏
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_NO_KEYWORDS
    $<$<CONFIG:Debug>:PS_DEBUG>
    $<$<CONFIG:Release>:PS_RELEASE>
)

# 安装
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/ParasolidGeometryRepair
)

# 测试
enable_testing()
add_subdirectory(tests)

# 输出配置信息
message(STATUS "")
message(STATUS "=== ParasolidGeometryRepair Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Qt6 Version: ${Qt6Core_VERSION}")
message(STATUS "Parasolid Include: ${PARASOLID_INCLUDE_DIR}")
message(STATUS "Parasolid Libraries: ${PARASOLID_LIBRARIES}")
message(STATUS "============================================")
message(STATUS "")
