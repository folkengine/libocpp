include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0       
)

# Fetch GoogleTest amd make build scripts available
FetchContent_MakeAvailable(googletest)


add_executable(v16 lib/ocpp/v16/test_smart_charging_handler.cpp)
target_link_libraries(v16 gtest_main gmock_main)
target_include_directories(v16 PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        lib
        ../lib/ocpp/v16
        date::date
        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator
        pthread)
add_test(NAME v16_test COMMAND v16)

# include(GoogleTest)

# add_definitions(-D_SQL_INIT_FILE="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/everest/modules/OCPP/init.sql")
# add_executable(database_tests database_tests.cpp)

# target_include_directories(database_tests PUBLIC)

# find_package(GTest REQUIRED)

# target_link_libraries(database_tests PRIVATE
#     ocpp
#     date::date
#     nlohmann_json::nlohmann_json
#     nlohmann_json_schema_validator
#     pthread
#     )


# add_test(database_tests database_tests)

# add_executable(libocpp_unit_tests
#         lib/ocpp/v201/test_component_state_manager.cpp)

# add_custom_command(TARGET libocpp_unit_tests POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model.db
#         COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/unittest_device_model_missing_required.db ${CMAKE_CURRENT_BINARY_DIR}/resources/unittest_device_model_missing_required.db
# )

# add_test(libocpp_unit_tests libocpp_unit_tests)

# target_include_directories(libocpp_unit_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# # target_link_libraries(libocpp_unit_tests PRIVATE
# #         ocpp
# #         GTest::gmock_main
# # )
# target_sources(libocpp_unit_tests PRIVATE
#         comparators.cpp)

# add_subdirectory(lib/ocpp/v16)
# add_subdirectory(lib/ocpp/v201)
# add_subdirectory(lib/ocpp/common)


# add_executable(utils_tests utils_tests.cpp)

# target_include_directories(utils_tests PUBLIC)

# target_link_libraries(utils_tests PRIVATE
#         ocpp
# )

# add_test(utils_tests utils_tests)
