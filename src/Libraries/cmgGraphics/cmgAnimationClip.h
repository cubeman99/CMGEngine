#ifndef _CMG_GRAPHICS_ANIMATION_H_
#define _CMG_GRAPHICS_ANIMATION_H_

#include <cmgMath/cmg_math.h>
#include <cmgGraphics/cmgSkeleton.h>


class AnimationJointTransform : public Transform3f
{
	friend class AnimationClip;
	friend class AnimationFrame;

public:
	AnimationJointTransform()
	{
	}

	inline static AnimationJointTransform Lerp(
		const AnimationJointTransform& a, const AnimationJointTransform& b, float t)
	{
		AnimationJointTransform result;
		result.position = Vector3f::Lerp(a.position, b.position, t);
		result.rotation = Quaternion::Slerp(a.rotation, b.rotation, t);
		result.scale = Vector3f::Lerp(a.scale, b.scale, t);
		return result;
	}

	inline Matrix4f ToMatrix() const
	{
		return GetMatrix();
	}
};


class AnimationFrame
{
	friend class AnimationClip;

public:
	AnimationFrame();
	AnimationFrame(float time, int numJoints);
	
	inline Array<AnimationJointTransform>& GetTransforms() { return m_transforms; }

	void SetTransform(int jointIndex, const Vector3f& translation,
		const Quaternion& rotation, const Vector3f& scale);

	inline float GetTime() const { return m_time; }

private:
	float m_time;
	Array<AnimationJointTransform> m_transforms;
};



class AnimationClip
{
public:
	using sptr = cmg::shared_ptr<AnimationClip>;

	AnimationClip();
	AnimationClip(Skeleton* skeleton);
	~AnimationClip();

	inline Skeleton* GetSkeleton() { return &m_skeleton; }
	inline bool GetLoops() const { return m_loops; }
	inline float GetDuration() const { return m_duration; }
	inline Array<AnimationFrame*>& GetFrames() { return m_frames; }
	const String& GetJointName(uint32 index) const { return m_jointNames[index]; }
	bool HasJoint(const String& name) const;
	int32 GetJointIndex(const String& name) const { return m_jointIndices.at(name); }
	
	inline void SetDuration(float duration) { m_duration = duration; }
	inline void SetLoops(bool loops) { m_loops = loops; }

	AnimationFrame* AddFrame(float time);

	static Error Load(const Path& path, AnimationClip*& outAnimation);
	static Error Save(const Path& path, const AnimationClip* animation);
	static Error Decode(File& file, AnimationClip*& outAnimation);
	static Error Encode(File& file, const AnimationClip* animation);

private:
	Skeleton m_skeleton;
	float m_duration;
	bool m_loops;
	Map<String, uint32> m_jointIndices;
	Array<String> m_jointNames;
	Array<AnimationFrame*> m_frames;
};


#endif // _CMG_GRAPHICS_ANIMATION_H_