#pragma once

#include "RpgTypes.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <cmath>


// Epsilon low precision
#define RPG_MATH_EPS_LP		0.0001f

// Epsilon medium precision
#define RPG_MATH_EPS_MP		0.0000001f

// Epsilon high precision
#define RPG_MATH_EPS_HP		0.0000000001f

// PI
#define RPG_MATH_PI			3.141592653f

// PI/2
#define RPG_MATH_PI_2		1.570796326f

// PI/4
#define RPG_MATH_PI_4		0.785398163f




namespace RpgMath
{
	[[nodiscard]] inline bool FloatEquals(float a, float b, float eps = RPG_MATH_EPS_MP) noexcept
	{
		return fabsf(a - b) <= eps;
	}

	[[nodiscard]] inline bool IsZero(float value, float eps = RPG_MATH_EPS_MP) noexcept
	{
		return FloatEquals(value, 0.0f, eps);
	}

	[[nodiscard]] constexpr inline bool IsPowerOfTwo(uint32_t value) noexcept
	{
		return (value > 0) && !(value & (value - 1));
	}

	[[nodiscard]] constexpr inline float RadToDeg(float rad) noexcept
	{
		return rad * (180.0f / RPG_MATH_PI);
	}

	[[nodiscard]] constexpr inline float DegToRad(float deg) noexcept
	{
		return deg * (RPG_MATH_PI / 180.0f);
	}


	template<typename T>
	[[nodiscard]] constexpr inline T Abs(T value) noexcept
	{
		static_assert(RpgType::IsArithmetic<T>::Value, "Type of <T> must be arithmetic type!");
		return std::abs(value);
	}


	[[nodiscard]] inline float ModF(float a, float b) noexcept
	{
		return fmodf(a, b);
	}


	template<typename T>
	[[nodiscard]] constexpr inline T Clamp(T value, T minValue, T maxValue) noexcept
	{
		static_assert(RpgType::IsArithmetic<T>::Value, "Type of <T> must be arithmetic type!");

		if (value < minValue) return minValue;
		if (value > maxValue) return maxValue;

		return value;
	}


	[[nodiscard]] inline float ClampDegree(float degree) noexcept
	{
		float clamped = ModF(degree + 180.0f, 360.0f);

		if (clamped < 0.0f)
		{
			clamped += 360.0f;
		}

		return clamped - 180.0f;
	}


	template<typename T>
	[[nodiscard]] constexpr inline T Min(T a, T b) noexcept
	{
		static_assert(RpgType::IsArithmetic<T>::Value, "Type of <T> must be arithmetic type!");
		return a < b ? a : b;
	}


	template<typename T>
	[[nodiscard]] constexpr inline T Max(T a, T b) noexcept
	{
		static_assert(RpgType::IsArithmetic<T>::Value, "Type of <T> must be arithmetic type!");
		return a > b ? a : b;
	}


	template<typename T>
	[[nodiscard]] constexpr inline T Sqrt(T value) noexcept
	{
		static_assert(RpgType::IsArithmetic<T>::Value, "Type of <T> must be arithmetic type!");
		return sqrt(value);
	}

};




class RpgVector2
{
public:
	union
	{
		DirectX::XMVECTOR Xmm;

		struct
		{
			float X;
			float Y;
			float Unused0;
			float Unused1;
		};
	};

	static const RpgVector2 ZERO;


public:
	RpgVector2() noexcept
	{
		Xmm = DirectX::XMVectorZero();
	}

	RpgVector2(DirectX::XMVECTOR in_Xmm) noexcept
		: Xmm(in_Xmm)
	{
	}

	explicit RpgVector2(float in_Value) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_Value, in_Value, 0.0f, 0.0f);
	}

	RpgVector2(float in_X, float in_Y) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_X, in_Y, 0.0f, 0.0f);
	}

public:
	inline RpgVector2& operator=(float rhs) noexcept
	{
		Xmm = DirectX::XMVectorSet(rhs, rhs, 0.0f, 0.0f);
		return *this;
	}

	inline RpgVector2 operator+(const RpgVector2& rhs) const noexcept
	{
		return DirectX::XMVectorAdd(Xmm, rhs.Xmm);
	}

	inline RpgVector2 operator+(float rhs) const noexcept
	{
		return DirectX::XMVectorAdd(Xmm, DirectX::XMVectorSet(rhs, rhs, 0.0f, 0.0f));
	}

	inline RpgVector2 operator-(const RpgVector2& rhs) const noexcept
	{
		return DirectX::XMVectorSubtract(Xmm, rhs.Xmm);
	}

	inline RpgVector2 operator-(float rhs) const noexcept
	{
		return DirectX::XMVectorSubtract(Xmm, DirectX::XMVectorSet(rhs, rhs, 0.0f, 0.0f));
	}

	inline RpgVector2 operator*(float rhs) const noexcept
	{
		return DirectX::XMVectorMultiply(Xmm, DirectX::XMVectorSet(rhs, rhs, 0.0f, 0.0f));
	}

};



