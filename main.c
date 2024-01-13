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
      update_camera(cursor_delta, scroll_delta, *player_transform, viewproj_matrix);
    }

    // Render
    {
      clear_window();

      draw_level(viewproj_matrix);
      draw_player(viewproj_matrix);

      /* if (draw_player_vel_arrow)
      {
         // Translation
         mat4 arrow_matrix;
         glm_translate_make(arrow_matrix, player_matrix[3]);

         // Rotation
         {
           vec3 dir;
           glm_vec3_normalize_to(player_vel, dir);

           vec3 forward;
           forward[0] = forward[1] = 0.0f;
           forward[2] = 1.0f;

           versor quat;
           glm_quat_from_vecs(forward, dir, quat);
           glm_quat_rotate(arrow_matrix, quat, arrow_matrix);
         }

         // Scale
         // glm_scale_uni(arrow_matrix, glm_vec3_norm(player_vel));

         draw_arrow(arrow_matrix, viewproj_matrix, player_vel_arrow_color);
       }

       if (draw_hit_arrow)
      {
         // Translation
         mat4 arrow_matrix;
         glm_translate_make(arrow_matrix, player_matrix[3]);

         // Rotation
         {
           vec3 dir;
           glm_vec3_normalize_to(hit_arrow, dir);

           vec3 forward;
           forward[0] = forward[1] = 0.0f;
           forward[2] = 1.0f;

           versor quat;
           glm_quat_from_vecs(forward, dir, quat);
           glm_quat_rotate(arrow_matrix, quat, arrow_matrix);
         }

         // Scale
         // glm_scale_uni(arrow_matrix, glm_vec3_norm(player_vel));

         draw_arrow(arrow_matrix, viewproj_matrix, hit_arrow_color);
       }

      if (hit_active)
      {
        mat4 world_matrix;
        glm_translate_make(world_matrix, hit_point);
        draw_arrow(world_matrix, viewproj_matrix);
      }*/

      refresh_window();
    }
  }

  destroy_level();
  destroy_player();

  destroy_window();

  return EXIT_SUCCESS;
}
