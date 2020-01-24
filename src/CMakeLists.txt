project(BGMM)
cmake_minimum_required(VERSION 2.6)


#--------------------------------#
# Includes                       #
#--------------------------------#

if(APPLE)
  cmake_policy(SET CMP0042 NEW)
  include (${PROJECT_SOURCE_DIR}/AppleClang.cmake REQUIRED)
endif(APPLE)