class RpgVector3
{
public:
	union
	{
		DirectX::XMVECTOR Xmm;

		struct
		{
			float X;
			float Y;
			float Z;
			float Unused;
		};
	};

	static const RpgVector3 ZERO;
	static const RpgVector3 RIGHT;
	static const RpgVector3 LEFT;
	static const RpgVector3 UP;
	static const RpgVector3 DOWN;
	static const RpgVector3 FORWARD;
	static const RpgVector3 BACKWARD;


public:
	RpgVector3() noexcept
	{
		Xmm = DirectX::XMVectorZero();
	}

	RpgVector3(DirectX::XMVECTOR in_Xmm) noexcept
		: Xmm(in_Xmm)
	{
	}

	explicit RpgVector3(float in_Value) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_Value, in_Value, in_Value, 0.0f);
	}

	RpgVector3(float in_X, float in_Y, float in_Z) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_X, in_Y, in_Z, 0.0f);
	}

	explicit RpgVector3(const RpgInt3& in_Int3) noexcept
	{
		Xmm = DirectX::XMVectorSet(static_cast<float>(in_Int3.X), static_cast<float>(in_Int3.Y), static_cast<float>(in_Int3.Z), 0.0f);
	}


public:
	inline RpgVector3& operator=(float rhs) noexcept
	{
		Xmm = DirectX::XMVectorSet(rhs, rhs, rhs, 0.0f);
		return *this;
	}

	inline RpgVector3 operator+(const RpgVector3& rhs) const noexcept
	{
		return DirectX::XMVectorAdd(Xmm, rhs.Xmm);
	}

	inline RpgVector3 operator+(float rhs) const noexcept
	{
		return DirectX::XMVectorAdd(Xmm, DirectX::XMVectorSet(rhs, rhs, rhs, 0.0f));
	}

	inline RpgVector3& operator+=(const RpgVector3& rhs) noexcept
	{
		Xmm = DirectX::XMVectorAdd(Xmm, rhs.Xmm);
		return *this;
	}

	inline RpgVector3 operator-(const RpgVector3& rhs) const noexcept
	{
		return DirectX::XMVectorSubtract(Xmm, rhs.Xmm);
	}

	inline RpgVector3 operator-(float rhs) const noexcept
	{
		return DirectX::XMVectorSubtract(Xmm, DirectX::XMVectorSet(rhs, rhs, rhs, 0.0f));
	}
	
	inline RpgVector3 operator*(float rhs) const noexcept
	{
		return DirectX::XMVectorMultiply(Xmm, DirectX::XMVectorSet(rhs, rhs, rhs, 0.0f));
	}

	inline RpgVector3& operator*=(float rhs) noexcept
	{
		Xmm = DirectX::XMVectorMultiply(Xmm, DirectX::XMVectorSet(rhs, rhs, rhs, 0.0f));
		return *this;
	}


