# Auto-increment build version script
# This script reads version.txt, increments the patch version, and writes it back

file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

# Parse version (format: MAJOR.MINOR.PATCH)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" VERSION_MATCH "${VERSION_STRING}")
if(VERSION_MATCH)
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    set(VERSION_MINOR ${CMAKE_MATCH_2})
    set(VERSION_PATCH ${CMAKE_MATCH_3})
    
    # Increment patch version
    math(EXPR VERSION_PATCH "${VERSION_PATCH} + 1")
    
    # Write new version back to file
    set(NEW_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    file(WRITE "${CMAKE_SOURCE_DIR}/version.txt" "${NEW_VERSION}")
    
    message(STATUS "Version incremented to ${NEW_VERSION}")
else()
    message(WARNING "Invalid version format in version.txt: ${VERSION_STRING}")
endif()
