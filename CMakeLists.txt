cmake_minimum_required(VERSION 3.23)
project(inventory_system)

set(CMAKE_CXX_STANDARD 23)

# vcpkg toolchain (CLion handles this well)
find_package(Crow CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(inventory_system main.cpp src/database/connection.h src/database/connection.cpp src/database/models/product.h src/database/models/product.cpp src/api/controllers/product_controller.h src/api/controllers/product.controller.cpp src/api/routes.h src/api/routes.cpp
        src/database/connection.cpp
        src/database/connection.cpp)

target_link_libraries(inventory_system
        Crow::Crow
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        )