public:
	[[nodiscard]] inline float GetMagnitudeSqr() const noexcept
	{
		float magSqr = 0.0f;
		DirectX::XMStoreFloat(&magSqr, DirectX::XMVector3LengthSq(Xmm));
		return magSqr;
	}

	[[nodiscard]] inline float GetMagnitude() const noexcept
	{
		float mag = 0.0f;
		DirectX::XMStoreFloat(&mag, DirectX::XMVector3Length(Xmm));
		return mag;
	}

	[[nodiscard]] inline void Normalize() noexcept
	{
		Xmm = DirectX::XMVector3Normalize(Xmm);
	}

	[[nodiscard]] inline RpgVector3 GetNormalize() const noexcept
	{
		RpgVector3 result = Xmm;
		result.Normalize();
		return result;
	}


	[[nodiscard]] static inline RpgVector3 Min(const RpgVector3& vecA, const RpgVector3& vecB) noexcept
	{
		return DirectX::XMVectorMin(vecA.Xmm, vecB.Xmm);
	}

	[[nodiscard]] static inline RpgVector3 Max(const RpgVector3& vecA, const RpgVector3& vecB) noexcept
	{
		return DirectX::XMVectorMax(vecA.Xmm, vecB.Xmm);
	}

	[[nodiscard]] static inline float DotProduct(const RpgVector3& vecA, const RpgVector3& vecB) noexcept
	{
		float dot = 0.0f;
		DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(vecA.Xmm, vecB.Xmm));
		return dot;
	}

	[[nodiscard]] static inline RpgVector3 CrossProduct(const RpgVector3& a, const RpgVector3& b) noexcept
	{
		return DirectX::XMVector3Cross(a.Xmm, b.Xmm);
	}

	[[nodiscard]] static inline RpgVector3 Reflect(const RpgVector3& v, const RpgVector3& n) noexcept
	{
		return DirectX::XMVector3Reflect(v.Xmm, n.Xmm);
	}

	[[nodiscard]] static inline RpgVector3 Lerp(const RpgVector3& a, const RpgVector3& b, float t) noexcept
	{
		return DirectX::XMVectorLerp(a.Xmm, b.Xmm, t);
	}

	[[nodiscard]] static inline float DistanceSqr(const RpgVector3& a, const RpgVector3& b) noexcept
	{
		return (b - a).GetMagnitudeSqr();
	}

	[[nodiscard]] static inline float Distance(const RpgVector3& a, const RpgVector3& b) noexcept
	{
		return (b - a).GetMagnitude();
	}

	[[nodiscard]] static inline float AngleBetweenRadian(const RpgVector3& a, const RpgVector3& b) noexcept
	{
		float radian = 0.0f;
		DirectX::XMStoreFloat(&radian, DirectX::XMVector3AngleBetweenVectors(a.Xmm, b.Xmm));
		return radian;
	}

	[[nodiscard]] static inline RpgVector3 ProjectOnNormal(const RpgVector3& v, const RpgVector3& n) noexcept
	{
		return n * DotProduct(v, n);
	}

	[[nodiscard]] static inline RpgVector3 ProjectOnPlane(const RpgVector3& v, const RpgVector3& n) noexcept
	{
		return v - ProjectOnNormal(v, n);
	}

};




class RpgVector4
{
public:
	union
	{
		DirectX::XMVECTOR Xmm;

		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};
	};


public:
	RpgVector4() noexcept
	{
		Xmm = DirectX::XMVectorZero();
	}

	RpgVector4(DirectX::XMVECTOR in_Xmm) noexcept
	{
		Xmm = in_Xmm;
	}

	explicit RpgVector4(float in_Value) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_Value, in_Value, in_Value, in_Value);
	}

	RpgVector4(float in_X, float in_Y, float in_Z, float in_W) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_X, in_Y, in_Z, in_W);
	}

	RpgVector4(const RpgVector3& in_Vec, float in_W = 1.0f) noexcept
	{
		Xmm = in_Vec.Xmm;
		W = in_W;
	}


public:
	inline RpgVector4 operator+(const RpgVector4& rhs) const noexcept
	{
		return DirectX::XMVectorAdd(Xmm, rhs.Xmm);
	}

	inline RpgVector4& operator+=(const RpgVector4& rhs) noexcept
	{
		Xmm = DirectX::XMVectorAdd(Xmm, rhs.Xmm);
		return *this;
	}


public:
	[[nodiscard]] inline void Normalize() noexcept
	{
		Xmm = DirectX::XMVector4Normalize(Xmm);
	}


	[[nodiscard]] inline RpgVector4 GetNormalize() const noexcept
	{
		RpgVector4 result = Xmm;
		result.Normalize();

		return result;
	}


	[[nodiscard]] inline RpgVector3 ToVector3() const noexcept
	{
		return DirectX::XMVectorSet(X, Y, Z, 0.0f);
	}


public:
	[[nodiscard]] static inline RpgVector4 Min(const RpgVector4& vecA, const RpgVector4& vecB) noexcept
	{
		return DirectX::XMVectorMin(vecA.Xmm, vecB.Xmm);
	}

	[[nodiscard]] static inline RpgVector4 Max(const RpgVector4& vecA, const RpgVector4& vecB) noexcept
	{
		return DirectX::XMVectorMax(vecA.Xmm, vecB.Xmm);
	}

};




class RpgQuaternion
{
public:
	DirectX::XMVECTOR Xmm;


public:
	RpgQuaternion() noexcept
	{
		Xmm = DirectX::XMQuaternionIdentity();
	}

	RpgQuaternion(DirectX::XMVECTOR in_Xmm) noexcept
		: Xmm(in_Xmm)
	{
	}

	RpgQuaternion(float in_X, float in_Y, float in_Z, float in_W) noexcept
	{
		Xmm = DirectX::XMVectorSet(in_X, in_Y, in_Z, in_W);
	}

public:
	inline void Normalize() noexcept
	{
		Xmm = DirectX::XMQuaternionNormalize(Xmm);
	}

