#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <vector>

const char *window_title = "Hello World!";
const int window_x = 100;
const int window_y = 100;
const int window_width = 1680;
const int window_height = 1200;
const int tile_res = 8;
const int tile_scale = 3;
const int buffer_width = window_width / tile_res / tile_scale;
const int buffer_height = window_height / tile_res / tile_scale;
const int FPS_LIMIT = 30;

enum class TileType { Empty, Wall, Floor };

struct Char {
  Char(std::string ch = "", SDL_Color fg = {255, 255, 255, 255},
       SDL_Color bg = {0, 0, 0, 255})
      : ch(ch), fg(fg), bg(bg) {}

  std::string ch;
  SDL_Color fg;
  SDL_Color bg;
};

class Buffer {
public:
  static Buffer build_default() {
    return Buffer(buffer_width, buffer_height,
                  Char("ą", {255, 0, 255, 255}, {0, 0, 100, 255}));
  }

  Buffer(size_t width, size_t height, Char paint)
      : width(width), height(height) {
    resize(width, height);
    fill(paint);
  }

  void resize(size_t width, size_t height) {
    buffer.resize(width);
    for (size_t x = 0; x < width; ++x) {
      buffer[x].resize(height);
    }
  }

  void fill(Char fill) {
    for (size_t x = 0; x < width; ++x) {
      for (size_t y = 0; y < height; ++y) {
        buffer[x][y] = fill;
      }
    }
  }

  Char &at(size_t x, size_t y) { return buffer[x][y]; }

  void render(SDL_Renderer *renderer, TTF_Font *font) {
    for (int row = 0; row < buffer_height; ++row) {
      for (int col = 0; col < buffer_width; ++col) {
        // Get the character
        const auto &ch = buffer[col][row];
        const auto str = ch.ch.c_str();

        // Create a surface from the text
        SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, str, ch.fg);

        if (textSurface == nullptr) {
          throw std::runtime_error(std::string("TTF_RenderText_Solid Error: ") +
                                   TTF_GetError());
        }

        // Create a texture from the surface
        SDL_Texture *textTexture =
            SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture == nullptr) {
          throw std::runtime_error(
              std::string("SDL_CreateTextureFromSurface Error: ") +
              SDL_GetError());
        }

        // Now you can free the surface after creating the texture
        SDL_FreeSurface(textSurface);

        // Render the texture
        SDL_Rect dstRect;
        dstRect.x = col * tile_res * tile_scale;
        dstRect.y = row * tile_res * tile_scale;
        dstRect.w = tile_res * tile_scale;
        dstRect.h = tile_res * tile_scale;

        SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);

        // Clean up
        SDL_DestroyTexture(textTexture);
      }
    }
  }

private:
  int width;
  int height;
  std::vector<std::vector<Char>> buffer;
};

class Assets {
public:
  static Assets build_default() { return Assets(); }

  Assets() { font = sdl_load_font("../assets/unscii-8-fantasy.ttf", 8); }
  ~Assets() { sdl_close_font(font); }

  TTF_Font *get_font() { return font; }

private:
  TTF_Font *sdl_load_font(const char *font_path, int font_size) {
    TTF_Font *font;
    font = TTF_OpenFont(font_path, font_size);
    if (font == nullptr) {
      throw std::runtime_error(std::string("TTF_OpenFont Error: ") +
                               TTF_GetError());
    }
    return font;
  }

  void sdl_close_font(TTF_Font *font) { TTF_CloseFont(font); }

  TTF_Font *font;
};

class Tile {
public:
  enum class Type {
    Passable,
    Impassable,
  };

  static Tile build_wall() { return Tile(Type::Impassable, Char("█")); }
  static Tile build_floor() { return Tile(Type::Passable, Char(".")); }

  Tile(Type type, Char ch) : type(type), ch(ch) {}

  Type type;
  Char ch;
};

class Map {
public:
  Map()

      private:
};

class Core {
public:
  Core(SDL_Window *window, SDL_Renderer *renderer)
      : buffer(Buffer::build_default()), assets(Assets::build_default()),
        running(true), window(window), renderer(renderer) {}
  ~Core() {}

  void run() {
    auto last_frame_end = SDL_GetTicks64();
    while (running) {
      auto frame_start = SDL_GetTicks64();
      auto delta_time = frame_start - last_frame_end;

      update(delta_time);
      redraw();

      auto frame_end = SDL_GetTicks64();
      last_frame_end = frame_end;
      if (frame_end - frame_start < 1000 / FPS_LIMIT) {
        auto frame_delay = 1000 / FPS_LIMIT - (frame_end - frame_start);
        SDL_Delay(frame_delay);
      }
    }
  }

  void update(unsigned long delta_time) { handle_input_events(); }

  void redraw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    buffer.render(renderer, assets.get_font());

    SDL_RenderPresent(renderer);
  }

private:
  void handle_input_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        handle_event_close();
      }
    }
  }

  void handle_event_close() { running = false; }

  SDL_Window *window;
  SDL_Renderer *renderer;

  bool running;
  Buffer buffer;
  Assets assets;
};

void sdl_init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    throw std::runtime_error("SDL_Init Error");
  }
}
void sdl_ttf_init() {
  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    throw std::runtime_error("TTF_Init Error");
  }
}
void sdl_quit() { SDL_Quit(); }
void sdl_ttf_quit() { TTF_Quit(); }

int main(int argc, char *argv[]) {
  sdl_init();
  sdl_ttf_init();

  SDL_Window *window =
      SDL_CreateWindow(window_title, window_x, window_y, window_width,
                       window_height, SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == nullptr) {
    SDL_DestroyWindow(window);
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  {
    auto core = Core(window, renderer);
    core.run();
  }
  // TODO: Fonts are not being freed in correct order

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  sdl_quit();
  sdl_ttf_quit();

  return 0;
}