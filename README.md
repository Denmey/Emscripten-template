This template sets up a project for compiling C++ OpenGL code to the WebGL JS code that could be run on browsers using Emscripten.

For project to work properly, install:
1) Emscripten from https://emscripten.org/docs/getting_started/downloads.html;
2) Conan package manager https://conan.io/;
3) Download GLES2 includes from https://www.khronos.org/registry/OpenGL/index_es.php and put them in your compiler's includes directory;

Compile instructions:

1. Create subdirectory in your project, i.e. build

2.1. For compiling with emscripten, call ``emcmake cmake .. -DCMAKE_BUILD_TYPE=Debug`` and then ``emmake make``.

2.2. To compile for your home system (Windows/Linux/MacOS), just call ``cmake ..`` and ``make``.

TODO List:

* Add external resources support: textures, external shaders, model object files.

* Implement shader class that puts correct version of GLSL based on compiler used.

* Continuous integration?
