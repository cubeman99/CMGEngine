
#include <stdio.h>
#include "TestApp.h"
#include "physics/PhysicsApp.h"
#include "physics/CollisionTester.h"
#include "driving/DrivingEngine.h"
#include <sstream>

#include <gtest/gtest.h>


using namespace std;

/*
#define CATCH_CONFIG_MAIN 
#include <Catch/catch.hpp>


#define CHECK_MATRIX4F(mat,x1,y1,z1,w1,x2,y2,z2,w2,x3,y3,z3,w3,x4,y4,z4,w4) \
  CHECK((mat[0]) == Approx(x1)); CHECK((mat[1]) == Approx(y1));\
  CHECK((mat[2]) == Approx(z1)); CHECK((mat[3]) == Approx(w1));\
\
  CHECK((mat[4]) == Approx(x2)); CHECK((mat[5]) == Approx(y2));\
  CHECK((mat[6]) == Approx(z2)); CHECK((mat[7]) == Approx(w2));\
\
  CHECK((mat[8]) == Approx(x3)); CHECK((mat[9]) == Approx(y3));\
  CHECK((mat[10]) == Approx(z3)); CHECK((mat[11]) == Approx(w3));\
\
  CHECK((mat[12]) == Approx(x4)); CHECK((mat[13]) == Approx(y4));\
  CHECK((mat[14]) == Approx(z4)); CHECK((mat[15]) == Approx(w4))


#define CHECK_VEC4F(vec,_x,_y,_z,_w) \
	CHECK((vec).x == Approx(_x) ); \
	CHECK((vec).y == Approx(_y) ); \
	CHECK((vec).z == Approx(_z) ); \
	CHECK((vec).w == Approx(_w) )



TEST_CASE("Quaternion")
{
	Quaternion q = Quaternion::IDENTITY;
	CHECK(q.x == Approx(0.0f));
}

TEST_CASE("Math/Matrix4/Constructors")
{
	Matrix4f a, b, c;

	a = Matrix4f::IDENTITY;
	CHECK_MATRIX4F(a, 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
	a.SetIdentity();
	CHECK_MATRIX4F(a, 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
	a.SetZero();
	CHECK_MATRIX4F(a, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);

	CHECK(a.data() == a.m);

	a = Matrix4f::IDENTITY;
	b = Matrix4f::IDENTITY;
	c = a * b;
	CHECK_MATRIX4F(c, 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

	Vector4f v(0,0,0,0);
	v = Matrix4f::IDENTITY * v;
	CHECK_VEC4F(v, 0, 0, 0, 0);
}

TEST_CASE("Vector3f")
{
	Vector3f v;
	
	v = Vector3f::Cross(Vector3f::UNITX, Vector3f::UNITY);
	CHECK(v.x == 0);
	CHECK(v.y == 0);
	CHECK(v.z == 1);

	v = Vector3f::Cross(Vector3f::UNITY, Vector3f::UNITZ);
	CHECK(v.x == 1);
	CHECK(v.y == 0);
	CHECK(v.z == 0);

	v = Vector3f::Cross(Vector3f::UNITZ, Vector3f::UNITX);
	CHECK(v.x == 0);
	CHECK(v.y == 1);
	CHECK(v.z == 0);
}
*/
/*
#include <type_traits> //for std::underlying_type

enum class EnumClass
{
	k_neg_one = -1,
	k_one = 1,
	k_two = 2,
	k_three = 3,
};
#define DEFINE_IMPLICIT_CAST_TO_UNDERLYING_TYPE(_enum) \
	std::underlying_type<_enum>::type enum_val(_enum e)\
	{\
		return static_cast<std::underlying_type<_enum>::type>(e);\
	}\

template <class T_EnumClass>
std::underlying_type<T_EnumClass>::type enum_val(T_EnumClass e)
{
	return static_cast<std::underlying_type<T_EnumClass>::type>(e);
}


typedef std::underlying_type<EnumClass>::type utype;
*/


