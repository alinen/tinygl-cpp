#include "tinygl-cpp.h"

using namespace tinygl;
class MyWindow : public Window {
  public:
    MyWindow(int w, int h) : Window(w, h) {}

    void setup() override {
      std::cout << "Window size: " << width() << ", " << height() << std::endl;
    }

    void draw() override {
      background(0.2f, 0.2f, 0.2f); // parameters: r, g, b

      color(0.5, 1.0, 0.25);   // parameters: r, g, b
      circle(50, 30, 100); // parameters: x, y, radius
      circle(50, 30, 100); // parameters: x, y, radius
      square(300, 150, 200, 50); // parameters: x, y, width, height

      color(1, 0, 0, 0.5);
      triangle(350, 150, 50, 100);

      color(1, 0, 1, 0.5);
      ellipsoid(150, 350+25*sin(elapsedTime()), 100, 100);
    }
};

int main() {
  MyWindow window(500, 500);
  window.run();
}