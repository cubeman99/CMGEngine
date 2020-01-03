#ifndef _CMG_GRAPHICS_SKELETON_H_
#define _CMG_GRAPHICS_SKELETON_H_

#include <cmgCore/cmg_core.h>
#include <cmgMath/cmg_math.h>


class SkeletonJoint
{
	friend class Skeleton;

public:
	SkeletonJoint(const Skeleton* skeleton, const String& name, int32 index, int32 parentIndex = -1);

	inline const String& GetName() const { return m_name; }
	inline int32 GetIndex() const { return m_index; }
	inline bool HasParent() const { return m_parentIndex >= 0; }
	inline int32 GetParentIndex() const { return m_parentIndex; }
	inline SkeletonJoint& GetParent();
	inline const SkeletonJoint& GetParent() const;
	inline const Skeleton& GetSkeleton() const { return *m_skeleton; }
	inline const Transform3f& GetLocalTransform() const { return m_transform; }
	inline const Vector3f& GetLocalPosition() const { return m_transform.position; }
	inline const Quaternion& GetLocalRotation() const { return m_transform.rotation; }
	inline const Vector3f& GetLocalScale() const { return m_transform.scale; }
	inline const Matrix4f& GetGlobalTransform() const { return m_globalMatrix; }
	inline const Matrix4f& GetInverseBindTransform() const { return m_globalInvMatrix; }

	inline void SetLocalTransform(const Transform3f& transform) { m_transform = transform; }
	inline void SetLocalPosition(const Vector3f& position) { m_transform.position = position; }
	inline void SetLocalRotation(const Quaternion& rotation) { m_transform.rotation = rotation; }
	inline void SetLocalScale(const Vector3f& scale) { m_transform.scale = scale; }

	inline void SetLocalTransform(const Matrix4f& localTransform) { m_localMatrix = localTransform; }
	inline void SetGlobalTransform(const Matrix4f& globalTransform) { m_globalMatrix = globalTransform; }
	inline void SetInverseBindTransform(const Matrix4f& transform) { m_globalInvMatrix = transform; }

private:
	const Skeleton* m_skeleton;
	String m_name;
	int32 m_index = 0;
	int32 m_parentIndex = -1;
	Transform3f m_transform;

	// Cached values
	Matrix4f m_localMatrix;
	Matrix4f m_localInvMatrix;
	Matrix4f m_globalMatrix;
	Matrix4f m_globalInvMatrix;
};


class Skeleton : public cmg::ResourceImpl<Skeleton>
{
public:
	Skeleton();

	uint32 GetNumJoints() const { return m_joints.size(); }
	SkeletonJoint& GetJoint(int32 index) { return m_joints[index]; }
	const SkeletonJoint& GetJoint(int32 index) const { return m_joints[index]; }
	Array<SkeletonJoint>& GetJoints() { return m_joints; }
	const Array<SkeletonJoint>& GetJoints() const { return m_joints; }
	SkeletonJoint* FindJoint(const String& name);
	const SkeletonJoint* FindJoint(const String& name) const;

	SkeletonJoint& AddJoint(const String& name, int32 parentIndex = -1);
	void ClearJoints();

	static Error Load(const Path& path, Skeleton& outSkeleton);
	static Error Save(const Path& path, const Skeleton* skeleton);
	static Error Decode(File& file, Skeleton& outSkeleton);
	static Error Encode(File& file, const Skeleton* skeleton);

	void CalcTransforms();

protected:
	virtual Error UnloadImpl();
	virtual Error LoadImpl();

private:

	Array<SkeletonJoint> m_joints;
};


#endif // _CMG_GRAPHICS_SKELETON_H_
