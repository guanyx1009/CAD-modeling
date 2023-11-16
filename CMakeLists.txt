cmake_minimum_required(VERSION 3.20)
project(CAD_Modeling)

set(CMAKE_CXX_STANDARD 17)

set(VTK_BASE_DIR F:/3rdparty/VTK-9.3.0.rc2/build)
find_package(VTK
        PATHS ${VTK_BASE_DIR}
        NO_DEFAULT_PATH
        REQUIRED)

file(GLOB_RECURSE VTK_LIB_FILES ${VTK_BASE_DIR}/lib/*.lib
        ${VTK_BASE_DIR}/bin/*.dll)
file(COPY ${VTK_LIB_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/cmake-build-debug)

set(LIBS ${VTK_LIBRARIES})
set(SOURCE_FILES src/HalfEdge.h src/Common.h src/EulerOperation.h src/Draw.cpp)

add_executable(CAD_Modeling ${SOURCE_FILES})
target_link_libraries(CAD_Modeling ${LIBS})

vtk_module_autoinit(
        TARGETS CAD_Modeling
        MODULES ${VTK_LIBRARIES}
)