	[[nodiscard]] RpgQuaternion GetNormalize() const noexcept
	{
		return DirectX::XMQuaternionNormalize(Xmm);
	}

public:
	[[nodiscard]] static inline RpgQuaternion FromPitchYawRollDegree(float pitchDeg, float yawDeg, float rollDeg) noexcept
	{
		return DirectX::XMQuaternionRotationRollPitchYaw(RpgMath::DegToRad(pitchDeg), RpgMath::DegToRad(yawDeg), RpgMath::DegToRad(rollDeg));
	}

	[[nodiscard]] static inline RpgQuaternion FromPitchYawRollDegree(const RpgVector3& pitchYawRollDeg) noexcept
	{
		return DirectX::XMQuaternionRotationRollPitchYaw(RpgMath::DegToRad(pitchYawRollDeg.X), RpgMath::DegToRad(pitchYawRollDeg.Y), RpgMath::DegToRad(pitchYawRollDeg.Z));
	}

	[[nodiscard]] static inline RpgQuaternion Slerp(const RpgQuaternion& a, const RpgQuaternion& b, float t) noexcept
	{
		return DirectX::XMQuaternionSlerp(a.Xmm, b.Xmm, t);
	}

	[[nodiscard]] static inline RpgVector3 RotateVector(const RpgQuaternion& q, const RpgVector3& v) noexcept
	{
		RpgVector3 result = v.GetNormalize();
		result.Xmm = DirectX::XMVector3Rotate(result.Xmm, q.Xmm);
		return result;
	}

};




class RpgMatrixTransform
{
public:
	DirectX::XMMATRIX Xmm;


public:
	RpgMatrixTransform() noexcept
	{
		Xmm = DirectX::XMMatrixIdentity();
	}

	RpgMatrixTransform(const DirectX::XMMATRIX& in_Xmm) noexcept
		: Xmm(in_Xmm)
	{
	}

	RpgMatrixTransform(const RpgVector3& position, const RpgQuaternion& rotation, const RpgVector3& scale = RpgVector3(1.0f)) noexcept
	{
		Xmm = DirectX::XMMatrixAffineTransformation(scale.Xmm, DirectX::XMQuaternionIdentity(), rotation.Xmm, position.Xmm);
	}


public:
	inline RpgMatrixTransform operator*(const RpgMatrixTransform& rhs) const noexcept
	{
		return DirectX::XMMatrixMultiply(Xmm, rhs.Xmm);
	}

	inline RpgMatrixTransform& operator*=(const RpgMatrixTransform& rhs) noexcept
	{
		Xmm = DirectX::XMMatrixMultiply(Xmm, rhs.Xmm);
		return *this;
	}


public:
	inline void SetScale(float scale) noexcept
	{
		Xmm = DirectX::XMMatrixScaling(scale, scale, scale);
	}

	inline RpgVector4 TransformVector(const RpgVector4& vec) const noexcept
	{
		return DirectX::XMVector4Transform(vec.Xmm, Xmm);
	}

	inline void TransposeInPlace() noexcept
	{
		Xmm = DirectX::XMMatrixTranspose(Xmm);
	}

	[[nodiscard]] inline RpgMatrixTransform GetTranspose() const noexcept
	{
		return DirectX::XMMatrixTranspose(Xmm);
	}

	inline void InverseInPlace() noexcept
	{
		Xmm = DirectX::XMMatrixInverse(nullptr, Xmm);
	}

	[[nodiscard]] RpgMatrixTransform GetInverse() const noexcept
	{
		RpgMatrixTransform result = Xmm;
		result.InverseInPlace();

		return result;
	}

	inline void Decompose(RpgVector3& out_Position, RpgQuaternion& out_Rotation, RpgVector3& out_Scale) const noexcept
	{
		DirectX::XMMatrixDecompose(&out_Scale.Xmm, &out_Rotation.Xmm, &out_Position.Xmm, Xmm);
	}

	[[nodiscard]] inline RpgVector3 GetPosition() const noexcept
	{
		return Xmm.r[3];
	}

};


inline RpgVector3 operator*(const RpgVector3& lhs, const RpgMatrixTransform& rhs) noexcept
{
	return DirectX::XMVector3Transform(lhs.Xmm, rhs.Xmm);
}



class RpgMatrixProjection
{
public:
	DirectX::XMMATRIX Xmm;


public:
	RpgMatrixProjection() noexcept
	{
		Xmm = DirectX::XMMatrixIdentity();
	}

public:
	[[nodiscard]] static inline RpgMatrixProjection CreatePerspective(float aspect, float fovDegree, float nearClipZ, float farClipZ) noexcept
	{
		RpgMatrixProjection perspective;
		perspective.Xmm = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fovDegree), aspect, nearClipZ, farClipZ);

		return perspective;
	}

	[[nodiscard]] static inline RpgMatrixProjection CreateOrthographic(float left, float right, float top, float bottom, float nearClipZ, float farClipZ) noexcept
	{
		RpgMatrixProjection orthographic;
		orthographic.Xmm = DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearClipZ, farClipZ);

		return orthographic;
	}

};




