#ifndef tinygl_cpp_H_
#define tinygl_cpp_H_

#include <stdio.h>
#include <iostream>
#include <cmath>

#if ( (defined(__MACH__)) && (defined(__APPLE__)) )
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

namespace tinygl {

static void error_callback(int error, const char* description) {
  fputs("\n", stderr);
  fputs(description, stderr);
  fputs("\n", stderr);
}

// todo: pass width, height, color, scalex, scaley, position
const GLchar* vertexShader[] =
{
"#version 400\n"
"uniform vec4 inColor;"
"uniform vec3 inSize;"
"uniform vec3 inPos;"
"uniform float inScreenWidth;"
"uniform float inScreenHeight;"
"in vec3 VertexPosition;"
"out vec4 color;"
"void main() {"
"  color = inColor;"
"  vec4 x = vec4(2.0 / inScreenWidth, 0.0, 0.0, 0.0);"
"  vec4 y = vec4(0.0, 2.0 / inScreenHeight, 0.0, 0.0);"
"  vec4 z = vec4(0.0, 0.0, -2.0 / 2000, 0.0);"
"  vec4 d = vec4(-1.0, -1.0, 0.0, 1.0);"
"  mat4 projection = mat4(x, y, z, d);"
"  vec3 pos = inSize * VertexPosition + inPos;"
"  gl_Position = projection * vec4(pos, 1.0);"
"}"
};

const GLchar* fragmentShader[] =
{
"#version 400\n"
"in vec4 color;"
"out vec4 FragColor;"
"void main() { FragColor = color; }"
};


static void PrintShaderErrors(GLuint id, const std::string& label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}
static class Window* theInstance = 0;

/**
 * @brief Manages the window and user input.
 *
 * Override this class to create a custom application.
 * @verbinclude sphere.cpp
 */
class Window {
 public:
  /**
   * @brief Constructor.
   *
   * Override this class to create a custom application.
   * The default window is sized 500x500 and draws an empty (black) scene.
   * @verbinclude empty.cpp
   */
  Window(int width, int height) :
    _windowWidth(width),
    _windowHeight(height),
    _elapsedTime(0.0),
    _dt(-1.0) {

    // ASN TODO
    theInstance = this;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Cannot initialize GLFW\n");
        return;
    }

    // Set the GLFW window creation hints - these are optional
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);  // Request 4x antialiasing
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(_windowWidth, _windowHeight, "TinyGL C++ Window", 0, 0);
    if (!_window) {
        fprintf(stderr, "ERROR: Cannot initialize GLFW window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback(_window, Window::onKeyboardCb);
    glfwSetMouseButtonCallback(_window, Window::onMouseButtonCb);
    glfwSetCursorPosCallback(_window, Window::onMouseMotionCb);
    glfwSetScrollCallback(_window, Window::onScrollCb);

#ifndef APPLE
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Cannot initialize GLEW\n";
        return;
    }
#endif

    // Initialize openGL and set default values
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Alpha blend
    glCullFace(GL_BACK);

    background(0, 0, 0);

    //vec3 minCorner = center - 0.5f * dim;
    //vec3 maxCorner = center + 0.5f * dim;
    //ortho(minCorner.x, maxCorner.x,
    //      minCorner.y, maxCorner.y,
    //      minCorner.z, maxCorner.z);

    const float triangle[] =
    {
        -0.5, -0.5, 0, 
        0.5, -0.5, 0,
        0.0, 0.5, 0 
    };

    const float square[] =
    {
        -0.5, -0.5, 0, 
        0.5, -0.5, 0,
        0.5, 0.5, 0,

        -0.5, -0.5, 0, 
        0.5, 0.5, 0,
        -0.5, 0.5, 0 
    };

    // numVerts = numTris + 2 -> Set numTris = 16
    float* circle = new float[3*(numTris+2)]; // numFloats = numVerts * 3 = 36
    circle[0] = circle[1] = circle[2] = 0.0;
    float deltaAngle = 2.0f * 3.1415926535897932384626433832795f / numTris;
    for (int i = 1; i < numTris+2; i++) {
      float angle = (i-1) * deltaAngle;
      circle[i*3+0] = cos(angle);
      circle[i*3+1] = sin(angle);
      circle[i*3+2] = 0;
    }

    GLuint vboId;
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangle, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_triVao);
    glBindVertexArray(_triVao);
    glEnableVertexAttribArray(0); // 0 -> activate sending VertexPositions to the active shader
    glBindBuffer(GL_ARRAY_BUFFER, vboId); // as a habit -> always bind before setting data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), square, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_squareVao);
    glBindVertexArray(_squareVao);
    glEnableVertexAttribArray(0); // 0 -> activate sending VertexPositions to the active shader
    glBindBuffer(GL_ARRAY_BUFFER, vboId); // as a habit -> always bind before setting data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, 3*(numTris+2) * sizeof(float), circle, GL_STATIC_DRAW);

