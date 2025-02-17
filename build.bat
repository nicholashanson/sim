@echo off

set FILES="https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/profiler.hpp"
set FILES=%FILES% "https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/profile_manager.hpp"
set FILES=%FILES% "https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/performance_profile.hpp"
set FILES=%FILES% "https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/generate_graph.hpp"
set FILES=%FILES% "https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/fixture.hpp"
set FILES=%FILES% "https://raw.githubusercontent.com/nicholashanson/performance_profiler/refs/heads/main/average_time_profiler.hpp"

REM Loop through and download each file
for %%f in (%FILES%) do (
    echo Downloading %%f...
    curl -o "C:/Users/HP/Documents/sim/include/%%~nxf" %%f
    if errorlevel 1 (
        echo Failed to download %%f. Please check the URL or your internet connection.
        pause
        exit /b 1
    ) else (
        echo Successfully downloaded %%f.
    )
)

REM Check the first command-line argument
if "%1"=="--profile" (

    echo Compiling profiles
    g++ -g -v -std=c++23 -o sim_profile.exe ^
        "C:/Users/HP/Documents/sim/performance_profiling/profile_writing.cpp" ^
        "C:/Users/HP/Documents/sim/performance_profiling/profile_insert.cpp" ^
        -I"C:/Users/HP/Documents/sim/include" 

) else if "%1"=="--test" (
    echo Compiling tests...
    g++ -g -v -std=c++23 -o sim_test.exe ^
        "C:/Users/HP/Documents/sim/tests/test_row_reverse.cpp" ^
        -I"C:/Users/HP/Documents/sim/include" 
) 

pause