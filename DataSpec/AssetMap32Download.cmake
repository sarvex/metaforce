message(STATUS "32-bit asset name map not found; downloading to '${CMAKE_CURRENT_BINARY_DIR}/AssetNameMap32.bin'")
file(DOWNLOAD "https://www.dropbox.com/s/8rzkxstfap6hgi3/AssetNameMap32.dat"
     ${CMAKE_CURRENT_BINARY_DIR}/AssetNameMap32.bin SHOW_PROGRESS)