class RpgMatrixViewProjection
{
public:
	DirectX::XMMATRIX Xmm;


public:
	RpgMatrixViewProjection() noexcept
	{
		Xmm = DirectX::XMMatrixIdentity();
	}

	RpgMatrixViewProjection(const RpgMatrixTransform& viewMatrix, const RpgMatrixProjection& projectionMatrix) noexcept
	{
		Xmm = DirectX::XMMatrixMultiply(viewMatrix.Xmm, projectionMatrix.Xmm);
	}

	inline void Init(const RpgMatrixTransform& viewMatrix, const RpgMatrixProjection& projectionMatrix) noexcept
	{
		Xmm = DirectX::XMMatrixMultiply(viewMatrix.Xmm, projectionMatrix.Xmm);
	}

};



class RpgTransform
{
public:
	RpgVector3 Scale;
	RpgQuaternion Rotation;
	RpgVector3 Position;


public:
	RpgTransform() noexcept
		: Scale(1.0f)
	{
	}

	RpgTransform(const RpgVector3& in_Position, const RpgQuaternion& in_Rotation, const RpgVector3& in_Scale = RpgVector3(1.0f)) noexcept
		: Position(in_Position)
		, Rotation(in_Rotation)
		, Scale(in_Scale)
	{
	}

	RpgTransform(const RpgMatrixTransform& matrix) noexcept
	{
		matrix.Decompose(Position, Rotation, Scale);
	}


public:
	[[nodiscard]] inline RpgMatrixTransform ToMatrixTransform() const noexcept
	{
		return RpgMatrixTransform(Position, Rotation, Scale);
	}

	[[nodiscard]] inline RpgVector3 GetAxisRight() const noexcept
	{
		return RpgQuaternion::RotateVector(Rotation, RpgVector3::RIGHT);
	}

	[[nodiscard]] inline RpgVector3 GetAxisUp() const noexcept
	{
		return RpgQuaternion::RotateVector(Rotation, RpgVector3::UP);
	}

	[[nodiscard]] inline RpgVector3 GetAxisForward() const noexcept
	{
		return RpgQuaternion::RotateVector(Rotation, RpgVector3::FORWARD);
	}

};




// =============================================================================================================================================================== //
// LINE
// =============================================================================================================================================================== //
class RpgLine
{
public:
	RpgVector3 A;
	RpgVector3 B;


public:
	RpgLine() noexcept = default;


	RpgLine(const RpgVector3& inA, const RpgVector3& inB) noexcept
		: A(inA)
		, B(inB)
	{
	}


public:
	[[nodiscard]] inline RpgVector3 GetDirection() const noexcept
	{
		RpgVector3 direction = B - A;
		direction.Normalize();
		return direction;
	}

	[[nodiscard]] inline float GetLength() const noexcept
	{
		return RpgVector3::Distance(A, B);
	}


	[[nodiscard]] inline float GetShortestDistancePoint(const RpgVector3& point) const noexcept
	{
		float distance = FLT_MAX;
		DirectX::XMStoreFloat(&distance, DirectX::XMVector3LinePointDistance(A.Xmm, B.Xmm, point.Xmm));
		return distance;
	}

	[[nodiscard]] inline float GetShortestDistanceLine(const RpgLine& line) const noexcept
	{
		const RpgVector3 vecA = GetDirection();
		const RpgVector3 vecB = line.GetDirection();
		const RpgVector3 AxB = RpgVector3::CrossProduct(vecA, vecB);
		const float mag = AxB.GetMagnitude();
		const RpgVector3 AB = line.A - A;
		float distance = 0.0f;

		if (mag > 0.0f)
		{
			const float dot = RpgVector3::DotProduct(AxB, AB);
			distance = RpgMath::Abs(dot / mag);
		}
		else
		{
			const RpgVector3 cross = RpgVector3::CrossProduct(vecA, AB);
			distance = cross.GetMagnitude() / vecA.GetMagnitude();
		}

		return distance;
	}

	inline RpgVector3 ProjectPoint(const RpgVector3& point, float* optOut_t = nullptr) const noexcept
	{
		const RpgVector3 AB = B - A;
		const RpgVector3 AP = point - A;
		const float t = RpgVector3::DotProduct(AP, AB) / AB.GetMagnitudeSqr();

		if (optOut_t)
		{
			*optOut_t = t;
		}

		return A + AB * t;
	}

};




