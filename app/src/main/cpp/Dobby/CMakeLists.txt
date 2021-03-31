cmake_minimum_required(VERSION 3.5)
project(Dobby)
enable_language(ASM)
include(cmake/Util.cmake)
include(cmake/Globals.cmake)
include(cmake/Macros.cmake)
include(cmake/XcodeGenerator.cmake)
include(cmake/AutoFiles.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

AutoFiles("." "CmakeSource" "\\.(cc|cpp|c|h)$")

# :< You Shall Not Pass!
if(0)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")

# ===== Handle Option =====
option(DOBBY_GENERATE_SHARED "Build shared library" ON)

option(DOBBY_DEBUG "Enable debug logging" ON)

option(NearBranch "Enable Near Branch Trampoline" ON)

option(DynamicBinaryInstrument "Enable Dynamic Binary Instrument" ON)

option(DBI.FullFloatingPointRegisterPack "Save and pack all floating-point registers" OFF)


option(Plugin.SymbolResolver "Resolve symbol by [DobbySymbolResolver] " OFF)

option(Plugin.LinkerLoadCallback "Register image load callback " OFF)

# frida is better choice
option(Plugin.ApplicationEventMonitor "Auto monitor linker, file, etc." OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DOBBY_DEBUG ON)
endif()
if(DOBBY_DEBUG)
  add_definitions(-DDOBBY_DEBUG)
  add_definitions(-DLOGGING_DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
  message(STATUS "[Dobby] Enable debug logging")
endif()

# Enable full floating point register pack
# for arm64, allow access q8 - q31
if(DBI.FullFloatingPointRegisterPack)
  add_definitions(-DFULL_FLOATING_POINT_REGISTER_PACK)
  message(STATUS "[Dobby] Save and pack all floating-point registers")
endif()


if(NOT DOBBY_DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--exclude-libs,ALL")
endif()
if(COMPILER.Clang)
  if(NOT DOBBY_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden")
  endif()
  if(PROCESSOR.ARM)
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -arch armv7 -x assembler-with-cpp")
  elseif(PROCESSOR.AARCH64)
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -arch arm64 -x assembler-with-cpp")
  endif()
endif()

# refer apple ld
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-unexported_symbol -Wl,_DobbyHook -Wl,-unexported_symbol -Wl,_DobbyInstrument -Wl,-unexported_symbol -Wl,_DobbySymbolResolver -Wl,-unexported_symbol -Wl,_svc_0x80_stub -Wl,-unexported_symbol -Wl,_svc_mprotect")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-s -Wl,-X -Wl,-dead_strip -Wl,-exported_symbol,_main")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")

# arch prefix
if(PROCESSOR.ARM)
  set(ARCH1 ARM)
  set(arch1 arm)
  set(core_arch arm)
elseif(PROCESSOR.AARCH64)
  set(ARCH1 ARM64)
  set(arch1 arm64)
  set(core_arch arm64)
elseif(PROCESSOR.X86)
  set(ARCH1 X86)
  set(arch1 x86)
  set(core_arch ia32)
elseif(PROCESSOR.X86_64)
  set(ARCH1 X64)
  set(arch1 x64)
  set(core_arch x64)
else()
endif()




set(dobby.SOURCE_FILE_LIST ${dobby.SOURCE_FILE_LIST}
  # cpu
  source/core/arch/CpuFeature.cc
  source/core/arch/CpuRegister.cc

  # assembler
  source/core/modules/assembler/assembler.cc
  source/core/modules/assembler/assembler-${core_arch}.cc

  # codegen
  source/core/modules/codegen/codegen-${core_arch}.cc

  # memory kit
  source/MemoryKit/CodeBuffer/CodeBufferBase.cc
  source/MemoryKit/CodeBuffer/code-buffer-${arch1}.cc
  source/MemoryKit/AssemblyCodeBuilder.cc
  source/MemoryKit/MemoryArena.cc

  # instruction relocation
  source/InstructionRelocation/${arch1}/${ARCH1}InstructionRelocation.cc

  # intercept routing
  source/InterceptRouting/InterceptRouting.cpp

  # intercept routing trampoline
  source/TrampolineKit/InterceptRoutingTrampoline/${arch1}/trampoline-${arch1}.cc

  # intercept routing plugin (buildin)
  source/InterceptRouting/RoutingPlugin/FunctionInlineReplace/function-inline-replace.cc
  source/InterceptRouting/RoutingPlugin/FunctionInlineReplace/FunctionInlineReplaceExport.cc

  # plugin register
  source/InterceptRouting/ExtraInternalPlugin/RegisterPlugin.cc

  # platform util
  source/UserMode/PlatformUtil/ProcesssRuntimeUtility.cc

  # user mode - platform interface
  source/UserMode/UnifiedInterface/platform-posix.cc

  # user mode - executable memory
  source/UserMode/ExecMemory/code-patch-tool-posix.cc
  source/UserMode/ExecMemory/clear-cache-tool-all.cc

  # main
  source/dobby.cpp
  source/Interceptor.cpp
  )





if(FunctionWrapper OR DynamicBinaryInstrument)
  set(dobby.SOURCE_FILE_LIST ${dobby.SOURCE_FILE_LIST}
    # closure trampoline bridge
    source/TrampolineKit/ClosureTrampolineBridge/closure-trampoline-common-handler/closure-trampoline-common-handler.cc
    source/TrampolineKit/ClosureTrampolineBridge/${arch1}/helper-${arch1}.cc
    source/TrampolineKit/ClosureTrampolineBridge/${arch1}/closure-bridge-${arch1}.cc
    source/TrampolineKit/ClosureTrampolineBridge/${arch1}/${ARCH1}AssemblyClosureTrampoline.cc
    )
endif()

if(FunctionWrapper)
  message(FATAL_ERROR "[!] FunctionWrapper plugin is not supported")
endif()

if(DynamicBinaryInstrument)
  message(STATUS "[Dobby] Enable Dynamic Binary Instrument allow you instrument at any instruction")
  set(dobby.SOURCE_FILE_LIST ${dobby.SOURCE_FILE_LIST}
    source/InterceptRouting/RoutingPlugin/DynamicBinaryInstrument/dynamic-binary-instrument.cc
    source/InterceptRouting/RoutingPlugin/DynamicBinaryInstrument/DynamicBinaryInstrumentExport.cc
    source/InterceptRouting/RoutingPlugin/DynamicBinaryInstrument/intercept_routing_handler.cc
    )
endif()

if(NearBranch)
  message(STATUS "[Dobby] Enable Near Branch Trampoline")
  set(dobby.SOURCE_FILE_LIST ${dobby.SOURCE_FILE_LIST}
    source/InterceptRouting/ExtraInternalPlugin/NearBranchTrampoline/NeaBranchTrampoline.cc
    source/InterceptRouting/ExtraInternalPlugin/NearBranchTrampoline/NearMemoryArena.cc)
endif()

# add logging library
# add_subdirectory(external/logging)
set(logging.SOURCE_FILE_LIST
  external/logging/logging.c
  external/logging/cxxlogging.cc
  )

# add xnucxx library
# add_subdirectory(external/xnucxx)
set(xnucxx.SOURCE_FILE_LIST
  external/xnucxx/LiteObject.cc
  external/xnucxx/LiteMemOpt.cc
  external/xnucxx/LiteCollection.cc
  external/xnucxx/LiteIterator.cc
  external/xnucxx/LiteMutableArray.cc
  external/xnucxx/LiteMutableBuffer.cc
  )

set(dobby.plugin.SOURCE_FILE_LIST)


if(Plugin.HideSystemCall)
  set(dobby.plugin.SOURCE_FILE_LIST ${dobby.plugin.SOURCE_FILE_LIST}
    )
endif()



include_directories(
  .
  ./source
  ./source/UserMode
  ./source/MemoryKit
  ./source/TrampolineKit

  ./external
  ./external/logging
  ./external/xnucxx
)

set(dobby.HEADER_FILE_LIST
  include/dobby.h
)

# add build version
string(TIMESTAMP TODAY "%Y%m%d")
set(VERSION_REVISION "-${TODAY}")

set(DOBBY_BUILD_VERSION "Dobby${VERSION_REVISION}")
message(STATUS "Version: ${DOBBY_BUILD_VERSION}")
add_definitions(-D__DOBBY_BUILD_VERSION__="${DOBBY_BUILD_VERSION}")

set(dobby_name dobby)

if(DOBBY_GENERATE_SHARED)
  add_library(${dobby_name} SHARED ${dobby.HEADER_FILE_LIST} ${dobby.SOURCE_FILE_LIST} ${logging.SOURCE_FILE_LIST} ${xnucxx.SOURCE_FILE_LIST} ${dobby.plugin.SOURCE_FILE_LIST})
else()
  add_library(${dobby_name} STATIC ${dobby.HEADER_FILE_LIST} ${dobby.SOURCE_FILE_LIST} ${logging.SOURCE_FILE_LIST} ${xnucxx.SOURCE_FILE_LIST} ${dobby.plugin.SOURCE_FILE_LIST})
endif()

target_include_directories(${dobby_name} PUBLIC include)

if(SYSTEM.Android)
  target_link_libraries(dobby log)
endif()


