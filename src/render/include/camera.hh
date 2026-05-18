//
// Created by Martin Gruscher on 18.05.26.
//

#ifndef SWARMS_CAMERA_H
#define SWARMS_CAMERA_H
#include <Eigen/Dense>

namespace tz {


struct CameraUniformBufferObject
{
  Eigen::Matrix4f view;
  Eigen::Matrix4f proj;
};


enum class CameraType
{
  Ortho,
  Perspective
};


class Camera
{
  public:
  [[maybe_unused]] Camera(Eigen::Vector3f pos, Eigen::Vector3f lookAt, CameraType type) : pos(pos), lookAt(lookAt), type(type)
  {

  }

  // Right-handed look-at view matrix for Vulkan // Vulkan uses: +X right, +Y down (in NDC), +Z forward (into screen) // World space convention here: +X right, +Y up, -Z forward (RH, like GLM default)
  Eigen::Matrix4f lookAtRH()
  {
    Eigen::Vector3f up = {0, 1, 0};
    // Forward vector (from eye to target), negated for RH
    Eigen::Vector3f f = (lookAt - pos).normalized(); // Right vector
    Eigen::Vector3f s = f.cross(up).normalized(); // Recomputed up (orthogonal)
    Eigen::Vector3f u = s.cross(f);
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    view(0, 0) = s.x();
    view(0, 1) = s.y();
    view(0, 2) = s.z();
    view(1, 0) = u.x();
    view(1, 1) = u.y();
    view(1, 2) = u.z();
    view(2, 0) = -f.x();
    view(2, 1) = -f.y();
    view(2, 2) = -f.z();
    view(0, 3) = -s.dot(pos);
    view(1, 3) = -u.dot(pos);
    view(2, 3) = f.dot(pos);
    return view;
  }

  Eigen::Matrix4f getProjectionMatrix(float width, float height)
  {
    if (type == CameraType::Perspective)
    {
      float fovY = 0.5236; // around 30 degress vertical fov
      float aspect = width / height;
      float tanHalfFovy = std::tan(fovY * 0.5f);
      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      // TODO customizable:

      float zFar = 1000;
      float zNear = 0.1;

      m(0,0) = 1.0f / (aspect * tanHalfFovy);
      m(1,1) = 1.0f / (tanHalfFovy);
      m(2,2) = zFar / (zNear - zFar);
      m(2,3) = (zNear * zFar) / (zNear - zFar);
      m(3,2) = -1.0f;

      return m;
    }
    else if (type == CameraType::Ortho)
    {
      float left = 0;
      float right = width;
      float top = height;
      float bottom = 0;

      // TODO customizable:
      float zFar = 100;
      float zNear = 0.1;

      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      m(0,0) = 2.0f / (right - left);
      m(1,1) = 2.0f / (top - bottom);
      m(2,2) = 1.0f / (zNear - zFar);   // Vulkan: [0,1] depth
      m(3,3) = 1.0f;

      m(0,3) = -(right + left) / (right - left);
      m(1,3) = -(top + bottom) / (top - bottom);
      m(2,3) = zNear / (zNear - zFar);

      return m;
    }
  }


  public:
  Eigen::Vector3f pos;
  Eigen::Vector3f lookAt;
  CameraType type;

};

}


#endif //SWARMS_CAMERA_H
