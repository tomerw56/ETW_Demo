cmake_minimum_required(VERSION 3.15)
project(WPADemo)

# Set the output directory for all executables
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Recursively get all .cpp files in the "src" directory
file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_SOURCE_DIR} src/*.cpp)

foreach(SRC_FILE IN LISTS CPP_FILES)
    # Get the filename without extension
    get_filename_component(FILE_NAME ${SRC_FILE} NAME_WE)

    # Create an executable
    add_executable(${FILE_NAME} ${SRC_FILE})

    # Set the output location for the executable
    set_target_properties(${FILE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    )
	 # Link special libraries if this is the GPU demo
    if(FILE_NAME STREQUAL "gpu_demo")
        target_link_libraries(${FILE_NAME}
            d3d11 dxgi d3dcompiler user32 gdi32
        )
        target_compile_definitions(${FILE_NAME} PRIVATE NOMINMAX)
		# Tell MSVC this is a Win32 subsystem (uses WinMain, not main)
		set_target_properties(${FILE_NAME} PROPERTIES
			WIN32_EXECUTABLE TRUE
		)
    endif()

    # Optionally, copy the exe to OUTPUT_DIR explicitly after build
    add_custom_command(TARGET ${FILE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${FILE_NAME}>"
            "${OUTPUT_DIR}/$<TARGET_FILE_NAME:${FILE_NAME}>"
    )
endforeach()

file(GLOB_RECURSE RESOURCE_FILES RELATIVE ${CMAKE_SOURCE_DIR} resources/*.*)
foreach(RESOURCE_FILE IN LISTS RESOURCE_FILES)
	message(STATUS "copy file ${RESOURCE_FILE}")
	file(COPY "${RESOURCE_FILE}" DESTINATION "${OUTPUT_DIR}")
endforeach()

# Enable Doxygen if available
find_package(Doxygen REQUIRED)

# Set input/output for Doxygen
set(DOXYGEN_INPUT_DIR ${CMAKE_SOURCE_DIR}/src)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)

# Configure Doxygen file
configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

# Add custom target for generating documentation
add_custom_target(doc_doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating Doxygen documentation"
    VERBATIM
)