    glGenVertexArrays(1, &_circleVao);
    glBindVertexArray(_circleVao);
    glEnableVertexAttribArray(0); // 0 -> activate sending VertexPositions to the active shader
    glBindBuffer(GL_ARRAY_BUFFER, vboId); // as a habit -> always bind before setting data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    delete[] circle;

    GLint result;
    GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderId, 1, vertexShader, NULL);
    glCompileShader(vshaderId);
    glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
       PrintShaderErrors(vshaderId, "Vertex shader");
    }

    GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderId, 1, fragmentShader, NULL);
    glCompileShader(fshaderId);
    glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
       PrintShaderErrors(fshaderId, "Fragment shader");
    }

    GLuint shaderId = glCreateProgram();
    glAttachShader(shaderId, vshaderId);
    glAttachShader(shaderId, fshaderId);
    glLinkProgram(shaderId);
    glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
       PrintShaderErrors(shaderId, "Shader link");
    }

    glUseProgram(shaderId);
    _colorUniform = glGetUniformLocation(shaderId, "inColor");
    _sizeUniform = glGetUniformLocation(shaderId, "inSize");
    _posUniform = glGetUniformLocation(shaderId, "inPos");

    GLuint widthUniform = glGetUniformLocation(shaderId, "inScreenWidth");
    glUniform1f(widthUniform, _windowWidth);

    GLuint heightUniform = glGetUniformLocation(shaderId, "inScreenHeight");
    glUniform1f(heightUniform, _windowHeight);
  }

  virtual ~Window() {
    glfwTerminate();   
  }

  /**
   * @brief Opens the window and starts the main application loop.
   *
   * This function should typically called from main after creating the Window
   * object. This function invokes the user's setup() function and then
   * repeatedly calls the user's draw() function. This function returns when
   * the user closes the window (via the escape key or close menu button)
   */
  void run() {
    if (!_window) return;  // window wasn't initialized
    setup();

    while (!glfwWindowShouldClose(_window)) {
        float time = glfwGetTime();
        _dt = time - _elapsedTime;
        _elapsedTime = time;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();  // user function
        glfwPollEvents();
        glfwSwapBuffers(_window);
    }
  }

 protected:

  /** @name Draw commands
   */
  ///@{
  /**
   * @brief Draws a square with the current color in pixel coordinates
   * @param x The horizontal position of the center
   * @param y The vertical position of the center
   * @param width 
   * @param height 
   *
   */
  void square(float x, float y, float width, float height) {
    glUniform3f(_posUniform, x, y, 10);
    glUniform3f(_sizeUniform, width, height, 1);
    glBindVertexArray(_squareVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  /**
   * @brief Draw a triangle with the current color using pixel coordinates
   * @param x The horizontal position of the center
   * @param y The vertical position of the center
   * @param width of the triangle base
   * @param height of the triangle
   *
   */
  void triangle(int x, int y, float width, float height) {
    glUniform3f(_posUniform, x, y, 0);
    glUniform3f(_sizeUniform, width, height, 1);
    glBindVertexArray(_triVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  /**
   * @brief Draws a circle with the current color in pixel coordinates
   * @param x The horizontal position of the center
   * @param y The vertical position of the center
   * @param diameter The width of the circle 
   *
   */
  void circle(int x, int y, float diameter) {
    ellipsoid(x, y, diameter, diameter);
  }

  /**
   * @brief Draws an ellipse with the current color in pixel coordinates
   * @param x The horizontal position of the center
   * @param y The vertical position of the center
   * @param width The width of the ellipse 
   * @param height The height of the ellipse 
   *
   */
  void ellipsoid(int x, int y, float width, float height) {
    glUniform3f(_posUniform, x, y, 0);
    glUniform3f(_sizeUniform, width, height, 1);
    glBindVertexArray(_circleVao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numTris+2);
  }

  /**
   * @brief Set the current RGB color 
   * @param r The red component (range [0,1])
   * @param g The red component (range [0,1])
   * @param b The red component (range [0,1])
   *
   */
  void color(float r, float g, float b, float a = 1.0f) {
    glUniform4f(_colorUniform, r, g, b, a);
  }

  ///@}


  /** @name Respond to events
   */
  ///@{
  /**
   * @brief Override this method to perform setup before the main application
   * loop
   */
  virtual void setup() {}

  /**
   * @brief Override this method to draw
   */
  virtual void draw() {}

  /**
   * @brief Override this method to respond to mouse movement
   * @param x The current x position
   * @param y The current y position
   * @param dx The change in x since the last mouse event (in pixels)
   * @param dy The change in y since the last mouse event (in pixels)
   *
   * Screen coordinates are in pixels. x values are always between 0 and width.
   * y values are always between 0 and height.
   */
  virtual void mouseMotion(int x, int y, int dx, int dy) {}

  /**
   * @brief Override this method to respond to mouse press (button down)
   * @param button The mouse button that is pressed, e.g. GLFW_MOUSE_BUTTON_LEFT
   * @param mods Modifiers that are pressed (e.g. shift, control, etc)
   *
   * @ref https://www.glfw.org/docs/latest/input_guide.html
   * @see mouseUp(int, int)
   * @see mouseIsDown(int)
   */
  virtual void mouseDown(int button, int mods) {}

  /**
   * @brief Override this method to respond to mouse press (button up)
   *
   * @see mouseDown(int, int)
   * @see mouseIsDown(int)
   */
  virtual void mouseUp(int button, int mods) {}

  /**
   * @brief Override this method to respond to scrolling the middle mouse
   * button
   * @param dx The change in the x direction (in scroll coordinates)
   * @param dy The change in the x direction (in scroll coordinates)
   *
   * @verbinclude plane.cpp
   */
  virtual void scroll(float dx, float dy) {}

  /**
   * @brief Override this method to respond to key presses (button up)
   * @param key The key, e.g. as either a constant or capital ascii letter,
   * such as 'P'. 
   * @param mods Modifiers that are pressed (e.g. shift, control, etc)
   *
   * @ref https://www.glfw.org/docs/latest/group__keys.html
   * @see keyDown(int,int)
   * @see keyIsDown(int)
   */
  virtual void keyUp(int key, int mods) {}

  /**
   * @brief Override this method to respond to key presses (button down)
   * @param key The key, e.g. as either a constant or capital ascii letter,
   * such as 'P'. 
   * @param mods Modifiers that are pressed (e.g. shift, control, etc)
   *
   * @ref https://www.glfw.org/docs/latest/group__keys.html
   * @see keyUp(int,int)
   */
  virtual void keyDown(int key, int mods) {}

  /** @name Query input and time
   */
  ///@{
  /** 
   * @brief Query whether the given key is down 
   * @key The key to test
   * 
   * More than one key can be pressed at once. The key can be an ascii 
   * capital letter or a GLFW constant
   * @ref https://www.glfw.org/docs/latest/input_guide.html
   */
  bool keyIsDown(int key) const {
    int state = glfwGetKey(_window, key);
    return (state == GLFW_PRESS);
  }

  /** 
   * @brief Query whether the given mouse button is down
   * @param button The button to test
   * 
   * More than one button can be pressed at once. The button should be a GLFW
   * constant, such as GLFW_MOUSE_BUTTON_LEFT
   * @ref https://www.glfw.org/docs/latest/input.html
   */
  bool mouseIsDown(int button) const {
    int state = glfwGetMouseButton(_window, button);
    return (state == GLFW_PRESS);
  }

  /** 
   * @brief Return the current mouse position X coordinate (in screen coordinates)
   */
  float mouseX() const {
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    return static_cast<float>(xpos);
  }

  /** 
   * @brief Return the current mouse position Y coordinate (in screen coordinates)
   */
  float mouseY() const {
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);
    return static_cast<float>(ypos);
  }

  /** 
   * @brief Return the amount of time since the previous frame (in seconds)
   *
   * If the frame rate is 30 frames per second, dt would be approximately 1/30
   * = 0.033333 seconds each frame
   */
  float dt() const {
    return _dt;
  }

  /** 
   * @brief Return the amount of time since the setup() was called (in seconds)
   */
  float elapsedTime() const {
    return _elapsedTime;
  }

  /** 
   * @brief Return the window height in pixels
   */
  float height() const {
    return static_cast<float>(_windowHeight);
  }

  /** 
   * @brief Return the window width in pixels
   */
  float width() const {
    return static_cast<float>(_windowWidth);
  }
  ///@}

  /** 
   * @brief Stop the main application loop
   * 
   * Call this function to terminate the main loop, either from setup() or 
   * draw()
   */
  void noLoop() {
    glfwSetWindowShouldClose(_window, GL_TRUE);
  }

  /** 
   * @brief Set the background color
   * 
   * This function can be called from draw and will additionally clear the
   * color and depth buffers
   */
  void background(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

 private:
  static void onScrollCb(GLFWwindow* w, double xoffset, double yoffset) {
    theInstance->scroll(
      static_cast<float>(xoffset),
      static_cast<float>(yoffset));
  }
  static void onMouseMotionCb(GLFWwindow* w, double x, double y) {
    theInstance->onMouseMotion(static_cast<int>(x), static_cast<int>(y));
  }

  static void onMouseButtonCb(GLFWwindow* w, int button, int action, int mods) {
    theInstance->onMouseButton(button, action, mods);
  }

  static void onKeyboardCb(GLFWwindow* w, int key, int code, int action, int mods) {
    theInstance->onKeyboard(key, code, action, mods);
  }

  void onMouseMotion(int x, int y) {
    int dx = mouseX() - _lastx;
    int dy = mouseY() - _lasty;
    mouseMotion(x, height()-y, dx, -dy);  // user hook
  }

  void onMouseButton(int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(_window, &xpos, &ypos);

    if (action == GLFW_PRESS) {
      _lastx = xpos;
      _lasty = ypos;
      mouseDown(button, mods);
    } else if (action == GLFW_RELEASE) {
      mouseUp(button, mods);
    }

    onMouseMotion(static_cast<float>(xpos), static_cast<float>(ypos));
  }

  void onKeyboard(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(_window, GL_TRUE);
    }

    if (action == GLFW_PRESS) {
      keyDown(key, mods);
    } else if (action == GLFW_RELEASE) {
      keyUp(key, mods);
    }
  }
   
 private:
  int _windowWidth, _windowHeight;
  float _elapsedTime;
  float _dt;
  float _lastx, _lasty;
  struct GLFWwindow* _window = 0;
  GLuint _triVao, _squareVao, _circleVao;
  GLuint _colorUniform, _sizeUniform, _posUniform;
  const int numTris = 16; // for circles

 protected:
  inline GLFWwindow* window() const { return _window; }
};

}  // namespace tinygl

#endif
