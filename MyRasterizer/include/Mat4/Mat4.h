#pragma once

#include "../Vec4/Vec4.h"

namespace lm
{
	template <typename T> class Mat4
	{
	private:
		Vec4<T> matrix[4];

	public:
		static const Mat4<T> identity;

		Mat4()
		{
			this->matrix[0] = Vec4<T>(1, 0, 0, 0);
			this->matrix[1] = Vec4<T>(0, 1, 0, 0);
			this->matrix[2] = Vec4<T>(0, 0, 1, 0);
			this->matrix[3] = Vec4<T>(0, 0, 0, 1);
		}

		Mat4(const T init)
		{
			this->matrix[0] = Vec4<T>(init, 0, 0, 0);
			this->matrix[1] = Vec4<T>(0, init, 0, 0);
			this->matrix[2] = Vec4<T>(0, 0, init, 0);
			this->matrix[2] = Vec4<T>(0, 0, 0, 1);
		}

		Mat4(const Vec4<T>& v1, const Vec4<T>& v2, const Vec4<T>& v3, const Vec4<T>& v4)
		{
			this->matrix[0] = v1;
			this->matrix[1] = v2;
			this->matrix[2] = v3;
			this->matrix[3] = v4;
		}

		Mat4(const Mat4<T>& mat4)
		{
			for (unsigned int i = 0; i < 4; i++)
				this->matrix[i] = mat4.matrix[i];
		}

		Mat4(Mat4<T>&& mat4) noexcept
		{
			for (unsigned int i = 0; i < 4; i++)
				this->matrix[i] = std::move(mat4.matrix[i]);
		}

		Mat4<T>& operator=(const Mat4<T>& mat4)
		{
			if (this == &mat4)
				return *this;

			for (unsigned int i = 0; i < 4; i++)
				this->matrix[i] = mat4.matrix[i];

			return *this;
		}

		Mat4<T>& operator=(Mat4<T>&& mat4) noexcept
		{
			if (this == &mat4)
				return *this;

			for (unsigned int i = 0; i < 4; i++)
				this->matrix[i] = std::move(mat4.matrix[i]);

			return *this;
		}

		const Vec4<T> operator[](int idx) const
		{
			if (idx >= 0 && idx < 4)
				return this->matrix[idx];
			return this->matrix[0];
		}

		Vec4<T>& operator[](int idx)
		{
			if (idx >= 0 && idx < 4)
				return this->matrix[idx];
			return this->matrix[0];
		}

		const T operator[](const char* idx) const
		{
			unsigned int vecIdx = idx[1] - '1';
			switch (idx[0])
			{
			case 'x': return this->matrix[vecIdx].X();
			case 'y': return this->matrix[vecIdx].Y();
			case 'z': return this->matrix[vecIdx].Z();
			case 'w': return this->matrix[vecIdx].W();

			default: return this->matrix[0].X();
			}
		}

		T& operator[](const char* idx)
		{
			unsigned int vecIdx = idx[1] - '0';
			switch (idx[0])
			{
			case 'x': return this->matrix[vecIdx].X();
			case 'y': return this->matrix[vecIdx].Y();
			case 'z': return this->matrix[vecIdx].Z();
			case 'w': return this->matrix[vecIdx].W();

			default: return this->matrix[0].X();
			}
		}

		Mat4<T> operator*(const Mat4<T>& mat4) const
		{
			Mat4<T> newMat4;

			for (unsigned int i = 0; i < 4; i++)
			{
				Vec4<T> vec4;
				for (unsigned int j = 0; j < 4; j++)
				{
					vec4[j] = this->matrix[0][j] * mat4.matrix[i].X()
						+ this->matrix[1][j] * mat4.matrix[i].Y()
						+ this->matrix[2][j] * mat4.matrix[i].Z()
						+ this->matrix[3][j] * mat4.matrix[i].W();
				}
				newMat4.matrix[i] = vec4;
			}
			
			return newMat4;
		}
		
		Vec4<T> operator*(const Vec4<T>& vec4) const
		{
			Vec4<T> newVec4;
			newVec4.X() = (this->matrix[0][0] * vec4.X()) + (this->matrix[1].X() * vec4.Y()) + (this->matrix[2].X() * vec4.Z()) + (this->matrix[3].X() * vec4.W());
			newVec4.Y() = (this->matrix[0][1] * vec4.X()) + (this->matrix[1].Y() * vec4.Y()) + (this->matrix[2].Y() * vec4.Z()) + (this->matrix[3].Y() * vec4.W());
			newVec4.Z() = (this->matrix[0].Z() * vec4.X()) + (this->matrix[1].Z() * vec4.Y()) + (this->matrix[2].Z() * vec4.Z()) + (this->matrix[3].Z() * vec4.W());
			newVec4.W() = (this->matrix[0].W() * vec4.X()) + (this->matrix[1].W() * vec4.Y()) + (this->matrix[2].W() * vec4.Z()) + (this->matrix[3].W() * vec4.W());
			return newVec4;
		}

		static Mat4<T> createTransformMatrix(const Vec3<T>& rotation, const Vec3<T>& position, const Vec3<T>& scaleVec)
		{
			return translation(position) * zRotation(rotation.Z()) * xRotation(rotation.X()) * yRotation(rotation.Y()) * scale(scaleVec);
		}
		
		static Mat4<T> createView(const Vec3<T>& rotation, const Vec3<T>& position)
		{
			return translation(position) * xRotation(rotation.X()) * yRotation(rotation.Y());
		}

		Mat4<T> getInverse()
		{
			float test = this->matrix[2].X();
			float test2 = this->matrix[0].Z();

			std::swap(this->matrix[0].Z(), this->matrix[2].X());
			std::swap(this->matrix[0].Y(), this->matrix[1].X());
			std::swap(this->matrix[1].Z(), this->matrix[2].Y());

			this->matrix[3] *= -1;
			return *this;
		}

		static Mat4<T> translation(const Vec3<T>& translation)
		{
			Mat4<T> translate;
			translate["x3"] = translation.X();
			translate["y3"] = translation.Y();
			translate["z3"] = translation.Z();
			return translate;
		}

		static Mat4<T> scale(const Vec3<T>& scale)
		{
			Mat4<T> matrixScale;
			matrixScale["x0"] = scale.X();
			matrixScale["y1"] = scale.Y();
			matrixScale["z2"] = scale.Z();
			return matrixScale;
		}

		static Mat4<T> xRotation(float angle)
		{
			float rad = float(Vec4<T>::degreesToRadians(double(angle)));

			Mat4<T> matrixScale;
			matrixScale["y1"] = std::cosf(rad);
			matrixScale["y2"] = -std::sinf(rad);

			matrixScale["z1"] = std::sinf(rad);
			matrixScale["z2"] = std::cosf(rad);
			
			return matrixScale;
		}

		static Mat4<T> yRotation(float angle)
		{
			float rad = float(Vec4<T>::degreesToRadians(double(angle)));

			Mat4<T> matrixRotation;
			matrixRotation["x0"] = std::cosf(rad);
			matrixRotation["x2"] = std::sinf(rad);

			matrixRotation["z0"] = -std::sinf(rad);
			matrixRotation["z2"] = std::cosf(rad);

			return matrixRotation;
		}

		static Mat4<T> zRotation(float angle)
		{
			float rad = float(Vec4<T>::degreesToRadians(double(angle)));

			Mat4<T> matrixScale;
			matrixScale["x0"] = std::cosf(rad);
			matrixScale["x1"] = -std::sinf(rad);

			matrixScale["y0"] = std::sinf(rad);
			matrixScale["y1"] = std::cosf(rad);

			return matrixScale;
		}

		static Mat4<T> perspectiveProjection(float fovy, float aspect, float near, float far)
		{
			float scale = tanf(fovy * float(M_PI / 360)) * near;
			float r = aspect * scale;
			float l = -r;
			float t = scale;
			float b = -t;

			float x1 = 2 * near / (r - l);
			float y2 = 2 * near / (t - b);
			float x3 = (r + l) / (r - l);
			float y3 = (t + b) / (t - b);
			float z3 = -(far + near) / (far - near);
			float z4 = -(2 * far * near) / (far - near);
	
			lm::Mat4<T> mat4;

			mat4.matrix[0] = lm::Vec4<T>(x1, 0, 0, 0);
			mat4.matrix[1] = lm::Vec4<T>(0, y2, 0, 0);
			mat4.matrix[2] = lm::Vec4<T>(x3, y3, z3, -1);
			mat4.matrix[3] = lm::Vec4<T>(0, 0, z4, 0);

			return mat4;
		}

		const bool operator==(const Mat4<T>& mat4) const
		{
			if (this == &mat4)
				return true;

			for (unsigned int i = 0; i < 4; i++)
				if (this->matrix[i] != mat4.matrix[i])
					return false;

			return true;
		}

		const bool operator!=(const Mat4<T>& mat4) const
		{
			return !(*this == mat4);
		}
	};

	template<class T> const Mat4<T> Mat4<T>::identity = Mat4<T>();

	typedef Mat4<float> mat4;
}