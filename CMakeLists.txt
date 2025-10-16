cmake_minimum_required(VERSION 3.25)
project(mSLAM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
# option(OPENCV_COPY_DLLS "Copy OpenCV DLLs next to the exe after build" ON)

include_directories("${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include")

find_package(OpenCV 4.5 REQUIRED COMPONENTS core imgproc highgui videoio features2d calib3d)

find_package(Eigen3 CONFIG REQUIRED)
find_package(Sophus CONFIG REQUIRED)
find_package(Pangolin REQUIRED) 

include_directories(${Pangolin_INCLUDE_DIRS})


add_executable(mSLAM
  mSLAM.cpp
  "Frame.cpp"
  "Frontend.cpp"
  "Viewer.cpp"
  "MapPoint.cpp"
  "Map.cpp")

target_include_directories(mSLAM PRIVATE 
    ${OpenCV_INCLUDE_DIRS} 
    ${Pangolin_INCLUDE_DIRS}
)
target_link_libraries(mSLAM PRIVATE 
    ${OpenCV_LIBS} 
    ${Pangolin_LIBRARIES}
    Eigen3::Eigen
    Sophus::Sophus
)

target_compile_features(mSLAM PRIVATE cxx_std_23)

# ---- MSVC niceties ----
if(MSVC)
  target_compile_options(mSLAM PRIVATE /W4 /permissive- /EHsc /Zc:__cplusplus)
  target_compile_definitions(mSLAM PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
  if(ENABLE_WARNINGS_AS_ERRORS)
    target_compile_options(mSLAM PRIVATE /WX)
  endif()
endif()

# ---- Copy OpenCV runtime DLLs to the output (so you don't need to edit PATH) ----
if(WIN32 AND OPENCV_COPY_DLLS)
  # Provided via CMakePresets; edit there if your path differs.
  set(OPENCV_BIN_DIR "" CACHE PATH "Path to OpenCV bin (e.g. C:/opencv/build/x64/vc16/bin)")
  if(EXISTS "${OPENCV_BIN_DIR}")
    add_custom_command(TARGET mSLAM POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${OPENCV_BIN_DIR}" "$<TARGET_FILE_DIR:mSLAM>"
      COMMENT "Copying OpenCV DLLs from ${OPENCV_BIN_DIR} to $<TARGET_FILE_DIR:mSLAM>")
  else()
    message(STATUS "OPENCV_BIN_DIR not set or does not exist; skipping DLL copy. Ensure the bin dir is on PATH.")
  endif()
endif()

install(TARGETS mSLAM RUNTIME DESTINATION .)
