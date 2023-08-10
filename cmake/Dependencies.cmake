include(cmake/modules/CPM.cmake)

function(setup_dependencies)
  if(NOT TARGET spdlog::spdlog)
    CPMAddPackage(
      NAME spdlog
      VERSION 1.11.0
      GITHUB_REPOSITORY gabime/spdlog
      SYSTEM YES
    )
  endif()
endfunction()
