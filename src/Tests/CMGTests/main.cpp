
#include <stdio.h>
#include "TestApp.h"
#include "physics/PhysicsApp.h"
#include "driving/DrivingEngine.h"
#include <sstream>

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

int main(int argc, char* argv[])
{
	/*
	String fsSource, vsSource;
	File::OpenAndGetContents(Path(ASSETS_PATH "shaders/test.fs"), fsSource);
	File::OpenAndGetContents(Path(ASSETS_PATH "shaders/test.vs"), vsSource);

	Shader shader;
	shader.AddStage(ShaderType::k_vertex_shader, vsSource);
	shader.AddStage(ShaderType::k_fragment_shader, vsSource);
	shader.CompileAndLink();

	return 0;*/

	//utype x = static_cast<utype>(EnumClass::k_two);
	//int x = enum_val(EnumClass::k_one);
	//int y = enum_val(EnumClass::k_neg_one);
	
	//Mesh* mesh = TestMeshes::LoadMesh(Path("C:/Workspace/C++/Framework Projects/CMGEngine/assets/models/ae86.obj"));
	//delete mesh;
	/*
	char slash;
	int position, texCoord, normal;
	String str;

	String line = "1/2/4 2//3 41// /212/ //124 124/2123/2313";
	std::stringstream lineStream(line);

	String token;
	while (lineStream >> token)
	{
		//String token = "/3345/";
		
		position = -1;
		texCoord = -1;
		normal = -1;

		unsigned int index;
		unsigned int start;
		index = token.find_first_of('/', 0);
		if (index > 0)
			position = atoi(token.substr(0, index).c_str());
		start = index + 1;
		index = token.find_first_of('/', start);
		if (index > start)
			texCoord = atoi(token.substr(start, index - start).c_str());
		start = index + 1;
		if (start < token.length())
			normal = atoi(token.substr(start, token.length() - start).c_str());
		cout << position << " / " << texCoord << " / " << normal << endl;
	}*/

	//if (start > index + 1)
		//cout << line.substr(start, start - index - 1) << endl;

	//start = index;
	//index = line.find_first_of('/', index);
	//cout << line.substr(start, index - start) << endl;
	//if (index != std::string::npos)




	//if (lineStream >> position)
	//	cout << "pos = " << position << endl;
	//if (lineStream >> slash >> index2)
	//	printf("i2\n");
	//lineStream >> str;
	//if (lineStream >> slash >> index3)
		//printf("i3\n");


	//system("pause");
	//return 0;



	//TestApp app;
	//PhysicsApp app;
	//app.Initialize("3D Physics Engine", 800, 600);
		
	DrivingApp app;
	app.Initialize("Vehicle Dynamics", 1200, 785);


	app.Run();

	return 0;
}
