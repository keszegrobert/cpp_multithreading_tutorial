cmake_minimum_required(VERSION 3.15)
project(ParallelProgrammingTutorial CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# TBB keresése, mivel a legtöbb implementáció (GCC, Clang/LLVM) ezt használja backendként
# macOS-en: brew install tbb
find_package(TBB QUIET)

if(NOT TBB_FOUND)
    # Homebrew TBB path hint
    if(EXISTS "/opt/homebrew/opt/tbb")
        set(TBB_ROOT "/opt/homebrew/opt/tbb")
        list(APPEND CMAKE_PREFIX_PATH ${TBB_ROOT})
        include_directories(${TBB_ROOT}/include)
        link_directories(${TBB_ROOT}/lib)
    endif()
    
    # Retry finding TBB
    find_package(TBB QUIET)
    
    if(NOT TBB_FOUND)
        message(STATUS "TBB package not found via CMake config. Will try linking 'tbb' manually if needed.")
        message(WARNING "
    -----------------------------------------------------------------------
    TBB (Threading Building Blocks) not found!
    
    Many compilers (GCC, Clang) require TBB for C++17 parallel algorithms.
    If the next check fails, please install TBB.
    - Ubuntu/Debian: sudo apt install libtbb-dev
    - macOS: brew install tbb
    -----------------------------------------------------------------------")
    endif()
endif()

# Check for C++17 Parallel Algorithms support
include(CheckCXXSourceCompiles)

# Temporary set CMAKE_REQUIRED_FLAGS for the check
set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++17")

# If TBB was found, we should include it in the check to give it a chance to pass
if(TBB_FOUND)
    set(SAFE_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
    set(SAFE_CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
    set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    
    # Check for TBB location usage (if manually set)
    if(DEFINED TBB_ROOT)
        list(APPEND CMAKE_REQUIRED_INCLUDES "${TBB_ROOT}/include")
        # Pass linker flag directly strictly for the check
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -L${TBB_ROOT}/lib")
    elseif(DEFINED TBB_INCLUDE_DIRS)
         list(APPEND CMAKE_REQUIRED_INCLUDES ${TBB_INCLUDE_DIRS})
    endif()
    
    # Link against 'tbb' library directly, avoiding "TBB::tbb" imported target visibility issues in try_compile
    list(APPEND CMAKE_REQUIRED_LIBRARIES "tbb")
endif()

# Check if TBB is needed for the check to pass (common for GCC/Clang)
# We will do a full link check
check_cxx_source_compiles("
#include <vector>
#include <algorithm>
#include <execution>
int main() {
    std::vector<int> v = {1, 2, 3};
    std::sort(std::execution::par, v.begin(), v.end());
    return 0;
}
" HAS_CXX17_PARALLEL_ALGORITHMS)

set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
if(TBB_FOUND)
    set(CMAKE_REQUIRED_LIBRARIES "${SAFE_CMAKE_REQUIRED_LIBRARIES}")
    set(CMAKE_REQUIRED_INCLUDES "${SAFE_CMAKE_REQUIRED_INCLUDES}")
endif()

if(NOT HAS_CXX17_PARALLEL_ALGORITHMS)
    message(FATAL_ERROR "
    -----------------------------------------------------------------------
    ERROR: C++17 Parallel Algorithms (std::execution::par) support is REQUIRED.
    -----------------------------------------------------------------------")
endif()

# JSON library setup
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

# Segédfüggvény a targetek egyszerűbb hozzáadásához
function(add_tutorial_executable name source_file)
    add_executable(${name} ${source_file})
    
    if(TBB_FOUND)
        target_link_libraries(${name} PRIVATE TBB::tbb)
    else()
        # Megpróbáljuk direktben linkelni, hátha a rendszer útvonalon elérhető
        target_link_libraries(${name} PRIVATE tbb)
    endif()
    
    # Linux/GCC esetén szükség lehet a -ltbb mellett a -pthread-re is
    find_package(Threads REQUIRED)
    target_link_libraries(${name} PRIVATE Threads::Threads)
endfunction()

# 1. Magas Szint (High-Level Abstractions)
add_subdirectory(01_High_Level)


# 2. Közepes Szint (Mid-Level Coordination)
add_subdirectory(02_Mid_Level)

# 3. Alacsony Szint (Low-Level Synchronization)
add_subdirectory(03_Low_Level)

# 4. Hardver és Optimalizáció (Hardware & Optimization)
add_subdirectory(04_Hardware_and_Optimization)
add_subdirectory(05_jsonrpc_actorlike_sample)
