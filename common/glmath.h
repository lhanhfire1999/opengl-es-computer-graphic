#pragma once

#include <math.h>
const auto PI = 3.1415926f;

float sinDeg(float angle)
{
	return sinf(angle * PI / 180.0f);
}

float cosDeg(float angle)
{
	return cosf(angle * PI / 180.0f);
}

class Vector
{
private:
	float v[4];
public:
	Vector(float x, float y, float z, float w)
	{
		v[0] = x; v[1] = y; v[2] = z; v[3] = w;
	}
	Vector(const float* p)
	{
		for (auto i = 0; i < 4; ++i) v[i] = p[i];
	}
	float x() const { return v[0]; }
	float y() const { return v[1]; }
	float z() const { return v[2]; }
	float w() const { return v[3]; }
	const float* data() const { return v; }
};

class Matrix
{
private:
	float m[16];

	Matrix()
	{
		for (auto i = 0; i < 16; ++i) m[i] = 0.0f;
	}

	Matrix(const float* p)
	{
		for (auto i = 0; i < 4; ++i)
			for (auto j = 0; j < 4; ++j)
				m[i * 4 + j] = p[j * 4 + i];
	}

	float get(int row, int col) const { return m[col * 4 + row]; }
	void set(int row, int col, float value) { m[col * 4 + row] = value; }

public:
	const float* data() const { return m; }

	Matrix operator * (const Matrix& other)
	{
		auto result = Matrix();		
		for (auto row = 0; row < 4; ++row)
			for (auto col = 0; col < 4; ++col)
			{
				auto sum = 0.0f;
				for (auto k = 0; k < 4; ++k) sum += this->get(row, k) * other.get(k, col);
				result.set(row, col, sum);
			}
		return result;
	}

	Vector operator * (const Vector& vector)
	{
		float result[4];
		auto v = vector.data();
		for (auto row = 0; row < 4; ++row)
		{
			auto sum = 0.0f;
			for (int k = 0; k < 4; ++k) sum += this->get(row, k) * v[k];
			result[row] = sum;
		}
		return Vector(result);
	}

	static Matrix identity()
	{
		auto a = Matrix();
		for (auto row = 0; row < 4; ++row)
			for (auto col = 0; col < 4; ++col)
			{
				const auto value = row == col ? 1.0f : 0.0f;
				a.set(row, col, value);
			}
		return a;
	}

	static Matrix translate(float a, float b, float c)
	{
		float p[] =
		{
			1.0f, 0.0f, 0.0f,    a,
			0.0f, 1.0f, 0.0f,    b,
			0.0f, 0.0f, 1.0f,    c,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		return Matrix(p);
	}

	static Matrix scale(float a, float b, float c)
	{
		float p[] =
		{
			   a, 0.0f, 0.0f, 0.0f,
			0.0f,    b, 0.0f, 0.0f,
			0.0f, 0.0f,    c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		return Matrix(p);
	}

	// CCW
	static Matrix rotation(float angle, float x, float y, float z)
	{
		const auto mag = sqrtf(x * x + y * y + z * z);
		if (mag > 0.0f)
		{
			auto sinAngle = sinDeg(angle);
			auto cosAngle = cosDeg(angle);

			x /= mag;
			y /= mag;
			z /= mag;

			auto xx = x * x;
			auto yy = y * y;
			auto zz = z * z;
			auto xy = x * y;
			auto yz = y * z;
			auto zx = z * x;
			auto xs = x * sinAngle;
			auto ys = y * sinAngle;
			auto zs = z * sinAngle;
			auto oneMinusCos = 1.0f - cosAngle;

			float p[] =
			{
				oneMinusCos * xx + cosAngle,       oneMinusCos * xy - zs,       oneMinusCos * zx + ys,    0.0f,
				      oneMinusCos * xy + zs, oneMinusCos * yy + cosAngle,       oneMinusCos * yz - xs,    0.0f,
				      oneMinusCos * zx - ys,       oneMinusCos * yz + xs, oneMinusCos * zz + cosAngle,    0.0f,
				                       0.0f,                        0.0f,                         0.0f,   1.0f
			};
			return Matrix(p);
		}
		else
		{
			return Matrix::identity();
		}
	}

	static Matrix frustum(float l, float r, float b, float t, float n, float f)
	{
		float p[] =
		{
			2 * n / (r - l),            0.0f,  (r + l) / (r - l),                 0.0f,
				       0.0f, 2 * n / (t - b),  (t + b) / (t - b),                 0.0f,
				       0.0f,            0.0f, -(f + n) / (f - n), -2 * f * n / (f - n),
				       0.0f,            0.0f,              -1.0f,                 0.0f
		};
		return Matrix(p);
	}

	static Matrix ortho(float l, float r, float b, float t, float n, float f)
	{
		float p[] =
		{
			2 / (r - l),        0.0f,         0.0f, -(r + l) / (r - l),
				   0.0f, 2 / (t - b),         0.0f, -(t + b) / (t - b),
				   0.0f,        0.0f, -2 / (f - n), -(f + n) / (f - n),
				   0.0f,        0.0f,         0.0f,               1.0f
		};
		return Matrix(p);
	}

	static Matrix perspective(float fovy, float aspect, float n, float f)
	{
		auto h = tanf(fovy / 360.0f * PI) * n;
		auto w = h * aspect;
		return frustum(-w, w, -h, h, n, f);
	}

};
