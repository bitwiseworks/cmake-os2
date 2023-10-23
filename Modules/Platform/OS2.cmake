# set the platform tag
SET(OS2 1)

# also add the install directory of the running cmake to the search directories
# CMAKE_ROOT is CMAKE_INSTALL_PREFIX/share/cmake, so we need to go two levels up
get_filename_component(_CMAKE_INSTALL_DIR "${CMAKE_ROOT}" PATH)
get_filename_component(_CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" PATH)

# List common installation prefixes.  These will be used for all
# search types.
list(APPEND CMAKE_SYSTEM_PREFIX_PATH
  # Standard
  /@unixroot/usr/local /@unixroot/usr

  # CMake install location
  "${_CMAKE_INSTALL_DIR}"
  )

list(APPEND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
  /@unixroot/usr/lib
  )

list(APPEND CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES
  /@unixroot/usr/include
  )
list(APPEND CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
  /@unixroot/usr/include
  )

SET(CMAKE_DL_LIBS "" )

# resource compiler
# to add a icon to the exe do the following in a CMakeLists.txt file
# if(OS2)
#   enable_language(RC)
#   set_source_files_properties(yourrcfile.rc PROPERTIES LANGUAGE RC)
#   set(project_SOURCES ${project_SOURCES}
#    yourrcfile.rc
#   )
# endif()
#
set(CMAKE_RC_COMPILER_INIT "wrc" )
set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -q -r <SOURCE> <INCLUDES> -fo=<OBJECT>")
set(CMAKE_INCLUDE_FLAG_RC "-i=")

# set flags for shared libraries
SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-Zdll")
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
SET(CMAKE_SHARED_LIBRARY_C_FLAGS " ")
SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS " ")
SET(CMAKE_SHARED_LINKER_FLAGS_INIT "-Zomf -Zdll -Zlinker DISABLE -Zlinker 1121")
# Shared libraries on OS/2 are named with their version number.
SET(CMAKE_SHARED_LIBRARY_NAME_WITH_VERSION 1)

# set flags for modules (almost as shared libraries)
SET(CMAKE_SHARED_MODULE_CREATE_C_FLAGS ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS})
SET(CMAKE_SHARED_MODULE_PREFIX "")
SET(CMAKE_SHARED_MODULE_SUFFIX ".dll")
SET(CMAKE_MODULE_LINKER_FLAGS_INIT "-Zomf -Zdll -Zlinker DISABLE -Zlinker 1121")
# Modules have a different default prefix than shared libs.
SET(CMAKE_MODULE_EXISTS 1)

# set flags for import libraries
SET(CMAKE_IMPORT_LIBRARY_PREFIX "")
SET(CMAKE_IMPORT_LIBRARY_SUFFIX "_dll.a")

# set flags for static libraries
SET(CMAKE_STATIC_LIBRARY_PREFIX "")
SET(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

# set flags for executables
SET(CMAKE_EXECUTABLE_SUFFIX ".exe")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Zomf -Zlinker DISABLE -Zlinker 1121")

# set compiler options
SET(CMAKE_C_COMPILE_OPTIONS_PIC "")
SET(CMAKE_CXX_COMPILE_OPTIONS_PIC "")
SET(CMAKE_C_COMPILE_OPTIONS_PIE "")
SET(CMAKE_CXX_COMPILE_OPTIONS_PIE "")
SET(CMAKE_C_OUTPUT_EXTENSION ".o")
SET(CMAKE_CXX_OUTPUT_EXTENSION ".o")
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)

# set linker options
SET(CMAKE_C_LINK_FLAGS "-Zomf")
SET(CMAKE_CXX_LINK_FLAGS "-Zomf")

