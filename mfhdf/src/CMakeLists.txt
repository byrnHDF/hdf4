cmake_minimum_required (VERSION 3.18)
project (HDF4_MFHDF_SRC C)

if (WIN32)
  set (H4_WINDOWS_INCLUDE_FILE "#include <hdf.h>")
endif ()

set (HDF4_MFHDF_SRC_CSRCS
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/array.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/attr.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/cdf.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/dim.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/file.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/hdf_xdr.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/hdfsds.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/iarray.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/error.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/globdef.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/mfdatainfo.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/mfsd.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/nssdc.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/putget.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/putgetg.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/sharray.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/string.c
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/var.c
)

set (HDF4_MFHDF_SRC_INSTALL_CHDRS
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/mfdatainfo.h
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/mfhdf.h
)

set (HDF4_MFHDF_SRC_CHDRS
    ${HDF4_MFHDF_SRC_INSTALL_CHDRS}
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/error_priv.h
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/hdf_xdr_priv.h
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/mf_priv.h
    ${HDF4_MFHDF_SRC_SOURCE_DIR}/nc_priv.h
)

if (BUILD_STATIC_LIBS)
  add_library (${HDF4_MF_LIB_TARGET} STATIC ${HDF4_MFHDF_SRC_CSRCS} ${HDF4_MFHDF_SRC_CHDRS})
  target_include_directories (${HDF4_MF_LIB_TARGET}
      PRIVATE "${HDF4_HDFSOURCE_DIR};${HDF4_MFHDFSOURCE_DIR};${HDF4_BINARY_DIR};${HDF4_COMP_INCLUDE_DIRECTORIES}"
      INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
  )
  target_compile_options(${HDF4_MF_LIB_TARGET} PRIVATE "${HDF4_CMAKE_C_FLAGS}")
  target_compile_definitions(${HDF4_MF_LIB_TARGET}
      PUBLIC
          ${HDF_EXTRA_C_FLAGS}
          ${HDF_EXTRA_FLAGS}
  )
  TARGET_C_PROPERTIES (${HDF4_MF_LIB_TARGET} STATIC)
  target_link_libraries (${HDF4_MF_LIB_TARGET} PUBLIC "$<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:MinGW>>:ws2_32.lib>")
  target_link_libraries (${HDF4_MF_LIB_TARGET} PUBLIC ${HDF4_SRC_LIB_TARGET})
  set_global_variable (HDF4_LIBRARIES_TO_EXPORT "${HDF4_LIBRARIES_TO_EXPORT};${HDF4_MF_LIB_TARGET}")
  H4_SET_LIB_OPTIONS (${HDF4_MF_LIB_TARGET} ${HDF4_MF_LIB_NAME} STATIC 0)
  set_target_properties (${HDF4_MF_LIB_TARGET} PROPERTIES
      FOLDER libraries
      COMPILE_DEFINITIONS "HDF"
      INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
  )
  set (install_targets ${HDF4_MF_LIB_TARGET})
endif ()

if (BUILD_SHARED_LIBS)
  add_library (${HDF4_MF_LIBSH_TARGET} SHARED ${HDF4_MFHDF_SRC_CSRCS} ${HDF4_MFHDF_SRC_CHDRS})
  target_include_directories (${HDF4_MF_LIBSH_TARGET}
      PRIVATE "${HDF4_HDFSOURCE_DIR};${HDF4_MFHDFSOURCE_DIR};${HDF4_BINARY_DIR};${HDF4_COMP_INCLUDE_DIRECTORIES}"
      INTERFACE "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
  )
  target_compile_options(${HDF4_MF_LIBSH_TARGET} PRIVATE "${HDF4_CMAKE_C_FLAGS}")
  target_compile_definitions(${HDF4_MF_LIBSH_TARGET}
      PUBLIC
          ${HDF_EXTRA_C_FLAGS}
          ${HDF_EXTRA_FLAGS}
  )
  TARGET_C_PROPERTIES (${HDF4_MF_LIBSH_TARGET} SHARED)
  target_link_libraries (${HDF4_MF_LIBSH_TARGET} PUBLIC "$<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:MinGW>>:ws2_32.lib>")
  target_link_libraries (${HDF4_MF_LIBSH_TARGET} PUBLIC ${HDF4_SRC_LIBSH_TARGET})
  set_global_variable (HDF4_LIBRARIES_TO_EXPORT "${HDF4_LIBRARIES_TO_EXPORT};${HDF4_MF_LIBSH_TARGET}")
  H4_SET_LIB_OPTIONS (${HDF4_MF_LIBSH_TARGET} ${HDF4_MF_LIB_NAME} SHARED "MF")
  set_target_properties (${HDF4_MF_LIBSH_TARGET} PROPERTIES
      FOLDER libraries
      COMPILE_DEFINITIONS "HDF;H4_BUILT_AS_DYNAMIC_LIB"
      INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
      INTERFACE_COMPILE_DEFINITIONS H4_BUILT_AS_DYNAMIC_LIB=1
  )
  set (install_targets ${install_targets} ${HDF4_MF_LIBSH_TARGET})
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
install (
    FILES
        ${HDF4_MFHDF_SRC_INSTALL_CHDRS}
    DESTINATION
        ${HDF4_INSTALL_INCLUDE_DIR}
    COMPONENT
        headers
)

