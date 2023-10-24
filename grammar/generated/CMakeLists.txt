
# make glob of all files in directory
file(GLOB_RECURSE SOURCES *.cpp)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${SOURCES}
)