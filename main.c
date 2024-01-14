// #include "arrow.h"
#include "camera.h"
#include "input.h"
#include "level.h"
#include "player.h"
#include "window.h"

#include <stdlib.h>

static mat4 viewproj_matrix;

int main(int argc, char* argv[])
{
  if (!generate_window())
  {
    return EXIT_FAILURE;
  }

  init_input();

  if (!generate_player())
  {
    return EXIT_FAILURE;
  }

  if (!generate_level())
  {
    return EXIT_FAILURE;
  }

  // generate_arrow();

  // Main loop
  double time = get_time();
  while (!should_window_close())
  {
    if (is_key_down(KEY_ESC))
    {
      request_window_close();
    }

    // Update
    {
      // Calculate delta time
      float delta_time;
      {
        const double now = get_time();
        delta_time = (float)(now - time);
        time = now;
      }

      // Get cursor input
      vec2 cursor_delta;
      get_cursor_delta(cursor_delta);

      // Get scroll input
      const float scroll_delta = get_scroll_delta();

      update_player(cursor_delta, delta_time);

      mat4* player_transform = get_player_transform();
      const float player_height = get_player_height();
      update_camera(cursor_delta, scroll_delta, *player_transform, player_height, viewproj_matrix);
    }

    // Render
    {
      clear_window();

      draw_level(viewproj_matrix);
      draw_player(viewproj_matrix);

      refresh_window();
    }
  }

  destroy_level();
  destroy_player();

  destroy_window();

  return EXIT_SUCCESS;
}
