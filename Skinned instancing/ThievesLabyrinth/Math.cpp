#include "Math.h"
#include <math.h>

//I'm not going to go over this one. Update the var names

CMath::TVECTOR3::TVECTOR3()
{
	x = y = z = 0;
}

CMath::TVECTOR3::TVECTOR3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator+(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x + V.x;
	result.y = y + V.y;
	result.z = z + V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator-(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x - V.x;
	result.y = y - V.y;
	result.z = z - V.z;
	return result;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator+=(const TVECTOR3 V)
{
	x += V.x;
	y += V.y;
	z += V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator-=(const TVECTOR3 V)
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator*=(const TVECTOR3 V)
{
	x *= V.x;
	y *= V.y;
	z *= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator/=(const TVECTOR3 V)
{
	x /= V.x;
	y /= V.y;
	z /= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator*=(float S)
{
	x *= S;
	y *= S;
	z *= S;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator/=(float S)
{
	x /= S;
	y /= S;
	z /= S;
	return *this;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator*(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x * V.x;
	result.y = y * V.y;
	result.z = z * V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator/(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x / V.x;
	result.y = y / V.y;
	result.z = z / V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator*(float S)
{
	TVECTOR3 result;
	result.x = x * S;
	result.y = y * S;
	result.z = z * S;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator/(float S)
{
	TVECTOR3 result;
	result.x = x / S;
	result.y = y / S;
	result.z = z / S;
	return result;
}

CMath::TVECTOR3& CMath::TVECTOR3::operator=(TVECTOR3 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	return *this;
}

bool CMath::TVECTOR3::operator==(TVECTOR3 V)
{
	if (fabs(x - V.x) < EPSILON &&
		fabs(y - V.y) < EPSILON && 
		fabs(z - V.z) < EPSILON)
	{
		return true;
	}
	return false;
}

CMath::TVECTOR3 CMath::Vector3Normalize(TVECTOR3 V)
{
	float dot = Vector3Magnitude(V);
	if (fabsf(dot) > EPSILON)
	{
		dot = 1 / dot;
		TVECTOR3 result = V;
		result.x *= dot;
		result.y *= dot;
		result.z *= dot;
		return result;
	}
	return V;
}

float CMath::Vector3Dot(TVECTOR3 V1, TVECTOR3 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z);
}

CMath::TVECTOR3 CMath::Vector3Cross(const TVECTOR3 V1, const TVECTOR3 V2)
{
	TVECTOR3 result;
	result.x = V1.y * V2.z - (V1.z * V2.y);
	result.y = V1.z * V2.x - (V1.x * V2.z);
	result.z = V1.x * V2.y - (V1.y * V2.x);
	return result;
}

float CMath::Vector3Magnitude(const TVECTOR3 V)
{
	return sqrtf(Vector3Dot(V, V));
}

CMath::TVECTOR4::TVECTOR4()
{
	x = y = z = w = 0;
}

CMath::TVECTOR4::TVECTOR4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator+(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x + V.x;
	result.y = y + V.y;
	result.z = z + V.z;
	result.w = w + V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator-(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x - V.x;
	result.y = y - V.y;
	result.z = z - V.z;
	result.w = w - V.w;
	return result;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator+=(const TVECTOR4 V)
{
	x += V.x;
	y += V.y;
	z += V.z;
	w += V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator-=(const TVECTOR4 V)
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
	w -= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator*=(const TVECTOR4 V)
{
	x *= V.x;
	y *= V.y;
	z *= V.z;
	w *= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator/=(const TVECTOR4 V)
{
	x /= V.x;
	y /= V.y;
	z /= V.z;
	w /= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator*=(float S)
{
	x *= S;
	y *= S;
	z *= S;
	w *= S;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator/=(float S)
{
	x /= S;
	y /= S;
	z /= S;
	w /= S;
	return *this;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator*(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x * V.x;
	result.y = y * V.y;
	result.z = z * V.z;
	result.w = w * V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator/(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x / V.x;
	result.y = y / V.y;
	result.z = z / V.z;
	result.w = w / V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator*(float S)
{
	TVECTOR4 result;
	result.x = x * S;
	result.y = y * S;
	result.z = z * S;
	result.w = w * S;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator/(float S)
{
	TVECTOR4 result;
	result.x = x / S;
	result.y = y / S;
	result.z = z / S;
	result.w = w / S;
	return result;
}

CMath::TVECTOR4& CMath::TVECTOR4::operator=(TVECTOR4 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	w = V.w;
	return *this;
}

CMath::TVECTOR4 CMath::Vector4Normalize(TVECTOR4 V)
{
	float dot = Vector4Magnitude(V);
	if (fabsf(dot) > EPSILON)
	{
		dot = 1 / dot;
		TVECTOR4 result = V;
		result.x *= dot;
		result.y *= dot;
		result.z *= dot;
		result.x *= dot;
		return result;
	}
	return V;
}

float CMath::Vector4Dot(TVECTOR4 V1, TVECTOR4 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + V1.w * V2.w);
}

CMath::TVECTOR4 CMath::Vector4Cross(const TVECTOR4 V1, const TVECTOR4 V2)
{
	TVECTOR4 result;
	result.x = V1.y * V2.z - (V1.z * V2.y);
	result.y = V1.z * V2.x - (V1.x * V2.z);
	result.z = V1.x * V2.y - (V1.y * V2.x);
	result.w = 0;
	return result;
}

float CMath::Vector4Magnitude(const TVECTOR4 V)
{
	return sqrtf(Vector4Dot(V, V));
}

CMath::TMATRIX::TMATRIX()
{
	_11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = 0;
}

CMath::TMATRIX::TMATRIX(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	_11 = m00;
	_12 = m01;
	_13 = m02;
	_14 = m03;
	_21 = m10;
	_22 = m11;
	_23 = m12;
	_24 = m13;
	_31 = m20;
	_32 = m21;
	_33 = m22;
	_34 = m23;
	_41 = m30;
	_42 = m31;
	_43 = m32;
	_44 = m33;
}

CMath::TMATRIX::TMATRIX(const TVECTOR4 V1, const TVECTOR4 V2, const TVECTOR4 V3, const TVECTOR4 V4)
{
	r[0] = V1;
	r[1] = V2;
	r[2] = V3;
	r[3] = V4;
}

CMath::TMATRIX CMath::TMATRIX::operator+(const TMATRIX M)
{
	CMath::TMATRIX result;
	result.r[0] = r[0] + M.r[0];
	result.r[1] = r[1] + M.r[1];
	result.r[2] = r[2] + M.r[2];
	result.r[3] = r[3] + M.r[3];
	return result;
}

CMath::TMATRIX CMath::TMATRIX::operator-(const TMATRIX M)
{
	CMath::TMATRIX result;
	result.r[0] = r[0] - M.r[0];
	result.r[1] = r[1] - M.r[1];
	result.r[2] = r[2] - M.r[2];
	result.r[3] = r[3] - M.r[3];
	return result;
}

CMath::TMATRIX & CMath::TMATRIX::operator+=(const TMATRIX M)
{
	r[0] += M.r[0];
	r[1] += M.r[1];
	r[2] += M.r[2];
	r[3] += M.r[3];
	return *this;
}

CMath::TMATRIX & CMath::TMATRIX::operator-=(const TMATRIX M)
{
	r[0] -= M.r[0];
	r[1] -= M.r[1];
	r[2] -= M.r[2];
	r[3] -= M.r[3];
	return *this;
}

CMath::TMATRIX & CMath::TMATRIX::operator*=(const TMATRIX M)
{
	for (int row = 0; row < 16; row += 4)
	{
		for (int col = 0; col < 4; col++)
		{
			mData[col + row] = M.mData[row] * mData[col] + M.mData[row + 1] * mData[col + 4] + M.mData[row + 2] * mData[col + 8] + M.mData[row + 3] * mData[col + 12];
		}
	}
	return *this;
}

CMath::TMATRIX CMath::TMATRIX::operator*(const TMATRIX M)
{
	TMATRIX result;
	for (int row = 0; row < 16; row += 4)
	{
		for (int col = 0; col < 4; col++)
		{
			result.mData[col + row] = M.mData[row] * mData[col] + M.mData[row + 1] * mData[col + 4] + M.mData[row + 2] * mData[col + 8] + M.mData[row + 3] * mData[col + 12];
		}
	}
	return result;
}

CMath::TMATRIX CMath::TMATRIX::operator*(float S)
{
	TMATRIX result;
	result.r[0] = r[0] * S;
	result.r[1] = r[1] * S;
	result.r[2] = r[2] * S;
	result.r[3] = r[3] * S;
	return result;
}

CMath::TMATRIX & CMath::TMATRIX::operator=(const TMATRIX M)
{
	r[0] = M.r[0];
	r[1] = M.r[1];
	r[2] = M.r[2];
	r[3] = M.r[3];
	return *this;
}

float CMath::MatrixDeterminant(TMATRIX M)
{
	return
		M._11 * (M._22 * (M._33 * M._44 - M._43 * M._34) - M._23 * (M._32 * M._44 - M._42 * M._34) + M._24 * (M._32 * M._43 - M._42 * M._33)) -
		M._21 * (M._12 * (M._33 * M._44 - M._43 * M._34) - M._13 * (M._32 * M._44 - M._42 * M._34) + M._14 * (M._32 * M._43 - M._42 * M._33)) +
		M._31 * (M._12 * (M._23 * M._44 - M._43 * M._24) - M._13 * (M._22 * M._44 - M._42 * M._24) + M._14 * (M._22 * M._43 - M._42 * M._23)) -
		M._41 * (M._12 * (M._23 * M._34 - M._33 * M._24) - M._13 * (M._22 * M._34 - M._32 * M._24) + M._14 * (M._22 * M._33 - M._32 * M._23));

}

float CMath::MatrixDeterminant(float e_11, float e_12, float e_13, float e_21, float e_22, float e_23, float e_31, float e_32, float e_33)
{
	return e_11 * (e_22 * e_33 - e_32 * e_23) - e_12 * (e_21 * e_33 - e_31 * e_23) + e_13 * (e_21 * e_32 - e_31 * e_22);;
}

CMath::TMATRIX CMath::MatrixTranspose(TMATRIX M)
{
	return { M._11,M._21,M._31,M._41,
			M._12,M._22,M._32,M._42,
			M._13,M._23,M._33,M._43,
			M._14,M._24,M._34,M._44 };
}

CMath::TMATRIX CMath::MatrixXRotation(float radian)
{
	return { 1,0,0,0,
			 0,cosf(radian),-sinf(radian),0,
			 0,sinf(radian),cosf(radian),0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixYRotation(float radian)
{
	return { cosf(radian),0,sinf(radian),0,
			 0	,	1	,	0	,	0,
			 -sinf(radian),0,cosf(radian),0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixZRotation(float radian)
{
	return { cosf(radian),-sinf(radian),0,0,
			 sinf(radian),cosf(radian),0,0,
			 0,0,1,0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixInverse(TMATRIX M)
{
	float det = MatrixDeterminant(M);
	if (fabs(det) < EPSILON)
		return M;
	TMATRIX a_transposed
	{
		 MatrixDeterminant(M._22, M._23, M._24, M._32, M._33, M._34, M._42, M._43, M._44),
		-MatrixDeterminant(M._12, M._13, M._14, M._32, M._33, M._34, M._42, M._43, M._44),
		 MatrixDeterminant(M._12, M._13, M._14, M._22, M._23, M._24, M._42, M._43, M._44),
		-MatrixDeterminant(M._12, M._13, M._14, M._22, M._23, M._24, M._32, M._33, M._34),

		-MatrixDeterminant(M._21, M._23, M._24, M._31, M._33, M._34, M._41, M._43, M._44),
		 MatrixDeterminant(M._11, M._13, M._14, M._31, M._33, M._34, M._41, M._43, M._44),
		-MatrixDeterminant(M._11, M._13, M._14, M._21, M._23, M._24, M._41, M._43, M._44),
		 MatrixDeterminant(M._11, M._13, M._14, M._21, M._23, M._24, M._31, M._33, M._34),

		 MatrixDeterminant(M._21, M._22, M._24, M._31, M._32, M._34, M._41, M._42, M._44),
		-MatrixDeterminant(M._11, M._12, M._14, M._31, M._32, M._34, M._41, M._42, M._44),
		 MatrixDeterminant(M._11, M._12, M._14, M._21, M._22, M._24, M._41, M._42, M._44),
		-MatrixDeterminant(M._11, M._12, M._14, M._21, M._22, M._24, M._31, M._32, M._34),

		-MatrixDeterminant(M._21, M._22, M._23, M._31, M._32, M._33, M._41, M._42, M._43),
		 MatrixDeterminant(M._11, M._12, M._13, M._31, M._32, M._33, M._41, M._42, M._43),
		-MatrixDeterminant(M._11, M._12, M._13, M._21, M._22, M._23, M._41, M._42, M._43),
		 MatrixDeterminant(M._11, M._12, M._13, M._21, M._22, M._23, M._31, M._32, M._33)
	};
	return a_transposed * (1 / det);
}

CMath::TMATRIX CMath::MatrixIdentity()
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixTranslation(float X, float Y, float Z)
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		X,Y,Z,1
	};
}

CMath::TMATRIX CMath::MatrixTranslationFromVector(TVECTOR4 V)
{
	return
		CMath::TMATRIX{
			1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		V.x,V.y,V.z,1
	};
}

CMath::TMATRIX CMath::MatrixTranslationFromVector(TVECTOR3 V)
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		V.x,V.y,V.z,1
	};
}

CMath::TMATRIX CMath::MatrixScale(float fX, float fY, float fZ)
{
	return
		CMath::TMATRIX{
		fX,0,0,0,
		0,fY,0,0,
		0,0,fZ,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixScaleFromVector(TVECTOR3 V)
{
	return
		CMath::TMATRIX{
		V.x,0,0,0,
		0,V.y,0,0,
		0,0,V.z,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixRotationQuaternion(TVECTOR4 V)
{
	CMath::TMATRIX result;

//#define REALQUATERNION
#ifdef  REALQUATERNION
		//CHANGED
	CMath::TVECTOR4 v = V; CMath::Vector4Normalize(V);

	float x = v.x, y = v.y, z = v.z, w = v.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	result.mData[0] = 1.0f - (yy + zz);
	result.mData[1] = xy - wz;
	result.mData[2] = xz + wy;

	result.mData[4] = xy + wz;
	result.mData[5] = 1.0f - (xx + zz);
	result.mData[6] = yz - wx;

	result.mData[8] = xz - wy;
	result.mData[9] = yz + wx;
	result.mData[10] = 1.0f - (xx + yy);
	result.mData[15] = 1.0f;

	result = CMath::MatrixTranspose(result);

#elif !(REALQUATERNION) //  REALQUATERNION
	float sqw = V.w*V.w;
	float sqx = V.x*V.x;
	float sqy = V.y*V.y;
	float sqz = V.z*V.z;

	float invs = 1.0f / (sqx + sqy + sqz + sqw);
	result._11 = (sqx - sqy - sqz + sqw)*invs;
	result._22 = (-sqx + sqy - sqz + sqw)*invs;
	result._33 = (-sqx - sqy + sqz + sqw)*invs;

	float tmp1 = V.x*V.y;
	float tmp2 = V.z*V.w;
	result._21 = 2.0f * (tmp1 + tmp2)*invs;
	result._12 = 2.0f * (tmp1 - tmp2)*invs;

	tmp1 = V.x*V.z;
	tmp2 = V.y*V.w;
	result._31 = 2.0f * (tmp1 - tmp2)*invs;
	result._13 = 2.0f * (tmp1 + tmp2)*invs;
	tmp1 = V.y*V.z;
	tmp2 = V.x*V.w;
	result._32 = 2.0f * (tmp1 + tmp2)*invs;
	result._23 = 2.0f * (tmp1 - tmp2)*invs;

	result._44 = 1.0f;
#endif
	return result;
}

CMath::TVECTOR4 CMath::QuaternionFromAxisAngle(TVECTOR3 axis, float degrees)
{
	float radians = degrees * 3.1416f / 180;

	float x = axis.x * sinf(radians / 2.0f);
	float y = axis.y * sinf(radians / 2.0f);
	float z = axis.z * sinf(radians / 2.0f);
	float w = cosf(radians / 2);

	return TVECTOR4(x, y, z, w);
}

CMath::TVECTOR4 CMath::QuaternionMultiply(TVECTOR4 a, TVECTOR4 b)
{
	TVECTOR4 result;

	result.x = b.w*a.x + b.x*a.w - b.y*a.z + b.z*a.y;
	result.y = b.w*a.y + b.x*a.z +b.y*a.w - b.z*a.x;
	result.z = b.w*a.z - b.x*a.y +b.y*a.x + b.z*a.w;
	result.w = b.w*a.w - b.x*a.x - b.y*a.y - b.z*a.z;

	return result;
}

CMath::TVECTOR3 CMath::Vector3Transform(TVECTOR3 tVector, TMATRIX tMatrix)
{
	TVECTOR3 tResult;

	tResult.x = tVector.x * tMatrix.r[0].mData[0] + tVector.y * tMatrix.r[1].mData[0] + tVector.z * tMatrix.r[2].mData[0] + tMatrix.r[3].mData[0];
	tResult.y = tVector.x * tMatrix.r[0].mData[1] + tVector.y * tMatrix.r[1].mData[1] + tVector.z * tMatrix.r[2].mData[1] + tMatrix.r[3].mData[1];
	tResult.z = tVector.x * tMatrix.r[0].mData[2] + tVector.y * tMatrix.r[1].mData[2] + tVector.z * tMatrix.r[2].mData[2] + tMatrix.r[3].mData[2];

	return tResult;
}

CMath::TVECTOR4 CMath::Vector4Transform(TVECTOR4 tVector, TMATRIX tMatrix)
{
	TVECTOR4 tResult;

	tResult.x = tVector.x * tMatrix.r[0].mData[0] + tVector.y * tMatrix.r[1].mData[0] + tVector.z * tMatrix.r[2].mData[0] + tVector.w * tMatrix.r[3].mData[0];
	tResult.y = tVector.x * tMatrix.r[0].mData[1] + tVector.y * tMatrix.r[1].mData[1] + tVector.z * tMatrix.r[2].mData[1] + tVector.w * tMatrix.r[3].mData[1];
	tResult.z = tVector.x * tMatrix.r[0].mData[2] + tVector.y * tMatrix.r[1].mData[2] + tVector.z * tMatrix.r[2].mData[2] + tVector.w * tMatrix.r[3].mData[2];
	tResult.w = tVector.x * tMatrix.r[0].mData[3] + tVector.y * tMatrix.r[1].mData[3] + tVector.z * tMatrix.r[2].mData[3] + tVector.w * tMatrix.r[3].mData[3];

	return tResult;
}

CMath::TVECTOR2::TVECTOR2()
{
	x = y = 0;
}

CMath::TVECTOR2::TVECTOR2(float fx, float fy)
{
	x = fx;
	x = fy;
}
