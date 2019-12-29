#include "cmgAnimationClip.h"
#include <cmgGraphics/importers/cmgSourceModelImporter.h>


AnimationFrame::AnimationFrame() :
	m_time(0.0f)
{
}

AnimationFrame::AnimationFrame(float time, int numJoints) :
	m_time(time)
{
	m_transforms.resize(numJoints);
}

void AnimationFrame::SetTransform(int jointIndex, const Vector3f& translation,
	const Quaternion& rotation, const Vector3f& scale)
{
	m_transforms[jointIndex].position = translation;
	m_transforms[jointIndex].rotation = rotation;
	m_transforms[jointIndex].scale = scale;
}


AnimationClip::AnimationClip() :
	m_loops(true),
	m_duration(0.0f)
{
}

AnimationClip::AnimationClip(Skeleton* skeleton) :
	m_loops(true),
	m_duration(0.0f),
	m_skeleton(*skeleton)
{
	m_jointNames.resize(skeleton->GetJoints().size());
	for (uint32 i = 0; i < skeleton->GetJoints().size(); i++)
	{
		auto joint = skeleton->GetJoint(i);
		m_jointIndices[joint.GetName()] = i;
		m_jointNames[i] = joint.GetName();
	}
}

AnimationClip::~AnimationClip()
{
	for (unsigned int i = 0; i < m_frames.size(); ++i)
		delete m_frames[i];
	m_frames.clear();
}


bool AnimationClip::HasJoint(const String & name) const
{
	return (m_jointIndices.find(name) != m_jointIndices.end());
}

AnimationFrame* AnimationClip::AddFrame(float time)
{
	AnimationFrame* newFrame;
	if (m_frames.size() > 0)
	{
		newFrame = new AnimationFrame(*m_frames.back());
		newFrame->m_time = time;
	}
	else
	{
		newFrame = new AnimationFrame(time, m_skeleton.GetNumJoints());
	}

	// TODO: insert sorted
	m_frames.push_back(newFrame);
	return newFrame;
}

Error AnimationClip::Load(const Path& path, AnimationClip*& outAnimation)
{
	String extension = string::ToLower(path.GetExtension());

	if (extension == "smd")
	{
		SourceModelImporter importer;
		return importer.ImportAnimation(path, outAnimation);
	}
	else
	{
		// Load a CMG model file
		File file(path);
		Error error = file.Open(FileAccess::READ, FileType::BINARY);
		if (error.Failed())
			return error.Uncheck();
		return Decode(file, outAnimation);
	}
}

Error AnimationClip::Save(const Path& path, const AnimationClip* animation)
{
	File file(path);
	Error error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return Encode(file, animation);
}

Error AnimationClip::Decode(File& file, AnimationClip*& outAnimation)
{
	return Error();
}

Error AnimationClip::Encode(File& file, const AnimationClip* animation)
{
	return Error();
}

