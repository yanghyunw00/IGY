# author Hyunseok Choi
# date 2025 Fall
# CS200 Computer Graphics I
# copyright DigiPen Institute of Technology

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(the_json INTERFACE)
target_include_directories(the_json SYSTEM INTERFACE ${nlohmann_json_SOURCE_DIR}/include)