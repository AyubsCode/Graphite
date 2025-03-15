# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/zeila/Documents/CMPT496/Graphite/esp-idf/components/bootloader/subproject"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/tmp"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/src/bootloader-stamp"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/src"
  "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/zeila/Documents/CMPT496/esp_server/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
