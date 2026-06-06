#include "Application.h"
#include "opengl_utils.h"
#include <cstdio>

void Application::TextRendering_ShowFramesPerSecond() {
  if (!m_ShowInfoText)
    return;
  static float old_seconds = (float)glfwGetTime();
  static int ellapsed_frames = 0;
  static char buffer[20] = "?? fps";
  static int numchars = 7;
  ellapsed_frames += 1;
  float seconds = (float)glfwGetTime(),
        ellapsed_seconds = seconds - old_seconds;
  if (ellapsed_seconds > 1.0f) {
    numchars =
        snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
    old_seconds = seconds;
    ellapsed_frames = 0;
  }
  float lineheight = TextRendering_LineHeight(m_Window),
        charwidth = TextRendering_CharWidth(m_Window);

  float x = 1.0f - (numchars + 1) * charwidth;
  float y = 1.0f - lineheight;
  float rect_w = (numchars + 1) * charwidth;
  float rect_h = lineheight;

  TextRendering_DrawRectangle(
      m_Window,
      x,
      y + rect_h / 1.25f,
      rect_w,
      rect_h,
      glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
  );
  TextRendering_PrintString(
      m_Window,
      buffer,
      x,
      y,
      1.0f,
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
  );
}

void Application::TextRendering_ShowGameOver() {
  if (!m_GameOver)
    return;

  const char *msg = "GAME OVER";
  int numchars = 9;
  float lineheight = TextRendering_LineHeight(m_Window);
  float charwidth = TextRendering_CharWidth(m_Window);

  float x = -(numchars / 2.0f) * charwidth * 2.0f;
  float y = 0.0f;

  TextRendering_PrintString(
      m_Window,
      msg,
      x,
      y,
      2.0f,
      glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
  );
}
