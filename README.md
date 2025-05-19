# lizual
C++ Graphics Sandbox

## Installation & Running
```sh
# Clone the repo and the submodules
git clone --recurse-submodules https://github.com/keenanturley/lizual.git
cd lizual

# Initialize CMake where -S is the source folder and -B is the build folder
cmake -S . -B build

# Build the executable
cmake --build build

# [Windows] Run the app
build/Debug/lizual.exe
```