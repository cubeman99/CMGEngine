#include "cmgSkeleton.h"


struct CMGSkeletonFile
{
	static constexpr char MAGIC[] = "CMGSkele";

	// Header (64 bytes)
	struct Header
	{
		char magic[8] = {'C', 'M', 'G', 'S', 'k', 'e', 'l', 'e'};
		uint32 version = 1;
		uint32 jointCount = 0;
		uint32 rootJointIndex = 0;
		uint32 reserved[1] = { 0 };
	} header;

	struct JointData
	{
		char name;
		int32 parent;
		Vector3f position;
		Quaternion rotation;
		Vector3f scale;
	};

	// Data
	uint8* vertexData;
	uint8* indexData;
};


SkeletonJoint::SkeletonJoint(const Skeleton& skeleton, const String& name,
	int32 index, int32 parentIndex) :
	m_skeleton(skeleton),
	m_name(name),
	m_index(index),
	m_parentIndex(parentIndex)
{
}

inline SkeletonJoint & SkeletonJoint::GetParent()
{
	return (SkeletonJoint&) m_skeleton.GetJoint(m_parentIndex);
}

inline const SkeletonJoint& SkeletonJoint::GetParent() const
{
	return m_skeleton.GetJoint(m_parentIndex);
}


Skeleton::Skeleton()
{
}

SkeletonJoint& Skeleton::AddJoint(const String& name, int32 parentIndex)
{
	m_joints.push_back(SkeletonJoint(
		*this, name, (int32) m_joints.size(), parentIndex));
	return m_joints.back();
}

void Skeleton::ClearJoints()
{
	m_joints.clear();
}

SkeletonJoint* Skeleton::FindJoint(const String& name)
{
	for (unsigned int i = 0; i < m_joints.size(); ++i)
	{
		if (m_joints[i].m_name == name)
			return &m_joints[i];
	}
	return nullptr;
}

Error Skeleton::Load(const Path& path, Skeleton& outSkeleton)
{
	File file(path);
	Error error = file.Open(FileAccess::READ, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return Decode(file, outSkeleton);
}

Error Skeleton::Save(const Path& path, const Skeleton* skeleton)
{
	File file(path);
	Error error = file.Open(FileAccess::WRITE, FileType::BINARY);
	if (error.Failed())
		return error.Uncheck();
	return Encode(file, skeleton);
}

Error Skeleton::Decode(File & file, Skeleton& outSkeleton)
{
	CMGSkeletonFile contents;

	outSkeleton.ClearJoints();

	// Read the header
	file.Read(&contents.header, sizeof(contents.header));

	// Read the joint data
	for (uint32 i = 0; i < contents.header.jointCount; i++)
	{
		// Read the joint data
		uint32 nameSize = 0;
		int32 parentIndex = -1;
		Vector3f position;
		Quaternion rotation;
		Vector3f scale;
		file.Read(&nameSize, sizeof(uint32));
		char* nameData = new char[nameSize];
		file.Read(nameData, nameSize);
		file.Read(&parentIndex, sizeof(int32));
		CMG_ASSERT(parentIndex < (int32) i);
		file.Read(&position, sizeof(Vector3f));
		file.Read(&rotation, sizeof(Quaternion));
		file.Read(&scale, sizeof(Vector3f));

		// Create the joint
		SkeletonJoint* parent = nullptr;
		SkeletonJoint& joint = outSkeleton.AddJoint(
			String(nameData), parentIndex);
		joint.SetLocalPosition(position);
		joint.SetLocalRotation(rotation);
		joint.SetLocalScale(scale);

		delete [] nameData;
		nameData = nullptr;
	}

	outSkeleton.CalcTransforms();
	return CMG_ERROR_SUCCESS;
}

Error Skeleton::Encode(File& file, const Skeleton* skeleton)
{
	CMGSkeletonFile contents;

	// Header
	contents.header.version = 1;
	contents.header.jointCount = (uint32) skeleton->GetJoints().size();
	contents.header.rootJointIndex = 0;
	file.Write(&contents.header, sizeof(contents.header));

	// Joints
	for (uint32 i = 0; i < skeleton->GetJoints().size(); i++)
	{
		const SkeletonJoint& joint = skeleton->GetJoint(i);
		int32 parentIndex = joint.GetParentIndex();
		Vector3f position = joint.GetLocalPosition();
		Quaternion rotation = joint.GetLocalRotation();
		Vector3f scale = joint.GetLocalScale();
		uint32 nameLength = joint.GetName().length() + 1;
		file.Write(&nameLength, sizeof(uint32));
		file.Write(joint.GetName().c_str(), nameLength);
		file.Write(&parentIndex, sizeof(int32));
		file.Write(&position, sizeof(Vector3f));
		file.Write(&rotation, sizeof(Quaternion));
		file.Write(&scale, sizeof(Vector3f));
	}

	return CMG_ERROR_SUCCESS;
}

void Skeleton::CalcTransforms()
{
	// Calculate local and global joint transforms
	for (auto& joint : m_joints)
	{
		joint.m_localMatrix = joint.m_transform.GetMatrix();
		joint.m_localInvMatrix = joint.m_transform.GetInvMatrix();
		if (joint.HasParent())
		{
			joint.m_globalMatrix = 
				joint.GetParent().m_globalMatrix * joint.m_localMatrix;
			joint.m_globalInvMatrix = joint.m_localInvMatrix *
				joint.GetParent().m_globalInvMatrix;
		}
		else
		{
			joint.m_globalMatrix = joint.m_localMatrix;
			joint.m_globalInvMatrix = joint.m_localInvMatrix;
		}
	}
}



