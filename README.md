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

# Run the app
# [Windows]
build/Debug/lizual.exe
# [Linux / MacOS]
build/Debug/lizual
```

## Acknowledgements

- [LearnOpenGL](https://learnopengl.com/)
- [TheCherno OpenGL Playlist](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
