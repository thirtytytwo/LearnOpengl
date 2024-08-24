#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace glm; 
class Camera
{
public:
	Camera(float32 fov = 90.0f, float32 aspect = 16/9, float32 nearPlane = 0.0001f, float32 farPlane = 1000.0f);
	~Camera();

	void SetPosition(vec3 position) { this->position = position; }
	void SetRotation(vec3 rotation) { this->rotation = rotation; }
	mat4x4 GetVPMatrix();

private:
	vec3 position     = vec3(0.0f);
	vec3 rotation     = vec3(0.0f);
	float32	fov	      = 0.0f;
	float32 aspect    = 0.0f;
	float32 nearPlane = 0.0f;
	float32 farPlane  = 0.0f; 
};

Camera::Camera(float32 fov, float32 aspect, float32 nearPlane, float32 farPlane)
{
	this->fov = fov;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}

Camera::~Camera()
{

}

mat4x4 Camera::GetVPMatrix()
{
	mat4x4 view = mat4x4(1.0f);
	view = translate(view, -position);
	view = rotate(view, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	view = rotate(view, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	mat4x4 projection = mat4x4(1.0f);
	projection = perspective(radians(fov), aspect, nearPlane, farPlane);
	return projection * view;
}