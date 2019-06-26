
set(CMG_MATH_SOURCE_FILES
	
	cmg_math.h
	cmgMathLib.h
	cmgMathLib.cpp

	ecs/cmgTransformComponent.h
	
	noise/cmgNoise.h

	types/cmgBBox2f.h
	types/cmgBBox2f.cpp
	types/cmgBounds.h
	types/cmgBounds.cpp
	types/cmgCircle2f.h
	types/cmgCircle2f.cpp
	types/cmgFrustum.h
	types/cmgFrustum.cpp
	types/cmgLine2f.h
	types/cmgLine2f.cpp
	types/cmgMatrix3f.h
	types/cmgMatrix3f.cpp
	types/cmgMatrix4f.h
	types/cmgMatrix4f.cpp
	types/cmgPlane.h
	types/cmgPlane.cpp
	types/cmgPoint2i.h
	types/cmgPoint2i.cpp
	types/cmgPoint3i.h
	types/cmgPoint3i.cpp
	types/cmgQuaternion.h
	types/cmgQuaternion.cpp
	types/cmgRay.h
	types/cmgRay.cpp
	types/cmgRect2f.h
	types/cmgRect2f.cpp
	types/cmgRect2i.h
	types/cmgRect2i.cpp
	types/cmgTransform3f.h
	types/cmgTransform3f.cpp
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
