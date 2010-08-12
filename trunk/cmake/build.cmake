macro (te_exec_defs base_name)
  string (TOUPPER ${base_name} defname)
  
  add_definitions (${myDEFS})
  include_directories (
    ${TE_SRC_ROOT}
    ${TE_CONFIG_DIR}
    ${TE_INCLUDES} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro ()
# -----------------------------------------------------------------------
macro (te_shared_defs base_name)
  string (TOUPPER ${base_name} defname)
  
  list (APPEND myDEFS "-DBUILD_${defname}")
  add_definitions (${myDEFS})
  include_directories (
    ${TE_SRC_ROOT}
    ${TE_CONFIG_DIR}
    ${TE_INCLUDES} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

endmacro ()
# -----------------------------------------------------------------------
macro (te_static_defs base_name)
  string (TOUPPER ${base_name} defname)
  
  list (APPEND myDEFS "-D${defname}_AS_STATIC_LIB=1")
  add_definitions (${myDEFS})
  include_directories (
    ${TE_SRC_ROOT} 
    ${TE_CONFIG_DIR}
    ${TE_INCLUDES} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro ()
# -----------------------------------------------------------------------
macro (te_shared_lib base_name sources headers)
  te_shared_defs (${base_name})
  set (outname ${base_name})
  add_library (${outname} SHARED ${${sources}} ${${headers}})
  target_link_libraries (${outname} ${ARGN} ${TE_LIBS})
  
  install(TARGETS ${outname} ${ARGN}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

  file (RELATIVE_PATH INC_NAME ${TE_SRC_ROOT} ${CMAKE_CURRENT_SOURCE_DIR})

  foreach (f ${${headers}})
    get_filename_component (pth ${f} PATH)
    if (NOT pth)
      set (i_oname include/${PROJECT_NAME}/${INC_NAME})
    else ()
      set (i_oname include/${PROJECT_NAME}/${INC_NAME}/${pth})
    endif ()
    install (FILES ${f} DESTINATION ${i_oname})
  endforeach ()
endmacro ()
# -------------------------------------------------------------------
macro (te_static_lib base_name sources headers)
  te_static_defs (${base_name})
  set (outname ${base_name})
  add_library (${outname} STATIC ${${sources}} ${${headers}})
endmacro ()
# -------------------------------------------------------------------
macro (te_exec base_name sources headers)
  te_exec_defs (${base_name})
  set (outname ${base_name})
  add_executable (${outname} ${${sources}} ${${headers}})
  target_link_libraries (${outname} ${ARGN} ${TE_LIBS})

  install(TARGETS ${outname} 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)
endmacro ()
