#pragma once
////////////////////////////////////////////////////////////
// Filename:			Math.h
// Date:				02/12/2019
// Mod Date:			--
// Mod Initials:		--
// Author:				Tylor Ford
// Purpose:				Defines Math Types 
//						and functions
////////////////////////////////////////////////////////////
#define EPSILON 0.00001f

class CMath
{
	
public:
	// Vector of 2 floats	x, y 
	struct TVECTOR2
	{
		union 
		{
			float mData[2];
			struct
			{
				float x, y;
			};
		};
		TVECTOR2();
		TVECTOR2(float fx, float fy);
	};



	// Vector of 3 floats	x, y, z
	struct TVECTOR3
	{
		union
		{
			float mData[3];
			struct
			{
				float x, y, z;
			};
		};
		TVECTOR3();
		TVECTOR3(float _x, float _y, float _z);

		CMath::TVECTOR3 operator+(const TVECTOR3 V);
		CMath::TVECTOR3 operator-(const TVECTOR3 V);

		CMath::TVECTOR3& operator+=(const TVECTOR3 V);
		CMath::TVECTOR3& operator-=(const TVECTOR3 V);
		CMath::TVECTOR3& operator*=(const TVECTOR3 V);
		CMath::TVECTOR3& operator/=(const TVECTOR3 V);

		CMath::TVECTOR3& operator*=(float S);
		CMath::TVECTOR3& operator/=(float S);

		CMath::TVECTOR3 operator*(const TVECTOR3 V);
		CMath::TVECTOR3 operator/(const TVECTOR3 V);
		CMath::TVECTOR3 operator*(float S);
		CMath::TVECTOR3 operator/(float S);

		CMath::TVECTOR3& operator=(TVECTOR3 v);
		bool operator==(TVECTOR3 V);
	};
	///////////////////////////////////////////////////////////////////
	// Vector3Normailize()		 
	// in:	TVECTOR3
	// out: TVECTOR3
	// returns normalized Vector3 from passed in Vector3
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR3 Vector3Normalize(TVECTOR3 V);
	///////////////////////////////////////////////////////////////////
	// Vector3Dot()		 
	// in:	TVECTOR3, TVECTOR3
	// out: FLOAT
	// returns dot product from two Vector3's
	///////////////////////////////////////////////////////////////////
	static float Vector3Dot(TVECTOR3 V1, TVECTOR3 V2);
	///////////////////////////////////////////////////////////////////
	// Vector3Cross()	 
	// in:	TVECTOR3, TVECTOR3
	// out: TVECTOR3
	// retuns crossed Vector3 from two passed in Vector3's
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR3 Vector3Cross(const TVECTOR3 V1, const TVECTOR3 V2);
	///////////////////////////////////////////////////////////////////
	// Magnitude()	 
	// in:	TVECTOR3
	// out: float
	// retuns a Vector3's magnitude 
	///////////////////////////////////////////////////////////////////
	static float Vector3Magnitude(const TVECTOR3 V);

	// Vector of 4 floats	x, y, z, w
	struct TVECTOR4
	{
		union
		{
			float mData[4];
			struct
			{
				float x, y, z, w;
			};
		};
		TVECTOR4();
		TVECTOR4(float _x, float _y, float _z, float _w);

		CMath::TVECTOR4 operator+(const TVECTOR4 V);
		CMath::TVECTOR4 operator-(const TVECTOR4 V);

		CMath::TVECTOR4& operator+=(const TVECTOR4 V);
		CMath::TVECTOR4& operator-=(const TVECTOR4 V);
		CMath::TVECTOR4& operator*=(const TVECTOR4 V);
		CMath::TVECTOR4& operator/=(const TVECTOR4 V);

		CMath::TVECTOR4& operator*=(float S);
		CMath::TVECTOR4& operator/=(float S);

		CMath::TVECTOR4 operator*(const TVECTOR4 V);
		CMath::TVECTOR4 operator/(const TVECTOR4 V);
		CMath::TVECTOR4 operator*(float S);
		CMath::TVECTOR4 operator/(float S);

		CMath::TVECTOR4& operator=(TVECTOR4 V);
	};

	///////////////////////////////////////////////////////////////////
	// Vector4Normailize()		 
	// in:	TVECTOR4
	// out: TVECTOR4
	// returns normalizes Vector4 from a passed in Vector4
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 Vector4Normalize(TVECTOR4 V);
	///////////////////////////////////////////////////////////////////
	// Vector4Dot()		 
	// in:	TVECTOR4, TVECTOR4
	// out: FLOAT
	// returns dot product from passed in Vector4's
	///////////////////////////////////////////////////////////////////
	static float Vector4Dot(TVECTOR4 V1, TVECTOR4 V2);
	///////////////////////////////////////////////////////////////////
	// Vector4Cross()		 
	// in:	TVECTOR4, TVECTOR4
	// out: TVECTOR4
	// returns crossed Vector4 from two passed in Vector4's
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 Vector4Cross(const TVECTOR4 V1, const TVECTOR4 V2);
	///////////////////////////////////////////////////////////////////
	// Vector4Magnitude()	 
	// in:	TVECTOR3
	// out: float
	// retuns a Vector3's magnitude 
	///////////////////////////////////////////////////////////////////
	static float Vector4Magnitude(const TVECTOR4 V);