SET(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" "_dll.a" ".dll" ".a")


SET(CMAKE_C_CREATE_SHARED_MODULE
  "<CMAKE_C_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_MODULE_C_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_MODULE_CREATE_C_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES>")
SET(CMAKE_CXX_CREATE_SHARED_MODULE
  "<CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_MODULE_CXX_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES>")

# When setting the below properties in the CMakeList.txt, a nice buildlevel
# string is written to the exe or the dll. If the properties are not set,
# the settings are filled with defaults.
# To enable them in the CMakeList.txt use the below snippet as a reference.
# If there is no VERSION or VERSION_PATCH set in CMakeList.txt, either remove
# this definition, or set it to your liking.
# It's important that you add this snippet after the target is created and of
# course <TARGET> needs to be changed to the real target
#
# With the TARGET_SHORT you can create a shorter name for the dll
#
# The 2 settings with OS2_DEF_EXE_ are for executables only.
# Usually the Stack value is not needed, so omit it please.
#
# if(OS2)
#   set_target_properties(<TARGET> PROPERTIES
#    OS2_DEF_VENDOR "whoever you are"
#    OS2_DEF_PATCH "${VERSION_PATCH}"
#    OS2_DEF_VERSION "${VERSION}"
#    TARGET_SHORT shortn
#    OS2_DEF_EXEType "WINDOWAPI"
#    OS2_DEF_EXEStack "STACKSIZE 0x8000")
# endif()
#

# create the timestamp and build maschine name
string(TIMESTAMP TSbldLevel "%d %b %Y %H:%M:%S")
exec_program(uname ARGS -n OUTPUT_VARIABLE unamebldLevel)
SET(bldLevelInfo "\#\#1\#\# ${TSbldLevel}\\ \\ \\ \\ \\ ${unamebldLevel}")

# there is the possibility to rely on EMXEXP instead of dllexport on CXX
# the OS2_USE_CXX_EMXEXP is meant for that. use it as -DOS2_USE_CXX_EMXEXP=ON
# it migh be necessary to add the same for C as well later on. who knows

SET(CMAKE_C_CREATE_SHARED_LIBRARY
  "echo LIBRARY \\\"<OS2_DEF_TARGET>\\\" INITINSTANCE TERMINSTANCE > <TARGET_BASE>.def && echo DESCRIPTION \\\"@\#<OS2_DEF_VENDOR>:<OS2_DEF_VERSION>\#@${bldLevelInfo}::::<OS2_DEF_PATCH>::@@<TARGET_NAME>\\\" >> <TARGET_BASE>.def && echo DATA MULTIPLE NONSHARED >> <TARGET_BASE>.def && echo EXPORTS >> <TARGET_BASE>.def && emxexp <OBJECTS> >> <TARGET_BASE>.def && <CMAKE_C_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_C_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES> <TARGET_BASE>.def && emximp -o <TARGET_IMPLIB> <TARGET_BASE>.def")

if(OS2_USE_CXX_EMXEXP)
SET(CMAKE_CXX_CREATE_SHARED_LIBRARY
  "echo LIBRARY \\\"<OS2_DEF_TARGET>\\\" INITINSTANCE TERMINSTANCE > <TARGET_BASE>.def && echo DESCRIPTION \\\"@\#<OS2_DEF_VENDOR>:<OS2_DEF_VERSION>\#@${bldLevelInfo}::::<OS2_DEF_PATCH>::@@<TARGET_NAME>\\\" >> <TARGET_BASE>.def && echo DATA MULTIPLE NONSHARED >> <TARGET_BASE>.def && echo EXPORTS >> <TARGET_BASE>.def && emxexp <OBJECTS> >> <TARGET_BASE>.def && <CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES> <TARGET_BASE>.def && emximp -o <TARGET_IMPLIB> <TARGET_BASE>.def")
else()
SET(CMAKE_CXX_CREATE_SHARED_LIBRARY
  "echo LIBRARY \\\"<OS2_DEF_TARGET>\\\" INITINSTANCE TERMINSTANCE > <TARGET_BASE>.def && echo DESCRIPTION \\\"@\#<OS2_DEF_VENDOR>:<OS2_DEF_VERSION>\#@${bldLevelInfo}::::<OS2_DEF_PATCH>::@@<TARGET_NAME>\\\" >> <TARGET_BASE>.def && echo DATA MULTIPLE NONSHARED >> <TARGET_BASE>.def && <CMAKE_CXX_COMPILER> <LANGUAGE_COMPILE_FLAGS> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <OBJECTS> <LINK_LIBRARIES> <TARGET_BASE>.def && emximp -o <TARGET_IMPLIB> <TARGET>")
endif(OS2_USE_CXX_EMXEXP)

SET(CMAKE_C_LINK_EXECUTABLE
  "echo NAME \\\"<TARGET_NAME>\\\" <OS2_DEF_EXEType> > <TARGET_BASE>.def && echo DESCRIPTION \\\"@\#<OS2_DEF_VENDOR>:<OS2_DEF_VERSION>\#@${bldLevelInfo}::::<OS2_DEF_PATCH>::@@<TARGET_NAME>\\\" >> <TARGET_BASE>.def && echo <OS2_DEF_EXEStack> >> <TARGET_BASE>.def && <CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <LINK_LIBRARIES> <TARGET_BASE>.def")
SET(CMAKE_CXX_LINK_EXECUTABLE
  "echo NAME \\\"<TARGET_NAME>\\\" <OS2_DEF_EXEType> > <TARGET_BASE>.def && echo DESCRIPTION \\\"@\#<OS2_DEF_VENDOR>:<OS2_DEF_VERSION>\#@${bldLevelInfo}::::<OS2_DEF_PATCH>::@@<TARGET_NAME>\\\" >> <TARGET_BASE>.def && echo <OS2_DEF_EXEStack> >> <TARGET_BASE>.def && <CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> ${CMAKE_GNULD_IMAGE_VERSION} <LINK_LIBRARIES> <TARGET_BASE>.def")

# Initialize C link type selection flags.  These flags are used when
# building a shared library, shared module, or executable that links
# to other libraries to select whether to use the static or shared
# versions of the libraries.
FOREACH(type SHARED_LIBRARY SHARED_MODULE EXE)
  SET(CMAKE_${type}_LINK_STATIC_C_FLAGS "")
  SET(CMAKE_${type}_LINK_DYNAMIC_C_FLAGS "")
ENDFOREACH(type)

IF(CMAKE_COMPILER_IS_GNUCC)
  SET (CMAKE_C_FLAGS_DEBUG_INIT "-g")
  SET (CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
  SET (CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
  SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
  SET (CMAKE_C_CREATE_PREPROCESSED_SOURCE "<CMAKE_C_COMPILER> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")
  SET (CMAKE_C_CREATE_ASSEMBLY_SOURCE "<CMAKE_C_COMPILER> <FLAGS> -S <SOURCE> -o <ASSEMBLY_SOURCE>")
  IF(NOT APPLE)
    SET (CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
  ENDIF(NOT APPLE)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
  SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
  SET (CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
  SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
  SET (CMAKE_CXX_CREATE_PREPROCESSED_SOURCE "<CMAKE_CXX_COMPILER> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")
  SET (CMAKE_CXX_CREATE_ASSEMBLY_SOURCE "<CMAKE_CXX_COMPILER> <FLAGS> -S <SOURCE> -o <ASSEMBLY_SOURCE>")
  IF(NOT APPLE)
    SET (CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
  ENDIF(NOT APPLE)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_SIZEOF_VOID_P 4)
