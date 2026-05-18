#ifndef _UTILS_H
#define _UTILS_H

#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <random>

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <Numeric T> T clamp(T in, T min, T max) {
  if (in > max) {
    return max;
  }

  if (in < min) {
    return min;
  }

  return in;
}

template <Numeric T> inline T get_rand(T min, T max) {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Define the range [0.0, 1.0)
  std::uniform_real_distribution<double> dis(0.0, 1.0);

  // Generate a random number
  double random_num = dis(gen);

  T range = random_num * (max - min) + min;
  auto ret = static_cast<T>(range);

  return ret;
}

static GLenum glCheckError_(const char *file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    const char *error;
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    default:
      error = "UNKNOWN";
      break;
    }
    fprintf(
        stderr,
        "ERROR: OpenGL \"%s\" in file \"%s\" (line %d)\n",
        error,
        file,
        line
    );
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif // _UTILS_H
