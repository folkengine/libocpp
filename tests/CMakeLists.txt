# Use FetchContent to download and build GoogleTest
include(FetchContent)
FetchContent_Declare(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(gtest)

# GIT_TAG release-1.12.1   
# GIT_TAG v1.14.0 
# release-1.10.0

#########################################################
# Run Database Tests

add_definitions(-D_SQL_INIT_FILE="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP/init.sql")
add_executable(database_tests database_tests.cpp)
target_include_directories(database_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(database_tests PRIVATE
    ocpp
    date::date
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator
    gtest_main
    pthread
)

add_test(database_tests database_tests)

#########################################################
# Run OCPP Common Tests

add_executable(libocpp_common_unit_tests
        lib/ocpp/common/test_message_queue.cpp)

target_include_directories(libocpp_common_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libocpp_common_unit_tests PRIVATE
        ocpp
        gtest_main
        gmock_main
)
add_subdirectory(lib/ocpp/common)
add_test(libocpp_common_unit_tests libocpp_common_unit_tests)

#########################################################
# Run OCPP v201 Tests

add_executable(libocpp_v201_unit_tests
        lib/ocpp/v201/test_component_state_manager.cpp)

add_custom_command(TARGET libocpp_v201_unit_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
)

target_include_directories(libocpp_v201_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libocpp_v201_unit_tests PRIVATE
        ocpp
        gtest_main
        gmock_main
)
target_sources(libocpp_v201_unit_tests PRIVATE
        comparators.cpp)

add_subdirectory(lib/ocpp/v201)

add_test(libocpp_v201_unit_tests libocpp_v201_unit_tests)

#########################################################
# Run OCPP v16 Tests

add_executable(libocpp_v16_unit_tests
        lib/ocpp/v16/test_smart_charging_handler.cpp)

target_include_directories(libocpp_v16_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libocpp_v16_unit_tests PRIVATE
        ocpp
        gtest_main
        gmock_main
)

add_test(libocpp_v16_unit_tests libocpp_v16_unit_tests)

add_subdirectory(lib/ocpp/v16)

#########################################################
# Run Utils Tests

add_executable(utils_tests utils_tests.cpp)
target_include_directories(utils_tests PUBLIC)
target_link_libraries(utils_tests PRIVATE
        ocpp
        gtest_main
)

add_test(utils_tests utils_tests)
