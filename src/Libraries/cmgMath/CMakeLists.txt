
set(CMG_MATH_SOURCE_FILES
	
	cmg_math.h
	cmgMathLib.h
	cmgMathLib.cpp

	ecs/cmgTransformComponent.h
	
	noise/cmgNoise.h

	geometry/cmgBBox2f.h
	geometry/cmgBBox2f.cpp
	geometry/cmgBounds.h
	geometry/cmgBounds.cpp
	geometry/cmgCircle2f.h
	geometry/cmgCircle2f.cpp
	geometry/cmgFrustum.h
	geometry/cmgFrustum.cpp
	geometry/cmgLine2f.h
	geometry/cmgLine2f.cpp
	geometry/cmgPlane.h
	geometry/cmgPlane.cpp
	geometry/cmgRay.h
	geometry/cmgRay.cpp
	geometry/cmgRect2f.h
	geometry/cmgRect2f.cpp
	geometry/cmgRect2i.h
	geometry/cmgRect2i.cpp

	types/cmgMatrix3f.h
	types/cmgMatrix3f.cpp
	types/cmgMatrix4f.h
	types/cmgMatrix4f.cpp
	types/cmgQuaternion.h
	types/cmgQuaternion.cpp
	types/cmgTransform3f.h
	types/cmgTransform3f.cpp
	types/cmgVector2.h
	types/cmgVector3.h
	types/cmgVector2f.h
	types/cmgVector2f.cpp
	types/cmgVector3f.h
	types/cmgVector3f.cpp
	types/cmgVector4f.h
	types/cmgVector4f.cpp

)

cmg_install_library(cmgMath "${CMG_MATH_SOURCE_FILES}")

target_link_libraries(cmgMath
	cmgCore
)
