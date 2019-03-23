#include <imgui.h>
#include <imgui_test/gl.h>
#include <opencv2/imgproc.hpp>

namespace imgui_test
{

bool glTexFromMat(cv::Mat& image, GLuint& texture_id);

}  // namespace imgui_test
