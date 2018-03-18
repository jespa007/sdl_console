cmake_minimum_required(VERSION 2.8)
 
project(sdl_console)


#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#

include_directories (
	${PROJECT_SOURCE_DIR}
)

message("dest dir:" ${CMAKE_INSTALL_PREFIX})

if(MSVC)
	MESSAGE ("********* MSVC ************" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS -std=c++1y -Wall")
   
	if (MINGW)
		MESSAGE ("********* MINGW CONF ************" )
		message("mingw path: $ENV{MINGWDIR} ")
		
		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH " $ENV{MINGWDIR}/local32/lib ")
		find_library(SDL2_LIB SDL2)
		find_library(SDL2_MAIN_LIB SDL2main)

		MESSAGE ("include" ${include_directories})
 
		SET( COMMON_PARAMS " ${COMMON_PARAMS} ")
  
	endif()

	include_directories (
		$ENV{MINGWDIR}/local32/include
	)

	LINK_DIRECTORIES (
		/usr/local/lib
		$ENV{MINGWDIR}/local32/lib
	)


endif()


#
# COMMON PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# RELEASE PARAMS
#


IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "release" )
ENDIF ()
 

if(MSVC)
	IF ( "${TARGET}" STREQUAL "debug")
		add_definitions(/bigobj)
	ENDIF()
else()



	set(TARGET_FLAGS " -O2  ")
	set( POST_NAME "")

	# -D_GLIBCXX_DEBUG

	IF ( "${TARGET}" STREQUAL "debug")
		set( POST_NAME "_d")
		set(TARGET_FLAGS " -O0 ")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer ")
	ENDIF ()


	# Compile for x86 target by default ...
	set(TARGET_FLAGS " ${TARGET_FLAGS} ")


	if ( "${EMSCRIPTEN}" STREQUAL "yes" )

		message("emscripten path: $ENV{EMSCRIPTENDIR} ")
		

		MESSAGE ("********* EMSCRIPTEN CONF ************" )
		set(TARGET_FLAGS " -O2 ")
		set(CMAKE_C_COMPILER "$ENV{EMSCRIPTENDIR}/emcc")
		set(CMAKE_CXX_COMPILER "$ENV{EMSCRIPTENDIR}/em++")

		SET( COMMON_PARAMS " ${COMMON_PARAMS} -I$ENV{EMSCRIPTENDIR}/system/lib/libcxxabi/include/ -s USE_SDL=2 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=2 -s VERBOSE=1 ")
		
		set( POST_NAME ${POST_NAME}.html)
	
	endif()

	SET(CMAKE_C_FLAGS " ")
	SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
	
	
	MESSAGE ("Target : " ${TARGET})

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	
endif()




#
# RELEASE PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#


# ENGINE EXAMPLE
add_executable(sdl_console${POST_NAME}

	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/CConsole.cpp
	${PROJECT_SOURCE_DIR}/CImage.cpp
	${PROJECT_SOURCE_DIR}/CInput.cpp
	${PROJECT_SOURCE_DIR}/font_8x16.c
	${PROJECT_SOURCE_DIR}/CFont.cpp
	

)

# Library
if ( "${EMSCRIPTEN}" STREQUAL "yes" )

	set_target_properties(engine${POST_NAME} PROPERTIES LINK_FLAGS " -s USE_SDL=2 ")

	target_link_libraries(engine${POST_NAME} SDL2main SDL2)
 	
else()

	
	

	
	if (MINGW)
		find_library(SDL2_LIB  NAMES SDL2  )
		target_link_libraries(sdl_console${POST_NAME} mingw32  SDL2main SDL2)
	else()
		target_link_libraries(sdl_console${POST_NAME} SDL2main SDL2)
	endif()
	
endif()




