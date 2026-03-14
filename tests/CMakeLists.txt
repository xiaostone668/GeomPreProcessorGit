cmake_minimum_required(VERSION 3.20)

# 测试可执行文件
set(TEST_SOURCES
    test_main.cpp
    test_sewing.cpp
    test_check_repair.cpp
    test_patching.cpp
    test_face_remove.cpp
    test_beam_extract.cpp
)

add_executable(ParaSolidGeometryRepair_tests ${TEST_SOURCES})

# 链接测试库
target_link_libraries(ParaSolidGeometryRepair_tests PRIVATE
    Qt6::Core
    Qt6::Test
)

# 包含目录
target_include_directories(ParaSolidGeometryRepair_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${PARASOLID_INCLUDE_DIR}
)

# 添加测试
add_test(NAME ParaSolidGeometryRepair_test COMMAND ParaSolidGeometryRepair_tests)