	// Matrix with 4 Vector4's (x-axis, y-axis, z-axis, translation) | 16 floats
	struct TMATRIX
	{
		union
		{
			TVECTOR4 r[4];
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float mData[16];
		};
		TMATRIX();
		TMATRIX(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
		TMATRIX(const TVECTOR4 V1, const TVECTOR4 V2, const TVECTOR4 V3, const TVECTOR4 V4);

		CMath::TMATRIX operator+(const TMATRIX M);
		CMath::TMATRIX operator-(const TMATRIX M);

		CMath::TMATRIX& operator+=(const TMATRIX M);
		CMath::TMATRIX& operator-=(const TMATRIX M);
		CMath::TMATRIX& operator*=(const TMATRIX M);

		CMath::TMATRIX operator*(const TMATRIX M);
		CMath::TMATRIX operator*(float S);

		CMath::TMATRIX& operator=(const TMATRIX M);

	};

	///////////////////////////////////////////////////////////////////
	// MatrixDeterminant()		 
	// in:	TMATRIX
	// out: FLOAT
	// returns Matrix determinant
	///////////////////////////////////////////////////////////////////
	static float MatrixDeterminant(TMATRIX M);
	///////////////////////////////////////////////////////////////////
	// MatrixDeterminant()		 
	// in:	9 FLOATS 
	// out: FLOAT
	// returns determinant of passed in floats
	///////////////////////////////////////////////////////////////////
	static float MatrixDeterminant(float e_11, float e_12, float e_13,
		float e_21, float e_22, float e_23,
		float e_31, float e_32, float e_33);
	///////////////////////////////////////////////////////////////////
	// MatrixTranspose()	 
	// in:	TMATRIX
	// out: TMATRIX
	// returns transposed Matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranspose(TMATRIX M);
	///////////////////////////////////////////////////////////////////
	// MatrixXRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on x-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixXRotation(float radian);
	///////////////////////////////////////////////////////////////////
	// MatrixYRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on y-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixYRotation(float radian);
	///////////////////////////////////////////////////////////////////
	// MatrixZRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on z-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixZRotation(float radian);
	///////////////////////////////////////////////////////////////////
	// MatrixInverse()	 
	// in:	TMATRIX
	// out: TMATRIX
	// returns inversed matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixInverse(TMATRIX M);
	///////////////////////////////////////////////////////////////////
	// MatrixIdentity()	 
	// in:	void
	// out: TMATRIX
	// returns an identity Matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixIdentity();
	///////////////////////////////////////////////////////////////////
	// MatrixTranslation()	 
	// in:	float, float, float
	// out: TMATRIX
	// returns a translated Matrix by passed in values
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslation(float X, float Y, float Z);
	///////////////////////////////////////////////////////////////////
	// MatrixTranslationFromVector()	 
	// in:	TVECTOR4
	// out: TMATRIX
	// returns a translated Matrix from a TVECTOR4
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslationFromVector(TVECTOR4 V);
	///////////////////////////////////////////////////////////////////
	// MatrixTranslationFromVector()	 
	// in:	TVECTOR3
	// out: TMATRIX
	// returns a translated Matrix from a TVECTOR3
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslationFromVector(TVECTOR3 V);

	static CMath::TMATRIX MatrixScale(float fX, float fY, float fZ);
	static CMath::TMATRIX MatrixScaleFromVector(TVECTOR3 V);
	///////////////////////////////////////////////////////////////////
	// MatrixRotationQuaternion()	 
	// in:	TVECTOR4
	// out: TMATRIX
	// returns a rotated Matrix from a TVECTOR4
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixRotationQuaternion(TVECTOR4 V);
	///////////////////////////////////////////////////////////////////
	// QuaternionFromAxisAngle()	 
	// in:	TVECTOR3, float
	// out: TVECTOR4
	// returns a quaternion for a rotation of the given degrees around the given axis
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 QuaternionFromAxisAngle(TVECTOR3 axis, float degrees);
	///////////////////////////////////////////////////////////////////
	// QuaternionMultiply()	 
	// in:	TVECTOR4, TVECTOR4
	// out:	TVECTOR4
	// returns the product of two quaternions
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 QuaternionMultiply(TVECTOR4 a, TVECTOR4 b);

	static CMath::TVECTOR3 Vector3Transform(TVECTOR3 tVector, TMATRIX tMatrix);

	static CMath::TVECTOR4 Vector4Transform(TVECTOR4 tVector, TMATRIX tMatrix);
};