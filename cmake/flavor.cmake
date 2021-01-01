# All of compile flags are listed here
# Their values are decided by the flavor
set(DEBUG_ASSERT_CRASH_IF_FAIL true)
set(RELEASE_ASSERT_CRASH_IF_FAIL false)

# The default flavor
if (NOT FLAVOR)
    set(FLAVOR "DEBUG")
endif()

# Assign values to the compile flags
if (FLAVOR STREQUAL "DEBUG")
    set(DEBUG_ASSERT_CRASH_IF_FAIL true)
    set(RELEASE_ASSERT_CRASH_IF_FAIL true)
elseif (FLAVOR STREQUAL "RELEASE")
    set(DEBUG_ASSERT_CRASH_IF_FAIL false)
    set(RELEASE_ASSERT_CRASH_IF_FAIL true)
else()
    message(FATAL_ERROR "Invalid flavor: [${FLAVOR}]")
endif()
