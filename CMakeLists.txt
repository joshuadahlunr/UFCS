cmake_minimum_required(VERSION 3.21)
project(FlexBisonTemplate)

if(MSVC)
	include(FetchContent)

	FetchContent_Declare(FlexBison_fetch GIT_REPOSITORY https://github.com/lexxmark/winflexbison.git)
	FetchContent_MakeAvailable(FlexBison_fetch)

	add_executable(flex ALIAS win_flex)
	add_executable(bison ALIAS win_bison)
else()
	include(ExternalProject)

	find_program(MAKE_EXECUTABLE NAMES gmake make mingw32-make REQUIRED)

	add_executable(flex IMPORTED)
	add_executable(bison IMPORTED)

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

	find_program(BISON_EXECUTABLE bison)
	if(BISON_EXECUTABLE MATCHES "BISON_EXECUTABLE-NOTFOUND")
		set(config_flags)  # parameters desired for ./configure of Autotools

		set(BISON_EXECUTABLE ${CMAKE_BINARY_DIR}/bison_fetch-prefix/src/bison_fetch-build/src/bison)

		ExternalProject_Add(bison_fetch
			URL https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz
			URL_HASH MD5=1e541a097cda9eca675d29dd2832921f
			DOWNLOAD_EXTRACT_TIMESTAMP true
			CONFIGURE_HANDLED_BY_BUILD true
			CONFIGURE_COMMAND <SOURCE_DIR>/configure ${config_flags}
			BUILD_COMMAND ${MAKE_EXECUTABLE} -j
			INSTALL_COMMAND ""
			TEST_COMMAND ""
		)
		add_dependencies(bison bison_fetch)

		configure_file(scripts/bison-shim.sh.in ${CMAKE_BINARY_DIR}/bison-shim.sh)
		set(BISON_EXECUTABLE ${CMAKE_BINARY_DIR}/bison-shim.sh)
	endif()
	message(${BISON_EXECUTABLE})

	
	set_property(TARGET flex PROPERTY IMPORTED_LOCATION ${FLEX_EXECUTABLE})
	set_property(TARGET bison PROPERTY IMPORTED_LOCATION ${BISON_EXECUTABLE})
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/gen)

set(FlexOutput ${CMAKE_SOURCE_DIR}/gen/scanner.h)
add_custom_command(
	OUTPUT ${FlexOutput}
	COMMAND flex
			--outfile=${FlexOutput}
			${CMAKE_SOURCE_DIR}/calculator.l
)
add_custom_target(run_flex ALL DEPENDS ${FlexOutput})
add_dependencies(run_flex flex)

set(BisonOutput ${CMAKE_SOURCE_DIR}/gen/parser.h)
add_custom_command(
	OUTPUT ${BisonOutput}
	COMMAND bison --output=${BisonOutput}
			${CMAKE_SOURCE_DIR}/calculator.y
)
add_custom_target(run_bison ALL DEPENDS ${BisonOutput})
add_dependencies(run_bison bison)

add_executable(tst hello.cpp)
add_dependencies(tst run_flex run_bison)