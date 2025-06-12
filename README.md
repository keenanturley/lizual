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

## Dependencies

1. [GLAD](https://github.com/Dav1dde/glad)
1. [SDL3](https://github.com/libsdl-org/SDL)
1. [stb_image](https://github.com/libigl/libigl-stb/blob/master/stb_image.h)
1. [glm](https://github.com/g-truc/glm)

## Acknowledgements

These resources have helped me learn OpenGL:

- [LearnOpenGL](https://learnopengl.com/)
- [TheCherno OpenGL Playlist](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)
- [3DGEP - Understanding the View Matrix](https://www.3dgep.com/understanding-the-view-matrix/)