static void EPXECT_MATRIX_EQ(const Matrix4f& m1, const Matrix4f& m2)
{
	EXPECT_FLOAT_EQ(m1.m[0], m2.m[0]);
	EXPECT_FLOAT_EQ(m1.m[1], m2.m[1]);
	EXPECT_FLOAT_EQ(m1.m[2], m2.m[2]);
	EXPECT_FLOAT_EQ(m1.m[3], m2.m[3]);
	EXPECT_FLOAT_EQ(m1.m[4], m2.m[4]);
	EXPECT_FLOAT_EQ(m1.m[5], m2.m[5]);
	EXPECT_FLOAT_EQ(m1.m[6], m2.m[6]);
	EXPECT_FLOAT_EQ(m1.m[7], m2.m[7]);
	EXPECT_FLOAT_EQ(m1.m[8], m2.m[8]);
	EXPECT_FLOAT_EQ(m1.m[9], m2.m[9]);
	EXPECT_FLOAT_EQ(m1.m[10], m2.m[10]);
	EXPECT_FLOAT_EQ(m1.m[11], m2.m[11]);
	EXPECT_FLOAT_EQ(m1.m[12], m2.m[12]);
	EXPECT_FLOAT_EQ(m1.m[13], m2.m[13]);
	EXPECT_FLOAT_EQ(m1.m[14], m2.m[14]);
	EXPECT_FLOAT_EQ(m1.m[15], m2.m[15]);
}

static void EPXECT_MATRIX_EQ(const Matrix4f& m,
	float x1, float y1, float z1, float w1,
	float x2, float y2, float z2, float w2,
	float x3, float y3, float z3, float w3,
	float x4, float y4, float z4, float w4)
{
	EXPECT_FLOAT_EQ(m.m[0], x1);
	EXPECT_FLOAT_EQ(m.m[1], x2);
	EXPECT_FLOAT_EQ(m.m[2], x3);
	EXPECT_FLOAT_EQ(m.m[3], x4);
	EXPECT_FLOAT_EQ(m.m[4], y1);
	EXPECT_FLOAT_EQ(m.m[5], y2);
	EXPECT_FLOAT_EQ(m.m[6], y3);
	EXPECT_FLOAT_EQ(m.m[7], y4);
	EXPECT_FLOAT_EQ(m.m[8], z1);
	EXPECT_FLOAT_EQ(m.m[9], z2);
	EXPECT_FLOAT_EQ(m.m[10], z3);
	EXPECT_FLOAT_EQ(m.m[11], z4);
	EXPECT_FLOAT_EQ(m.m[12], w1);
	EXPECT_FLOAT_EQ(m.m[13], w2);
	EXPECT_FLOAT_EQ(m.m[14], w3);
	EXPECT_FLOAT_EQ(m.m[15], w4);
}


TEST(Matrix4f, Matrix4f_Transpose)
{
	Matrix4f matrix;
	matrix.Set(1, 2, 3, 4,
			   5, 6, 7, 8,
			   9, 10, 11, 12,
			   13, 14, 15, 16);

	Matrix4f transpose = matrix.GetTranspose();
	EPXECT_MATRIX_EQ(transpose,
		1, 5, 9, 13,
		2, 6, 10, 14,
		3, 7, 11, 15,
		4, 8, 12, 16);
}


TEST(Matrix4f, Matrix4f_AffineInverse)
{
	Matrix4f affineMatrix =
		Matrix4f::CreateTranslation(2.5f, -1.1f, 3.8f) *
		Matrix4f::CreateScale(2.5f);

	Matrix4f affineMatrixInverse = affineMatrix.GetAffineInverse();

	EPXECT_MATRIX_EQ(Matrix4f::IDENTITY, affineMatrixInverse * affineMatrix);
}


TEST(Matrix4f, Matrix4f_Inverse)
{
	Matrix4f matrix =
		Matrix4f::CreateTranslation(2.5f, -1.1f, 3.8f) *
		Matrix4f::CreateScale(2.5f);

	Matrix4f matrixInverse = matrix.GetInverse();

	EPXECT_MATRIX_EQ(Matrix4f::IDENTITY, matrixInverse * matrix);
}

int main(int argc, char* argv[])
{
	srand((unsigned int) time(nullptr));

	//TestApp app;
	PhysicsApp app;
	//CollisionTestApp app;
	app.Initialize("3D Physics Engine", 800, 600);
	//DrivingApp app;
	//app.Initialize("Vehicle Dynamics", 1200, 785);
	app.Run();
	return 0;

	//::testing::InitGoogleTest(&argc, argv);
	//return RUN_ALL_TESTS();
}
