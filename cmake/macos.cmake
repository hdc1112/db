if (APPLE)
    execute_process(COMMAND bash -c "xcrun --show-sdk-path" OUTPUT_VARIABLE MACOS_SDK_PATH)
    string(STRIP ${MACOS_SDK_PATH} MACOS_SDK_PATH)
    set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
endif()