#-----------------------------------------------------------------------------
# Add library to CMake Install : Installs lib and cmake config info
#-----------------------------------------------------------------------------
if (BUILD_SHARED_LIBS)
  INSTALL_TARGET_PDB (${HDF4_MF_LIBSH_TARGET} ${HDF4_INSTALL_BIN_DIR} libraries)
endif ()
if (BUILD_STATIC_LIBS)
  INSTALL_TARGET_PDB (${HDF4_MF_LIB_TARGET} ${HDF4_INSTALL_LIB_DIR} libraries)
endif ()


install (
    TARGETS
        ${install_targets}
    EXPORT
        ${HDF4_EXPORTED_TARGETS}
    LIBRARY DESTINATION ${HDF4_INSTALL_LIB_DIR} COMPONENT libraries
    ARCHIVE DESTINATION ${HDF4_INSTALL_LIB_DIR} COMPONENT libraries
    RUNTIME DESTINATION ${HDF4_INSTALL_BIN_DIR} COMPONENT libraries
    FRAMEWORK DESTINATION ${HDF4_INSTALL_FWRK_DIR} COMPONENT libraries
    INCLUDES DESTINATION include
)

#-----------------------------------------------------------------------------
# Create pkgconfig files
#-----------------------------------------------------------------------------
set (_PKG_CONFIG_PREFIX ${CMAKE_INSTALL_PREFIX})
set (_PKG_CONFIG_EXEC_PREFIX \${prefix})
set (_PKG_CONFIG_LIBDIR \${exec_prefix}/lib)
set (_PKG_CONFIG_INCLUDEDIR \${prefix}/include)
set (_PKG_CONFIG_LIBNAME "${HDF4_SRC_LIB_CORENAME}")
set (_PKG_CONFIG_VERSION "${HDF4_PACKAGE_VERSION}")
set (PKGCONFIG_LIBNAME "${HDF4_SRC_LIB_CORENAME}")
if (${HDF_CFG_NAME} MATCHES "Debug")
  set (PKGCONFIG_LIBNAME "${PKGCONFIG_LIBNAME}${CMAKE_DEBUG_POSTFIX}")
endif ()
set (MFPKGCONFIG_LIBNAME "${HDF4_MF_LIB_CORENAME}")
if (${HDF_CFG_NAME} MATCHES "Debug")
  set (MFPKGCONFIG_LIBNAME "${MFPKGCONFIG_LIBNAME}${CMAKE_DEBUG_POSTFIX}")
endif ()

foreach (libs ${LINK_LIBS})
  set (_PKG_CONFIG_LIBS_PRIVATE "${_PKG_CONFIG_LIBS_PRIVATE} -l${libs}")
endforeach ()

# The settings for the compression libs depends on if they have pkconfig support
# Assuming they don't
foreach (libs ${LINK_COMP_LIBS})
#  set (_PKG_CONFIG_REQUIRES_PRIVATE "${_PKG_CONFIG_REQUIRES_PRIVATE} -l${libs}")
  set (_PKG_CONFIG_LIBS_PRIVATE "${_PKG_CONFIG_LIBS_PRIVATE} -l${libs}")
endforeach ()
set (_PKG_CONFIG_REQUIRES_PRIVATE)

set (_PKG_CONFIG_LIBS "${_PKG_CONFIG_LIBS} -l${PKGCONFIG_LIBNAME} -l${MFPKGCONFIG_LIBNAME}")
if (BUILD_SHARED_LIBS)
  set (_PKG_CONFIG_SH_LIBS "${_PKG_CONFIG_SH_LIBS} -l${PKGCONFIG_LIBNAME} -l${MFPKGCONFIG_LIBNAME}")
endif ()

set (_PKG_CONFIG_REQUIRES)

configure_file (
    ${HDF_RESOURCES_DIR}/libhdf4.pc.in
    ${HDF4_BINARY_DIR}/CMakeFiles/${HDF4_SRC_LIB_CORENAME}.pc
    @ONLY
)
install (
    FILES ${HDF4_BINARY_DIR}/CMakeFiles/${HDF4_SRC_LIB_CORENAME}.pc
    DESTINATION ${HDF4_INSTALL_LIB_DIR}/pkgconfig
    COMPONENT libraries
)

if (NOT WIN32)
  set (_PKG_CONFIG_COMPILER ${CMAKE_C_COMPILER})
  configure_file (
      ${HDF_RESOURCES_DIR}/libh4cc.in
      ${HDF4_BINARY_DIR}/CMakeFiles/h4cc
      @ONLY
  )
  install (
      FILES ${HDF4_BINARY_DIR}/CMakeFiles/h4cc
      DESTINATION ${HDF4_INSTALL_BIN_DIR}
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      COMPONENT libraries
  )
endif ()

#-----------------------------------------------------------------------------
# Option to build documentation
#-----------------------------------------------------------------------------
if (DOXYGEN_FOUND)
  # This custom target and doxygen/configure work together
  # Replace variables inside @@ with the current values
  add_custom_target (hdf4mflib_doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${HDF4_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${HDF4_MFHDFSOURCE_DIR}
        COMMENT "Generating HDF4 library Source API documentation with Doxygen"
        VERBATIM )

  if (NOT TARGET doxygen)
    add_custom_target (doxygen)
  endif ()

  add_dependencies (doxygen hdf4mflib_doc)

endif ()
