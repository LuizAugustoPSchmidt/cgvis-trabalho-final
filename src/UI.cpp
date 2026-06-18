#include "Application.h"
#include "opengl_utils.h"
#include <cstdio>

void Application::TextRendering_ShowStartScreen() {
  float lineheight = TextRendering_LineHeight(m_Window);
  float charwidth = TextRendering_CharWidth(m_Window);

  const char *title = "CACA NAS ESTRELAS";
  int titleChars = 17;
  float titleScale = 2.0f;
  float xTitle = -(titleChars / 2.0f) * charwidth * titleScale;
  float yTitle = 0.25f;
  TextRendering_PrintString(
      m_Window,
      title,
      xTitle,
      yTitle,
      titleScale,
      glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)
  );

  const char *prompt = "Pressione ESPACO ou clique para comecar";
  int promptChars = 38;
  float xPrompt = -(promptChars / 2.0f) * charwidth;
  float yPrompt = yTitle - lineheight * titleScale - lineheight * 2.0f;
  TextRendering_PrintString(
      m_Window,
      prompt,
      xPrompt,
      yPrompt,
      1.0f,
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
  );

  const char *controls = "Mover: mouse   Boost: W   Atirar: ESPACO   Camera: C";
  int controlsChars = 51;
  float xControls = -(controlsChars / 2.0f) * charwidth;
  float yControls = yPrompt - lineheight * 4.0f;
  TextRendering_PrintString(
      m_Window,
      controls,
      xControls,
      yControls,
      1.0f,
      glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)
  );
}

void Application::TextRendering_ShowPauseScreen() {
  if (!m_Paused)
    return;

  float lineheight = TextRendering_LineHeight(m_Window);
  float charwidth = TextRendering_CharWidth(m_Window);

  const char *title = "PAUSADO";
  int titleChars = 7;
  float titleScale = 2.0f;
  float xTitle = -(titleChars / 2.0f) * charwidth * titleScale;
  float yTitle = 0.2f;
  TextRendering_PrintString(
      m_Window,
      title,
      xTitle,
      yTitle,
      titleScale,
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
  );

  float yOpts = yTitle - lineheight * titleScale - lineheight * 2.0f;
  float optScale = 1.0f;

  const char *opt1 = "[ESC] Continuar";
  TextRendering_PrintString(
      m_Window,
      opt1,
      -(15 / 2.0f) * charwidth,
      yOpts,
      optScale,
      glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)
  );
  const char *opt2 = "[R] Reiniciar";
  TextRendering_PrintString(
      m_Window,
      opt2,
      -(13 / 2.0f) * charwidth,
      yOpts - lineheight * 2.5f,
      optScale,
      glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)
  );
  const char *opt3 = "[Q] Sair";
  TextRendering_PrintString(
      m_Window,
      opt3,
      -(8 / 2.0f) * charwidth,
      yOpts - lineheight * 5.0f,
      optScale,
      glm::vec4(1.0f, 0.3f, 0.3f, 1.0f)
  );
}

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

void Application::TextRendering_ShowVictory() {
  if (!m_Victory)
    return;

  const char *msg = "YOU WIN";
  int numchars = 7;
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
      glm::vec4(0.2f, 1.0f, 0.2f, 1.0f)
  );
}

void Application::TextRendering_ShowGameOver() {
  if (!m_GameOver)
    return;

  float lineheight = TextRendering_LineHeight(m_Window);
  float charwidth = TextRendering_CharWidth(m_Window);

  const char *msg = "GAME OVER";
  float titleScale = 2.0f;
  float yTitle = 0.2f;
  TextRendering_PrintString(
      m_Window, msg, -(9 / 2.0f) * charwidth * titleScale, yTitle, titleScale,
      glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
  );

  float yOpts = yTitle - lineheight * titleScale - lineheight * 2.0f;

  const char *opt1 = "[R] Reiniciar";
  TextRendering_PrintString(
      m_Window, opt1, -(13 / 2.0f) * charwidth, yOpts, 1.0f,
      glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)
  );
  const char *opt2 = "[Q] Sair";
  TextRendering_PrintString(
      m_Window, opt2, -(8 / 2.0f) * charwidth, yOpts - lineheight * 2.5f, 1.0f,
      glm::vec4(1.0f, 0.3f, 0.3f, 1.0f)
  );
}
