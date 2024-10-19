#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace glm; 
class Camera
{
public:
	Camera(float32 fov = 60.f, float32 aspect = 0, float32 nearPlane = 0.01f, float32 farPlane = 100.0f); 

	void SetPosition(vec3 pos) { this->position = pos; }
	void SetAspectRatio(float32 asp) { this->aspect = asp; }
	void Translate(vec3 dir, float speed, float deltaTime);
	void Rotate(float32 _yaw, float32 _pitch);
	
	mat4x4 GetViewMatrix();
	mat4x4 GetProjectionMatrix();
	vec3   GetPosition();

private:
	vec3 position     = vec3(0.0f);

	vec3 front;
	vec3 up;
	vec3 right;

	float32 yaw = -90.0f;
	float32 pitch = 0.f;
	
	float32	fov	      = 0.0f;
	float32 aspect    = 0.0f;
	float32 nearPlane = 0.0f;
	float32 farPlane  = 0.0f;

	void UpdateCameraDir();
};

inline Camera::Camera(float32 fov, float32 aspect, float32 nearPlane, float32 farPlane)
{
	this->fov = fov;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	UpdateCameraDir();
}

inline mat4x4 Camera::GetViewMatrix()
{
	mat4x4 view = mat4x4(1.0f);
	view = lookAt(position, position + front, up);
	return view;
}
inline mat4x4 Camera::GetProjectionMatrix()
{
	mat4x4 projection = mat4x4(1.0f);
	projection = perspective(radians(fov), aspect, nearPlane, farPlane);
	return projection;
}

inline vec3 Camera::GetPosition()
{
	return this->position;
}
inline void Camera::Translate(vec3 dir, float speed, float deltaTime)
{
	float velocity = speed * deltaTime;
	if(dir == vec3(0, 0, -1)) position += front * velocity;
	if(dir == vec3(0, 0, 1)) position -= front * velocity;
	if(dir == vec3(1, 0, 0)) position += right * velocity;
	if(dir == vec3(-1, 0, 0)) position -= right * velocity;
	if(dir == vec3(0, 1, 0)) position += up * velocity;
	if(dir == vec3(0, -1, 0)) position -= up * velocity;
	
}
inline void Camera::Rotate(float32 _yaw, float32 _pitch)
{
	this->yaw += _yaw;
	this->pitch += _pitch;
	if(this->pitch > 89.0f)
		this->pitch = 89.0f;
	if(this->pitch < -89.0f)
		this->pitch = -89.0f;
	UpdateCameraDir();
}

inline void Camera::UpdateCameraDir()
{
	vec3 front;
	front.x = cos(radians(yaw)) * cos(radians(pitch));
	front.y = sin(radians(pitch));
	front.z = sin(radians(yaw)) * cos(radians(pitch));
	this->front = normalize(front);
	this->right = normalize(cross(this->front, vec3(0, 1, 0)));
	this->up = normalize(cross(this->right, this->front));
}

