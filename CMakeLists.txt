
#setting cmake version

cmake_minimum_required(VERSION 3.18)

#adding useful functions

function(DIR_EXISTS variable dir_path)

file(GLOB ${variable}_check ${dir_path}/*)

list(LENGTH ${variable}_check ${variable}_len)

if(${${variable}_len} EQUAL 0)

set(${variable} FALSE PARENT_SCOPE)

else()

set(${variable} TRUE PARENT_SCOPE)

endif()

endfunction()

#adding extra cmake libs
include(GNUInstallDirs)
include(ExternalProject)
include(FetchContent)

#creating variables for ease of adding libraries
set(DEPS_TO_BUILD )

#project name
project("SimuDraco")
        
        
            
#-------------- external repositories ---------------
            
            

# repository download for glfw...
                    
dir_exists(glfw_exists ${PROJECT_SOURCE_DIR}/vendor/glfw)

if(NOT ${glfw_exists})
    ExternalProject_Add(GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.6
    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_SOURCE_DIR}/vendor/glfw
    BUILD_BYPRODUCTS ${PROJECT_SOURCE_DIR}/vendor/glfw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    list(APPEND DEPS_TO_BUILD GLFW)

endif()


                    

# repository download for glew...
                    
dir_exists(glew_exists ${PROJECT_SOURCE_DIR}/vendor/glew)

if(NOT ${glew_exists})
    ExternalProject_Add(GLEW
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
    GIT_TAG glew-cmake-2.2.0
    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_SOURCE_DIR}/vendor/glew
    BUILD_BYPRODUCTS ${PROJECT_SOURCE_DIR}/vendor/glew/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glew$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    list(APPEND DEPS_TO_BUILD GLEW)

endif()


                    

# repository download for glm...
                    
dir_exists(glm_exists ${PROJECT_SOURCE_DIR}/vendor/glm)

if(NOT ${glm_exists})
    FetchContent_Declare(GLM
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0.9.9.8
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/glm
    )

    FetchContent_MakeAvailable(GLM)

endif()
                    
                    

# repository download for thread_pool...
                    
dir_exists(thread_pool_exists ${PROJECT_SOURCE_DIR}/vendor/thread_pool)

if(NOT ${thread_pool_exists})
    FetchContent_Declare(THREAD_POOL
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool
    GIT_TAG v2.0.0
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/thread_pool
    )

    FetchContent_MakeAvailable(THREAD_POOL)

endif()
                    
                    

# repository download for implot...
                    
dir_exists(implot_exists ${PROJECT_SOURCE_DIR}/vendor/implot)

if(NOT ${implot_exists})
    FetchContent_Declare(IMPLOT
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG v0.13
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/implot
    )

    FetchContent_MakeAvailable(IMPLOT)

endif()
                    
                    

# repository download for pybind11...
                    
dir_exists(pybind11_exists ${PROJECT_SOURCE_DIR}/vendor/pybind11)

if(NOT ${pybind11_exists})
    FetchContent_Declare(PYBIND11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG v2.9.1
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/pybind11
    )

    FetchContent_MakeAvailable(PYBIND11)

endif()
                    
                    

# repository download for imgui...
                    
dir_exists(imgui_exists ${PROJECT_SOURCE_DIR}/vendor/imgui)

if(NOT ${imgui_exists})
    FetchContent_Declare(IMGUI
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG origin/master
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/imgui
    )

    FetchContent_MakeAvailable(IMGUI)

endif()
                    
                    

# repository download for nfd...
                    
dir_exists(nfd_exists ${PROJECT_SOURCE_DIR}/vendor/nfd)

if(NOT ${nfd_exists})
    ExternalProject_Add(NFD
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
    GIT_TAG origin/master
    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_SOURCE_DIR}/vendor/nfd
    BUILD_BYPRODUCTS ${PROJECT_SOURCE_DIR}/vendor/nfd/lib/${CMAKE_STATIC_LIBRARY_PREFIX}nfd${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    list(APPEND DEPS_TO_BUILD NFD)

endif()


                    
#finding packages...            

find_package(OpenGL REQUIRED)

find_package(Python COMPONENTS Development Interpreter REQUIRED)


#creating executable
add_executable(${PROJECT_NAME}

    #source files...
    ${PROJECT_SOURCE_DIR}/source/general/color.cpp
	${PROJECT_SOURCE_DIR}/source/general/registry.cpp
	${PROJECT_SOURCE_DIR}/source/general/registry_creates.cpp
	${PROJECT_SOURCE_DIR}/source/general/registry_deleters.cpp
	${PROJECT_SOURCE_DIR}/source/general/registry_getters.cpp
	${PROJECT_SOURCE_DIR}/source/opengl_wrappers/index_buffer.cpp
	${PROJECT_SOURCE_DIR}/source/opengl_wrappers/shader.cpp
	${PROJECT_SOURCE_DIR}/source/opengl_wrappers/vertex_array.cpp
	${PROJECT_SOURCE_DIR}/source/opengl_wrappers/vertex_buffer.cpp
	${PROJECT_SOURCE_DIR}/source/window/window.cpp
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
	${PROJECT_SOURCE_DIR}/vendor/implot/implot.cpp
	${PROJECT_SOURCE_DIR}/vendor/implot/implot_items.cpp
	${PROJECT_SOURCE_DIR}/source/gui/gui_layer.cpp
	${PROJECT_SOURCE_DIR}/source/python_layer/python_layer.cpp
	${PROJECT_SOURCE_DIR}/source/general/structures.cpp
)
        
#setting c/cpp standard

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

        
#adding dependencies

foreach(X ${DEPS_TO_BUILD})

    add_dependencies(${PROJECT_NAME} ${X})

endforeach()


            
# ------------- linking libraries -------------
            

    #linking for glfw...


    target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glfw/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX})
                

    #linking for glew...


    target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glew/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glew$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX})
                

    #linking for nfd...


    target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/nfd/lib/${CMAKE_STATIC_LIBRARY_PREFIX}nfd${CMAKE_STATIC_LIBRARY_SUFFIX})
                

    #linking for OpenGL...


    target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES})
                

    #linking for Python...


    target_link_libraries(${PROJECT_NAME} PUBLIC ${Python_LIBRARIES})
                

#------------ include directories -------------

            
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glfw/include)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glew/include)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/glm)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/thread_pool)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/implot)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/pybind11/include)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/imgui)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor/nfd/include)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${Python_INCLUDE_DIRS})
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/vendor)
    

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/source)
    