// =============================================================================================================================================================== //
// PLANE
// =============================================================================================================================================================== //
class RpgPlane
{
public:
	// Normal (X, Y, Z) Distance (W)
	RpgVector4 Plane;


public:
	RpgPlane() noexcept = default;

	RpgPlane(RpgVector3 normal, float distance) noexcept
		: Plane(normal.X, normal.Y, normal.Z, distance)
	{
	}

	RpgPlane(RpgVector3 normal, RpgVector3 point) noexcept
	{
		Plane.Xmm = DirectX::XMPlaneFromPointNormal(point.Xmm, normal.Xmm);
	}


public:
	[[nodiscard]] inline float DotProduct(const RpgVector3& normal) const noexcept
	{
		float dot = 0.0f;
		DirectX::XMStoreFloat(&dot, DirectX::XMPlaneDotNormal(Plane.Xmm, normal.Xmm));
		return dot;
	}


	[[nodiscard]] inline RpgVector3 GetNormal() const noexcept
	{
		return Plane.ToVector3();
	}


	[[nodiscard]] inline bool TestIntersectRay(RpgVector3& out_IntersectionPoint, RpgVector3 rayOrigin, RpgVector3 rayDirection, float* optOut_t = nullptr) const noexcept
	{
		const RpgVector3 planeNormal = RpgVector3(Plane.X, Plane.Y, Plane.Z);
		const float denom = RpgVector3::DotProduct(rayDirection, planeNormal);

		if (RpgMath::IsZero(denom))
		{
			return false;
		}

		const RpgVector3 planeOrigin = planeNormal * Plane.W;
		const float t = RpgVector3::DotProduct(planeOrigin - rayOrigin, planeNormal) / denom;
		out_IntersectionPoint = rayOrigin + rayDirection * t;

		if (optOut_t)
		{
			*optOut_t = t;
		}

		return true;
	}


	[[nodiscard]] inline RpgVector3 GetLineIntersectionPoint(RpgVector3 start, RpgVector3 end) const noexcept
	{
		return DirectX::XMPlaneIntersectLine(Plane.Xmm, start.Xmm, end.Xmm);
	}

	[[nodiscard]] inline RpgVector3 GetLineIntersectionPoint(const RpgLine& line) const noexcept
	{
		return DirectX::XMPlaneIntersectLine(Plane.Xmm, line.A.Xmm, line.B.Xmm);
	}

};




// =============================================================================================================================================================== //
// BOUNDING - SPHERE
// =============================================================================================================================================================== //
class RpgBoundingSphere
{
public:
	RpgVector3 Center;
	float Radius;


public:
	RpgBoundingSphere() noexcept = default;

	RpgBoundingSphere(RpgVector3 inCenter, float inRadius) noexcept
		: Center(inCenter)
		, Radius(inRadius)
	{
	}


	inline bool TestIntersectRay(const RpgVector3& rayOrigin, const RpgVector3& rayDirection, float* outDistance = nullptr) const noexcept
	{
		DirectX::BoundingSphere dxSphere;
		DirectX::XMStoreFloat3(&dxSphere.Center, Center.Xmm);
		dxSphere.Radius = Radius;

		float distance = 999999999.0f;
		const bool bIntersect = dxSphere.Intersects(rayOrigin.Xmm, rayDirection.Xmm, distance);

		if (outDistance)
		{
			*outDistance = distance;
		}

		return bIntersect;
	}

};




// =============================================================================================================================================================== //
// BOUNDING - AABB
// =============================================================================================================================================================== //
class RpgBoundingAABB
{
public:
	RpgVector3 Min;
	RpgVector3 Max;


public:
	RpgBoundingAABB() noexcept = default;

	RpgBoundingAABB(RpgVector3 in_Min, RpgVector3 in_Max) noexcept
		: Min(in_Min)
		, Max(in_Max)
	{
	}


	[[nodiscard]] inline RpgVector3 GetCenter() const noexcept
	{
		return (Min + Max) * 0.5f;
	}

	[[nodiscard]] inline RpgVector3 GetHalfExtents() const noexcept
	{
		return (Max - Min) * 0.5f;
	}

	[[nodiscard]] inline float GetDiagonalLength() const noexcept
	{
		return (Max - Min).GetMagnitude();
	}


