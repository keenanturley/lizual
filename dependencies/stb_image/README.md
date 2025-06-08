# stb_image

Source: <https://github.com/nothings/stb/blob/master/stb_image.h>

Modifications

1. Added `stb_image.cpp` to enable the implementation as suggested by the documentation:

    ```cpp
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
    ```

1. Added CMake support so that I don't need to initialize the implementation in my own source files.
