# tinygl-cpp

Single header, cross-platform openGL framework for drawing 2D squares, circles, and triangles.

# Building

This project relies on [OpenGL 4.0](https://www.khronos.org/registry/OpenGL-Refpages/gl4/), [GLEW](http://glew.sourceforge.net/), and [GLFW](https://www.glfw.org/) builds using CMake. 

**Windows**

Using [Visual Studio Community 2019](https://visualstudio.microsoft.com/vs/community/), you can generate the build files from the `Git bash` terminal.

```
$ git clone <this repository>
$ cd tinygl-cpp
$ mkdir build
$ cd build
$ cmake ..
$ start tinygl-cpp.sln
```

The final command opens visual studio, where you can build and run the same program. The dependencies (GLEW and GLFW) are included in this repository.

**MacOS**

On macOS, you should have git and a C++ compiler from running `command xcode-select --install`. To install cmake, glew, and glfw, you can use brew.

* `brew install glew`
* `brew install glfw3`

To build, open terminal and execute

```
$ git clone <this repository>
$ cd tinygl-cpp
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./tinygl-example
```

**Ubuntu**

On Ubuntu, you will need to install openGL, glew and glfw.

* `sudo apt-get install libglew-dev`
* `sudo apt-get install mesa-utils`
* `sudo apt-get install libglfw3-dev`

To build, open terminal and execute

```
$ git clone <this repository>
$ cd tinygl-cpp
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./tinygl-example
```