	[[nodiscard]] inline bool TestIntersectBoundingAABB(const RpgBoundingAABB& other) const noexcept
	{
		DirectX::BoundingBox first;
		DirectX::XMStoreFloat3(&first.Center, GetCenter().Xmm);
		DirectX::XMStoreFloat3(&first.Extents, GetHalfExtents().Xmm);

		DirectX::BoundingBox second;
		DirectX::XMStoreFloat3(&second.Center, other.GetCenter().Xmm);
		DirectX::XMStoreFloat3(&second.Extents, other.GetHalfExtents().Xmm);

		return first.Intersects(second);
	}

};




// =============================================================================================================================================================== //
// BOUNDING - BOX
// =============================================================================================================================================================== //
class RpgBoundingBox
{
public:
	// 8 corners position of bounding box.
	//     Near    Far
	//    0----1  4----5
	//    |    |  |    |
	//    |    |  |    |
	//    3----2  7----6
	struct FCornerPoints
	{
		RpgVector3 Points[8];
	};


public:
	RpgVector3 Center;
	RpgVector3 HalfExtents;
	RpgQuaternion Rotation;


public:
	RpgBoundingBox() noexcept = default;


	RpgBoundingBox(RpgVector3 inCenter, RpgVector3 inHalfExtents, RpgQuaternion inRotation) noexcept
		: Center(inCenter)
		, HalfExtents(inHalfExtents)
		, Rotation(inRotation)
	{
	}

	RpgBoundingBox(const RpgBoundingAABB& aabb, const RpgMatrixTransform& transformMatrix) noexcept
	{
		DirectX::BoundingOrientedBox dxBox;
		DirectX::XMStoreFloat3(&dxBox.Center, aabb.GetCenter().Xmm);
		DirectX::XMStoreFloat3(&dxBox.Extents, aabb.GetHalfExtents().Xmm);
		DirectX::XMStoreFloat4(&dxBox.Orientation, RpgQuaternion().Xmm);

		DirectX::BoundingOrientedBox dxBoxTransformed;
		dxBox.Transform(dxBoxTransformed, transformMatrix.Xmm);

		Center = DirectX::XMLoadFloat3(&dxBoxTransformed.Center);
		HalfExtents = DirectX::XMLoadFloat3(&dxBoxTransformed.Extents);
		Rotation = DirectX::XMLoadFloat4(&dxBoxTransformed.Orientation);
	}


	[[nodiscard]] inline FCornerPoints GetCornerPoints() const noexcept
	{
		DirectX::BoundingOrientedBox box;
		DirectX::XMStoreFloat3(&box.Center, Center.Xmm);
		DirectX::XMStoreFloat3(&box.Extents, HalfExtents.Xmm);
		DirectX::XMStoreFloat4(&box.Orientation, Rotation.Xmm);

		DirectX::XMFLOAT3 tempPoints[8];
		box.GetCorners(tempPoints);

		FCornerPoints corners;
		corners.Points[0] = DirectX::XMLoadFloat3(&tempPoints[0]);
		corners.Points[1] = DirectX::XMLoadFloat3(&tempPoints[1]);
		corners.Points[2] = DirectX::XMLoadFloat3(&tempPoints[2]);
		corners.Points[3] = DirectX::XMLoadFloat3(&tempPoints[3]);
		corners.Points[4] = DirectX::XMLoadFloat3(&tempPoints[4]);
		corners.Points[5] = DirectX::XMLoadFloat3(&tempPoints[5]);
		corners.Points[6] = DirectX::XMLoadFloat3(&tempPoints[6]);
		corners.Points[7] = DirectX::XMLoadFloat3(&tempPoints[7]);

		return corners;
	}


	[[nodiscard]] inline RpgBoundingAABB ToAABB() const noexcept
	{
		const FCornerPoints corners = GetCornerPoints();

		RpgVector3 boxMin(FLT_MAX);
		RpgVector3 boxMax(-FLT_MAX);

		boxMin = RpgVector3::Min(boxMin, corners.Points[0]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[0]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[1]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[1]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[2]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[2]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[3]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[3]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[4]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[4]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[5]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[5]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[6]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[6]);

		boxMin = RpgVector3::Min(boxMin, corners.Points[7]);
		boxMax = RpgVector3::Max(boxMax, corners.Points[7]);

		return RpgBoundingAABB(boxMin, boxMax);
	}

};




// =============================================================================================================================================================== //
// BOUNDING - CAPSULE
// =============================================================================================================================================================== //
class RpgBoundingCapsule
{
public:
	RpgVector3 Center;
	float HalfHeight;
	float Radius;


public:
	RpgBoundingCapsule() noexcept
	{
		HalfHeight = 0.0f;
		Radius = 0.0f;
	}

	RpgBoundingCapsule(const RpgVector3& center, float halfHeight, float radius) noexcept
	{
		Center = center;
		HalfHeight = halfHeight;
		Radius = radius;
	}


