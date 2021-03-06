# Header-only
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO cpsusie/cjm-numerics
    REF v1.0.0.0
    SHA512 0529486d4fc72f39d304b78075b59e904433182eb3f19e835e0cf961be28b57715f8dd766ab146c51ab080b86e034b06a48952194c36e09b5b8e65a979df4122
    HEAD_REF main
)
set(CMAKE_CXX_STANDARD 20)
vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}/src/
    PREFER_NINJA
)
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/)
file(INSTALL ${SOURCE_PATH}/src/include/ DESTINATION ${CURRENT_PACKAGES_DIR}/include/)
file(INSTALL ${SOURCE_PATH}/src/LICENSE.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
file(INSTALL ${SOURCE_PATH}/src/NOTICES.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
file(INSTALL ${SOURCE_PATH}/src/NOTICES.md DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})
file(INSTALL ${SOURCE_PATH}/README.md DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT})



