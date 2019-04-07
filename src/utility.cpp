#include <rogui/utility.hpp>
#include <opencv2/highgui.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace rogui
{

bool glTexFromMat(cv::Mat& image, GLuint& texture_id)
{
  if (image.empty()) {
    // TODO(lucasw) or make the texture 0x0 or 1x1 gray.
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Do these know which texture to use because of the above bind?
  // TODO(lucasw) make these configurable live
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Set texture clamping method - GL_CLAMP isn't defined
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // use fast 4-byte alignment (default anyway) if possible
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);

  // copy the data to the graphics memory
  // TODO(lucasw) actually look at the image encoding type and
  // have a big switch statement here
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               image.cols, image.rows,
               0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());

  // set length of one complete row in data (doesn't need to equal image.cols)
  // glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image_.elemSize());

  glBindTexture(GL_TEXTURE_2D, 0);

  return true;
}

}  // namespace rogui
