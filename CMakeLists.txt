CMAKE_MINIMUM_REQUIRED(VERSION 3.28)

PROJECT(
  LeapFrog
  LANGUAGES CXX
)

INCLUDE(cmake/base.cmake)

ADD_EXECUTABLE(leapfrog src/leapfrog.cc)
TARGET_INCLUDE_DIRECTORIES(leapfrog PRIVATE src)

ENABLE_TESTING()
ADD_EXECUTABLE(leapfrog_test src/leapfrog_test.cc)
TARGET_INCLUDE_DIRECTORIES(leapfrog_test PRIVATE src)
TARGET_LINK_LIBRARIES(leapfrog_test GTest::gtest_main)
INCLUDE(GoogleTest)
gtest_discover_tests(leapfrog_test)
