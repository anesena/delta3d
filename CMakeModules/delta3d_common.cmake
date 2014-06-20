# This defines the follow variables
# DELTA3D_ROOT : The root of the Delta3D directory
# DELTA3D_INCLUDE_DIR : The Delta3D include directory
# DELTA3D_EXT_DIR : The directory that contains the Delta3D external dependency
#
# This folder contains some functions which helps find the indidivual parts of Delta3D
# and is typically included by other .cmake files.


if (NOT DT_COMMON_RUN)

set(DT_COMMON_RUN "true")

#where to find the root Delta3D folder
FIND_PATH(DELTA3D_ROOT src
          HINTS
          $ENV{DELTA_ROOT}
          ${CMAKE_SOURCE_DIR}
          DOC "The root folder of Delta3D"
          )

#where to find the Delta3D include dir
FIND_PATH(DELTA3D_INCLUDE_DIR dtCore/dt.h
         HINTS
         ${DELTA3D_ROOT}/inc
         $ENV{DELTA_ROOT}/inc
         PATHS
         /usr/local/include
         /usr/freeware/include     
         DOC "The Delta3D include folder. Should contain 'dtCore', 'dtUtil', 'dtABC',..."
)

#where to find the Delta3D "ext" folder.  Look for one of the headers that might be in there.
SET(DELTA3D_EXT_DIR ${DELTA3D_ROOT}/ext CACHE PATH "The root of the Delta3D external dependency folder")

IF(DELTA3D_EXT_DIR)
  #for aiding FIND_FILE() and FIND_PATH() searches
  SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${DELTA3D_EXT_DIR})
  SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${DELTA3D_EXT_DIR}/inc)
ENDIF(DELTA3D_EXT_DIR)

#where to find the Delta3D lib dir
SET(DELTA3D_LIB_SEARCH_PATH 
             ${DELTA3D_ROOT}/lib
             ${DELTA3D_ROOT}/build/lib
             $ENV{DELTA_LIB}
             $ENV{DELTA_ROOT}/lib
             $ENV{DELTA_ROOT}/build/lib
             ${DELTA3D_LIB_DIR}
             /usr/local/lib
             /usr/lib
)

MACRO(FIND_DELTA3D_LIBRARY LIB_VAR LIB_NAME)
  FIND_LIBRARY(${LIB_VAR} NAMES ${LIB_NAME}
               HINTS
               ${DELTA3D_LIB_SEARCH_PATH}
              )
ENDMACRO(FIND_DELTA3D_LIBRARY LIB_VAR LIB_NAME)            



#
# DELTA_FIND_PATH
#
function(DELTA3D_FIND_PATH module header)
   string(TOUPPER ${module} module_uc)

   # Try the user's environment request before anything else.
   find_path(${module_uc}_INCLUDE_DIR ${header}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES include inc
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr
            /usr/include
            /usr/local/include
            ${DELTA3D_EXT_DIR} #defined in delta3d_common.cmake
            $ENV{OSG_DIR}
            $ENV{OSG_ROOT}

   )
endfunction(DELTA3D_FIND_PATH module header)


#
# DELTA3D_FIND_LIBRARY
#
function(DELTA3D_FIND_LIBRARY module library)
   string(TOUPPER ${module} module_uc)
   find_library(${module_uc}_LIBRARY
       NAMES ${library}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES lib64 lib
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr/lib
            ${DELTA3D_EXT_DIR}/lib
            ${DELTA3D_EXT_DIR}/lib64
            $ENV{DELTA_ROOT}/ext/lib
            $ENV{DELTA_ROOT}/ext/lib64
            $ENV{OSG_DIR}/build
            $ENV{OSG_ROOT}/build
            $ENV{OSG_DIR}
            $ENV{OSG_ROOT}
   )

   #Modify each entry to tack on "d" and "_d" for the debug file name
   FOREACH(debug_lib ${library})
     LIST(APPEND debug_list ${debug_lib}d ${debug_lib}_d ${debug_lib}_debug)
   ENDFOREACH(debug_lib ${library})
    
   find_library(${module_uc}_LIBRARY_DEBUG
       NAMES ${debug_list}
       HINTS
            $ENV{${module_uc}_DIR}
            $ENV{DELTA_ROOT}
       PATH_SUFFIXES lib64 lib
       PATHS
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            /usr/freeware
            /usr/lib
            ${DELTA3D_EXT_DIR}/lib
            ${DELTA3D_EXT_DIR}/lib64
            $ENV{DELTA_ROOT}/ext/lib
            $ENV{DELTA_ROOT}/ext/lib64
            $ENV{OSG_DIR}/build
            $ENV{OSG_ROOT}/build  
            $ENV{OSG_DIR}
            $ENV{OSG_ROOT}
    )

   if(NOT ${module_uc}_LIBRARY_DEBUG)
      # They don't have a debug library
      set(${module_uc}_LIBRARY_DEBUG ${${module_uc}_LIBRARY} PARENT_SCOPE)
      set(${module_uc}_LIBRARIES ${${module_uc}_LIBRARY} PARENT_SCOPE)
      MESSAGE(STATUS "No debug library was found for ${module_uc}_LIBRARY_DEBUG")
   else()
      # They really have a FOO_LIBRARY_DEBUG
      set(${module_uc}_LIBRARIES 
          optimized ${${module_uc}_LIBRARY}
          debug ${${module_uc}_LIBRARY_DEBUG}
          PARENT_SCOPE
      )
   endif()
endfunction(DELTA3D_FIND_LIBRARY module library)

MACRO (SETUP_PLUGIN_OUTPUT_DIRS SUBFOLDER)
    #put the binary into a "STAGE plugins" folder
    if (WIN32)
       # This design only makes sense on windows because there is no bin dir for libraries.
       SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/${SUBFOLDER})
       SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
    else()
       SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib/${SUBFOLDER})
    endif()
    
    SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR})
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
    IF(WIN32)
      SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
    ELSE(WIN32)
      SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR}) 
    ENDIF(WIN32)
    
    # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses) 
    FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
       # Go uppercase (DEBUG, RELEASE...)
       STRING(TOUPPER "${CONF}" CONF)
        
       #Handle the debug differently by putting the output into a debug subfolder
       IF (${CONF} STREQUAL "DEBUG")
         SET(DEBUG_FOLDER "Debug")
       ELSE (${CONF} STREQUAL "DEBUG")
         SET(DEBUG_FOLDER "")
       ENDIF (${CONF} STREQUAL "DEBUG")

       SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
       SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
       IF(WIN32)
         SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
       ELSE()
         SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
       ENDIF()
    ENDFOREACH()
ENDMACRO()

endif()