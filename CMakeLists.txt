cmake_minimum_required(VERSION 3.25)

# 1. Project Name (Can be anything)
project(AethexGUI)

# 2. Find Geode (This connects your code to the SDK)
find_package(Geode CONFIG REQUIRED)

# 3. Add your source files
# This tells the compiler to look at your main.cpp
#do not change the ${PROJECT_NAME} 
add_geode_mod(
    ${PROJECT_NAME}
    src/main.cpp
)

# 4. Include the Geode headers
# This is what allows you to use things like <Geode/modify/PlayLayer.hpp>
target_link_libraries(${PROJECT_NAME} PUBLIC geode::sdk)
