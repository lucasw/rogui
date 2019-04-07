#include <imgui.h>
#include <rogui/gl.h>
#include <opencv2/imgproc.hpp>

namespace rogui
{

bool glTexFromMat(cv::Mat& image, GLuint& texture_id);

}  // namespace rogui
