#include "tinygl-cpp.h"

using namespace tinygl;
class MyWindow : public Window {
  private:
    struct Point { float x; float y; };

  public:
    MyWindow(int w, int h) : Window(w, h) {}

    void setup() override {
      std::cout << "Window size: " << width() << ", " << height() << std::endl;
      loadSprite("particle", "../sprites/particle.png");
      loadSprite("star", "../sprites/star5.png");

      float dx = width() / (float)numParticles;
      for (int i = 0; i < numParticles; i++) {
        float x = i * dx;
        float y = height() * abs(sin(x));
        particles.push_back(Point{x, y});
      }
    }

    void draw() override {
      background(0.2f, 0.2f, 0.2f); // parameters: r, g, b

      color(0.5, 1.0, 1.0);   // parameters: r, g, b
      for (int i = 0; i < numParticles; i++) {
        sprite("particle", particles[i].x, particles[i].y, 0.25); 
        particles[i].y = fmod(particles[i].y + 200 * dt(), (float)height());
      }

      color(1.0, 1.0, 0.0);   // parameters: r, g, b
      float speed = 200;
      if (keyIsDown(GLFW_KEY_LEFT)) {
        p.x = std::max(0.0f, p.x - speed*dt()); 
      }
      else if (keyIsDown(GLFW_KEY_RIGHT)) {
        p.x = std::min((float)width(), p.x + speed*dt()); 
      }
      sprite("star", p.x, p.y, 0.5); // parameters: x, y, scale
    }

    void mouseMotion(int x, int y, int dx, int dy) {}
    void mouseDown(int button, int mods) {}
    void mouseUp(int button, int mods) {}
    void scroll(float dx, float dy) {}
    void keyDown(int key, int mods) {}
    void keyUp(int key, int mods) {}

  protected:
    std::vector<Point> particles;
    Point p = Point{250, 400};
    int numParticles = 50;
};

int main() {
  MyWindow window(500, 500);
  window.run();
}
