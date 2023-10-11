#pragma once

#include <sstream>
#include <fstream>
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

using namespace std;

namespace xr {
	string loadShaderSourceFromFile(const string& filePath);

	class ShaderLibrary {
	public:
		ShaderLibrary(const string& vertexShader, const string& fragmentShader);
		void use();
		void setInt(const string& name, int v);
		void setFloat(const string& name, float v);
		void setVec2(const string& name, float x, float y);
		void setVec3(const string& name, float x, float y, float z);
		void setVec4(const string& name, float x, float y, float z, float w);
		void setMat4(const string& name, const ew::Mat4& v) const;
		unsigned int getID();
	private:
		unsigned int id; //OpenGL program handle
	};
};