	[[nodiscard]] inline RpgVector3 GetCenterBottomSphere() const noexcept
	{
		return Center + RpgVector3::DOWN * HalfHeight;
	}

	[[nodiscard]] inline RpgVector3 GetCenterTopSphere() const noexcept
	{
		return Center + RpgVector3::UP * HalfHeight;
	}

	[[nodiscard]] inline RpgVector3 GetFootPosition() const noexcept
	{
		return Center + RpgVector3::DOWN * (HalfHeight + Radius);
	}

	[[nodiscard]] inline float GetCapsuleHeight() const noexcept
	{
		return (HalfHeight + Radius) * 2.0f;
	}

};




// =============================================================================================================================================================== //
// BOUNDING - FRUSTUM
// =============================================================================================================================================================== //
class RpgBoundingFrustum
{
public:
	// 8 corners position of bounding frustum.
	//     Near    Far
	//    0----1  4----5
	//    |    |  |    |
	//    |    |  |    |
	//    3----2  7----6
	struct FCornerPoints
	{
		RpgVector3 Points[8];
	};


private:
	DirectX::BoundingFrustum DxFrustum;


public:
	RpgBoundingFrustum() noexcept = default;

	RpgBoundingFrustum(const RpgMatrixTransform& transformMatrix, const RpgMatrixProjection& projectionMatrix) noexcept
	{
		CreateFromMatrix(transformMatrix, projectionMatrix);
	}


	inline void CreateFromMatrix(const RpgMatrixTransform& transformMatrix, const RpgMatrixProjection& projectionMatrix) noexcept
	{
		DirectX::BoundingFrustum::CreateFromMatrix(DxFrustum, projectionMatrix.Xmm);
		DxFrustum.Transform(DxFrustum, transformMatrix.Xmm);
	}


	[[nodiscard]] inline FCornerPoints GetCornerPoints() const noexcept
	{
		DirectX::XMFLOAT3 tempPoints[8];
		DxFrustum.GetCorners(tempPoints);

		FCornerPoints corners;
		corners.Points[0] = DirectX::XMLoadFloat3(&tempPoints[0]);
		corners.Points[1] = DirectX::XMLoadFloat3(&tempPoints[1]);
		corners.Points[2] = DirectX::XMLoadFloat3(&tempPoints[2]);
		corners.Points[3] = DirectX::XMLoadFloat3(&tempPoints[3]);
		corners.Points[4] = DirectX::XMLoadFloat3(&tempPoints[4]);
		corners.Points[5] = DirectX::XMLoadFloat3(&tempPoints[5]);
		corners.Points[6] = DirectX::XMLoadFloat3(&tempPoints[6]);
		corners.Points[7] = DirectX::XMLoadFloat3(&tempPoints[7]);

		return corners;
	}


	[[nodiscard]] inline bool TestIntersectBoundingSphere(const RpgBoundingSphere& boundingSphere) const noexcept
	{
		DirectX::BoundingSphere dxSphere;
		DirectX::XMStoreFloat3(&dxSphere.Center, boundingSphere.Center.Xmm);
		dxSphere.Radius = boundingSphere.Radius;

		return DxFrustum.Intersects(dxSphere);
	}

	[[nodiscard]] inline bool TestIntersectBoundingAABB(const RpgBoundingAABB& boundingAABB) const noexcept
	{
		DirectX::XMFLOAT3 aabbCenter;
		DirectX::XMStoreFloat3(&aabbCenter, boundingAABB.GetCenter().Xmm);

		DirectX::XMFLOAT3 aabbHalfExtents;
		DirectX::XMStoreFloat3(&aabbHalfExtents, boundingAABB.GetHalfExtents().Xmm);

		return DxFrustum.Intersects(DirectX::BoundingBox(aabbCenter, aabbHalfExtents));
	}

	[[nodiscard]] inline bool TestIntersectBoundingBox(const RpgBoundingBox& boundingBox) const noexcept
	{
		DirectX::BoundingOrientedBox box;
		DirectX::XMStoreFloat3(&box.Center, boundingBox.Center.Xmm);
		DirectX::XMStoreFloat3(&box.Extents, boundingBox.HalfExtents.Xmm);
		DirectX::XMStoreFloat4(&box.Orientation, boundingBox.Rotation.Xmm);

		return DxFrustum.Intersects(box);
	}

	[[nodiscard]] inline bool TestIntersectPlane(const RpgPlane& plane) const noexcept
	{
		return false;
	}

	[[nodiscard]] inline bool TestIntersectFrustum(const RpgBoundingFrustum& frustum) const noexcept
	{
		return DxFrustum.Intersects(frustum.DxFrustum);
	}

};

