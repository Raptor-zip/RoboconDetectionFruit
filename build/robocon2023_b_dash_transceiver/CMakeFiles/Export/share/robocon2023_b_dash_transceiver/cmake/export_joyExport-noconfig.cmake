#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "robocon2023_b_dash_transceiver::joy" for configuration ""
set_property(TARGET robocon2023_b_dash_transceiver::joy APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(robocon2023_b_dash_transceiver::joy PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libjoy.so"
  IMPORTED_SONAME_NOCONFIG "libjoy.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS robocon2023_b_dash_transceiver::joy )
list(APPEND _IMPORT_CHECK_FILES_FOR_robocon2023_b_dash_transceiver::joy "${_IMPORT_PREFIX}/lib/libjoy.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
