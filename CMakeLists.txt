cmake_minimum_required(VERSION 3.21)
project(FlexBisonTemplate)

if(MSVC)
	include(FetchContent)

	FetchContent_Declare(FlexBison_fetch GIT_REPOSITORY https://github.com/lexxmark/winflexbison.git)
	FetchContent_MakeAvailable(FlexBison_fetch)

	add_executable(flex ALIAS win_flex)
else()
	include(ExternalProject)

	find_program(MAKE_EXECUTABLE NAMES gmake make mingw32-make REQUIRED)

	add_executable(flex IMPORTED)
	find_program(FLEX_EXECUTABLE flex flex++)
	if(FLEX_EXECUTABLE MATCHES "FLEX_EXECUTABLE-NOTFOUND")
		set(config_flags)  # parameters desired for ./configure of Autotools

		set(FLEX_EXECUTABLE ${CMAKE_BINARY_DIR}/flex_fetch-prefix/src/flex_fetch-build/src/flex)

		ExternalProject_Add(flex_fetch
			URL https://github.com/westes/flex/files/981163/flex-2.6.4.tar.gz
			DOWNLOAD_EXTRACT_TIMESTAMP true
			CONFIGURE_COMMAND <SOURCE_DIR>/configure ${config_flags}
			BUILD_COMMAND ${MAKE_EXECUTABLE} -j
			INSTALL_COMMAND ""
			TEST_COMMAND ""
		)
		add_dependencies(flex flex_fetch)
	endif()
	message(${FLEX_EXECUTABLE})

	set_property(TARGET flex PROPERTY IMPORTED_LOCATION ${FLEX_EXECUTABLE})
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/gen)

set(FlexOutput ${CMAKE_SOURCE_DIR}/gen/scanner.h)
add_custom_command(
	OUTPUT ${FlexOutput}
	COMMAND flex
			--outfile=${FlexOutput}
			${CMAKE_SOURCE_DIR}/ufcs.l
	DEPENDS ${CMAKE_SOURCE_DIR}/ufcs.l
)
add_custom_target(run_flex ALL DEPENDS ${FlexOutput})
add_dependencies(run_flex flex)

add_subdirectory(compile-time-regular-expressions)
add_subdirectory(argparse)

add_executable(ufcs ufcs.cpp)
target_link_libraries(ufcs PUBLIC ctre argparse)
add_dependencies(ufcs run_flex)