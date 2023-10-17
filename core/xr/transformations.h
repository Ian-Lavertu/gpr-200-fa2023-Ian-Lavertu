//transformations.h
#pragma once
#include <cmath>
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace transformationLib {
	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		float cosValue = cos(rad);
		float sinValue = sin(rad);

		return ew::Mat4(
			1, 0, 0, 0,
			0, cosValue, -sinValue, 0,
			0, sinValue, cosValue, 0,
			0, 0, 0, 1);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		float cosValue = cos(rad);
		float sinValue = sin(rad);

		return ew::Mat4(
			cosValue, 0, sinValue, 0,
			0, 1, 0, 0,
			-sinValue, 0, cosValue, 0,
			0, 0, 0, 1);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		float cosValue = cos(rad);
		float sinValue = sin(rad);

		return ew::Mat4(
			cosValue, -sinValue, 0, 0,
			sinValue, cosValue, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	//Creates a right handed view space
	//eye = eye (camera) position
	//target = position to look at
	//up = up axis, usually(0,1,0)
	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up) {
		ew::Vec3 zAxis = ew::Normalize(eye - target);
		ew::Vec3 xAxis = ew::Normalize(ew::Cross(up, zAxis));
		ew::Vec3 yAxis = ew::Normalize(ew::Cross(zAxis, xAxis));

		return ew::Mat4(
			xAxis.x, xAxis.y, xAxis.z, -ew::Dot(xAxis, eye),
			yAxis.x, yAxis.y, yAxis.z, -ew::Dot(yAxis, eye),
			zAxis.x, zAxis.y, zAxis.z, -ew::Dot(zAxis, eye),
			0, 0, 0, 1
		);
	};

	//Orthographic projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far) {
		float top = height / 2;
		float bottom = -top;
		float right = (height * aspect) / 2;
		float left = -right;
		return ew::Mat4(
			2 / (right - left), 0, 0, -(right + left) / (right - left),
			0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, -2 / (far - near), -(far + near) / (far - near),
			0, 0, 0, 1
		);
	};

	//Perspective projection
	//fov = vertical aspect ratio (radians)
	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far) {
		return ew::Mat4(
			1 / (tan(fov / 2) * aspect), 0, 0, 0,
			0, 1 / tan(fov / 2), 0, 0,
			0, 0, (near + far) / (near - far), (2 * far * near) / (near - far),
			0, 0, -1, 0
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		//Euler angles (degrees)
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			ew::Mat4 translationMatrix = Translate(position);
			ew::Mat4 rotationMatrix = RotateX(ew::Radians(rotation.x)) * RotateY(ew::Radians(rotation.y)) * RotateZ(ew::Radians(rotation.z));
			ew::Mat4 scaleMatrix = Scale(scale);

			return translationMatrix * rotationMatrix * scaleMatrix;
		}
	};
}