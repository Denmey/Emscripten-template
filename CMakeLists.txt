cmake_minimum_required(VERSION 3.8)

# ---- Project ----

project(EMSC 
  VERSION 0.1
  LANGUAGES C CXX
)

# ---- Load dependencies ----

include(cmake/conan.cmake)

conan_add_remote(
    NAME bincrafters INDEX 1
    URL https://api.bintray.com/conan/bincrafters/public-conan)

# conan_cmake_run(REQUIRES glad/0.1.33
#                 BASIC_SETUP CMAKE_TARGETS
#                 BUILD missing
#                 )
#                 # ARCH x86)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(GLFW_LIBRARIES "-s USE_GLFW=3")
  set(IMGUI_ARCH ARCH x86)
else()
  conan_cmake_run(REQUIRES glfw/3.3.2
                  BASIC_SETUP CMAKE_TARGETS
                  BUILD missing)
  set(GLFW_LIBRARIES "CONAN_PKG::glfw")
endif()

conan_cmake_run(REQUIRES imgui/1.77
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing
                ${IMGUI_ARCH})

conan_cmake_run(REQUIRES glm/0.9.9.8
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)
                
conan_cmake_run(REQUIRES entt/3.3.2
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)


# ---- Add executable ----

FILE(GLOB sources 
  # "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/Application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/Shader.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/extern/GLAD/glad/glad.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_impl_opengl3.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui/imgui_impl_glfw.cpp"
)


add_executable(${PROJECT_NAME} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES
  LANGUAGE CXX
  CXX_STANDARD 17
)

target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/extern/GLAD"
  "${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui"
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  # See https://stackoverflow.com/questions/45260216/emscripten-cmake-pass-emscripten-options-in-cmakelist-file
  # for various interesting flags for Emscripten
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  # set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32")

  # Flags from imgui's repository example for Emscripten
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s NO_FILESYSTEM=1")
  
  # Flag to support ES3
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s FULL_ES3=1")  
  
  # Compile definition to use in hard-coded shaders
  target_compile_definitions(${PROJECT_NAME} PRIVATE GLSL_VERSION=130)

  # Definitions for imgui
  target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_ES3)
  target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_DISABLE_FILE_FUNCTIONS)

  # Packaging resources
  target_link_options(${PROJECT_NAME} PRIVATE --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@resources)
else()
  # Compile definition to use in hard-coded shaders
  # target_compile_definitions(${PROJECT_NAME} PRIVATE GLSL_VERSION=130)
  # Copy resources to binary's location
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying resources"
  )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
  )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD PROJECT_NAME="${PROJECT_NAME}")


# message(STATUS ${CMAKE_BINARY_DIR})
# message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
# message(STATUS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_link_libraries(${PROJECT_NAME} PRIVATE  ${GLFW_LIBRARIES} CONAN_PKG::glm CONAN_PKG::entt CONAN_PKG::imgui)