#include "app.hpp"
#include "world.hpp"

int main() {
  wind::World world{wind::App::Info{"Demo", 1280, 720}};
  world.run();
  return 0;
}
