#include "cmgSourceModelImporter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


SourceModelImporter::SourceModelImporter()
{
}

SourceModelImporter::~SourceModelImporter()
{
}

Error SourceModelImporter::ImportModel(const Path& path, Model *& outModel)
{
	// Open the file for reading
	std::ifstream file;
	file.open(path.ToString());
	if (!file.is_open())
		return CMG_ERROR_FAILURE;

	String line;
	SmdModel smdModel;
	ReadLine(file, line); // version 1

	Skeleton& skeleton = outModel->GetSkeleton();
	ReadSkeleton(file, smdModel.skeleton, &skeleton);
	ReadAnimation(file, smdModel.skeleton, smdModel.animation,
		&skeleton, nullptr);
	outModel = new Model();
	ReadMeshes(file, smdModel, outModel);
	file.close();

	return CMG_ERROR_SUCCESS;
}

Error SourceModelImporter::ImportAnimation(const Path& path, AnimationClip*& outAnimation)
{
	// Open the file for reading
	std::ifstream file;
	file.open(path.ToString());
	if (!file.is_open())
		return CMG_ERROR_FAILURE;

	String line;
	SmdModel smdModel;
	ReadLine(file, line); // version 1

	Skeleton* skeleton = new Skeleton();
	ReadSkeleton(file, smdModel.skeleton, skeleton);
	outAnimation = new AnimationClip(skeleton);
	ReadAnimation(file, smdModel.skeleton, smdModel.animation,
		nullptr, outAnimation);
	file.close();

	return CMG_ERROR_SUCCESS;
}

Error SourceModelImporter::ReadSkeleton(std::ifstream& file, SmdSkeleton& smdSkeleton, Skeleton* skeleton)
{
	std::istringstream lineStream;
	String line;

	// BLOCK 1: Skeleton joint hierarchy
	if (ReadLine(file, line) != "nodes")
	{
		return CMG_ERROR_FAILURE;
	}
	while (ReadLine(file, line) != "end")
	{
		lineStream.clear();
		lineStream.str(line);
		ReadJoint(lineStream, smdSkeleton);
	}

	// Convert SMD Skeleton to CMG Skeleton
	for (uint32 i = 0; i < smdSkeleton.nodes.size(); ++i)
	{
		SmdNode& smdNode = smdSkeleton.nodes[i];
		SkeletonJoint* parent = nullptr;
		if (smdNode.parentIndex >= 0)
			skeleton->AddJoint(smdNode.name, smdNode.parentIndex);
		else
			skeleton->AddJoint(smdNode.name);
	}

	return CMG_ERROR_SUCCESS;
}

Error SourceModelImporter::ReadAnimation(std::ifstream& file,
	SmdSkeleton& smdSkeleton, SmdAnimation& smdAnimation,
	Skeleton* skeleton, AnimationClip* animation)
{
	std::istringstream lineStream;
	String line;

	// BLOCK 2: Skeleton bind-pose joint transforms.
	// time 0 is the default "bind" pose of the skeleton
	// animation files will include more time poses
	if (ReadLine(file, line) != "skeleton")
	{
		return CMG_ERROR_FAILURE;
	}
	SmdKeyFrame keyFrame;
	keyFrame.poses.resize(smdSkeleton.nodes.size());
	while (ReadLine(file, line) != "end")
	{
		lineStream.clear();
		lineStream.str(line);

		if (line.find("time") != String::npos)
		{
			String temp;
			int time;
			lineStream >> temp;
			lineStream >> time;
			if (time > 0)
				smdAnimation.frames.push_back(keyFrame);
		}
		else
		{
			ReadNodeTransform(lineStream, keyFrame);
		}
	}
	smdAnimation.frames.push_back(keyFrame);

	// Skeleton bind pose as the first animation frame
	if (skeleton != nullptr)
	{
		const SmdKeyFrame& firstSmdFrame = smdAnimation.frames[0];
		for (uint32 j = 0; j < firstSmdFrame.poses.size(); ++j)
		{
			const SmdNodeTransform& transform = firstSmdFrame.poses[j];
			SkeletonJoint& joint = skeleton->GetJoint(j);
			joint.SetLocalPosition(transform.translation);
			joint.SetLocalRotation(transform.rotation);
		}
		skeleton->CalcTransforms();
	}

	if (animation == nullptr)
		return CMG_ERROR_SUCCESS;

	// Convert SMD Animation to CMG Animation
	// Also define the 
	float frequency = 0.05f;
	animation->SetDuration(frequency * smdAnimation.frames.size());
	for (uint32 frameIndex = 0; frameIndex <
		smdAnimation.frames.size(); frameIndex++)
	{
		const SmdKeyFrame& smdFrame = smdAnimation.frames[frameIndex];
		AnimationFrame* frame = animation->AddFrame(frameIndex * frequency);
		for (uint32 j = 0; j < smdFrame.poses.size(); ++j)
		{
			const SmdNodeTransform& transform = smdFrame.poses[j];
			frame->SetTransform(j, transform.translation,
				transform.rotation, Vector3f::ONE);
		}
	}

	return CMG_ERROR_SUCCESS;
}

Error SourceModelImporter::ReadMeshes(std::ifstream & file, SmdModel& smdModel, Model* outModel)
{
	String line;
	std::istringstream lineStream;

	// BLOCK 3: Faces/Vertices
	if (ReadLine(file, line) == "triangles")
	{
		String materialName;
		int materialIndex = -1;
		Array<String> materialNames;
		SmdMesh* mesh = nullptr;
		while (ReadLine(file, materialName) != "end")
		{
			// Remove material name extensions
			//int extPos = materialName.find_first_of('.');
			//if (extPos != String::npos)
			//materialName = materialName.substr(0, extPos);

			// Create a new mesh
			if (smdModel.meshes.find(materialName) == smdModel.meshes.end())
			{
				smdModel.meshes[materialName] = SmdMesh();
				mesh = &smdModel.meshes[materialName];
			}

			// Read the three vertices for each face
			for (int i = 0; i < 3; ++i)
			{
				ReadLine(file, line);	
				lineStream.clear();
				lineStream.str(line);
				ReadVertex(lineStream, *mesh);
			}
		}
	}

	// Convert the SMD meshes to CMG meshes
	uint32 meshIndex = 0;
	outModel->SetMeshCount(smdModel.meshes.size());
	for (std::pair<String, const SmdMesh&> pair : smdModel.meshes)
	{
		const SmdMesh& smdMesh = pair.second;
		Array<ModelVertex> vertices;
		Array<uint32> indices;
		vertices.resize(smdMesh.positions.size());
		indices.resize(smdMesh.positions.size());
		ModelVertex vertex;
		vertex.color = Color::WHITE.ToVector4f();
		for (uint32 i = 0; i < smdMesh.positions.size(); i++)
		{
			vertex.position = smdMesh.positions[i];
			vertex.texCoord = smdMesh.texCoords[i];
			vertex.normal = smdMesh.normals[i];
			for (size_t j = 0; j < 4; j++)
			{
				vertex.boneWeights[j] = smdMesh.jointWeights[i].weights[j];
				vertex.boneIndices[j] = smdMesh.jointIndices[i].indices[j];
			}
			vertices[i] = vertex;
			indices[i] = i;
		}

		Mesh* mesh = new Mesh();
		mesh->GetVertexData()->BufferVertices(vertices);
		mesh->GetIndexData()->BufferIndices(indices);
		outModel->SetMesh(meshIndex, mesh);
		meshIndex++;
	}

	return CMG_ERROR_SUCCESS;
}

void SourceModelImporter::ReadJoint(std::istringstream& line, SmdSkeleton& skeleton)
{
	int jointIndex;
	SmdNode joint;
	line >> jointIndex; // TODO: validate this
	line >> joint.name;
	line >> joint.parentIndex; // TODO: validate this
	joint.name = joint.name.substr(1, joint.name.length() - 2); // Remove "" string delimeters
	skeleton.nodes.push_back(joint);
}

void SourceModelImporter::ReadNodeTransform(std::istringstream& line, SmdKeyFrame& keyFrame)
{
	SmdNodeTransform pose;
	int nodeIndex;

	line >> nodeIndex;
	line >> pose.translation.x;
	line >> pose.translation.y;
	line >> pose.translation.z;
	line >> pose.rotation.x;
	line >> pose.rotation.y;
	line >> pose.rotation.z;
	
	// Convert from Euler angles to quaternion
	pose.rotation = Quaternion::FromEuler(
		pose.rotation.x, pose.rotation.y, pose.rotation.z);
	
	keyFrame.poses[nodeIndex] = pose;
}

void SourceModelImporter::ReadVertex(std::istringstream& line, SmdMesh& mesh)
{
	int parentJointIndex;

	SmdVertex vertex;
	line >> parentJointIndex;
	line >> vertex.position.x;
	line >> vertex.position.y;
	line >> vertex.position.z;
	line >> vertex.normal.x;
	line >> vertex.normal.y;
	line >> vertex.normal.z;
	line >> vertex.texCoord.x;
	line >> vertex.texCoord.y;
	vertex.texCoord.y = 1.0f - vertex.texCoord.y; // Invert texture V coordinate

	// Read optional joint weights (instead of using parent joint index)
	if (!line.eof())
	{
		int numLinks;
		line >> numLinks;

		if (numLinks <= SMD_VERTEX_MAX_JOINTS)
		{
			for (int j = 0; j < numLinks && j < SMD_VERTEX_MAX_JOINTS; ++j)
			{
				line >> vertex.jointIndices.indices[j];
				line >> vertex.jointWeights.weights[j];
			}
		}
		else
		{
			// Only use the joints with the largest weights
			Array<std::pair<int, float>> jointWeights;
			int jIndex;
			float jWeight;
			for (int i = 0; i < numLinks; ++i)
			{
				line >> jIndex;
				line >> jWeight;
				uint32 insertLoc = jointWeights.size();
				for (uint32 j = 0; j < insertLoc; ++j)
				{
					if (jWeight >= jointWeights[j].first)
						insertLoc = j;
				}
				jointWeights.insert(jointWeights.begin() + insertLoc,
					std::pair<int, float>(jIndex, jWeight));
			}

			// Normalize the largest weights so they add up to one.
			float weightSum = 0.0f;
			for (int i = 0; i < SMD_VERTEX_MAX_JOINTS; ++i)
				weightSum += jointWeights[i].second;
			for (int i = 0; i < SMD_VERTEX_MAX_JOINTS; ++i)
			{
				vertex.jointIndices.indices[i] = jointWeights[i].first;
				vertex.jointWeights.weights[i] = jointWeights[i].second / weightSum;
			}
		}
	}
	else
	{
		// Use parent joint index for skinning
		vertex.jointIndices.indices[0] = parentJointIndex;
		vertex.jointWeights.weights[0] = 1.0f;
	}

	// Add the vertex to the list of mesh vertices
	//if (vertices.size() % 3 == 2)
	//{
	//	// Flip the winding order from CCW to CW for proper backface culling.
	//	vertices.push_back(vertices.back());
	//	vertices.at(vertices.size() - 2) = vertex;
	//}
	//else
	{
		mesh.positions.push_back(vertex.position);
		mesh.normals.push_back(vertex.normal);
		mesh.texCoords.push_back(vertex.texCoord);
		mesh.jointIndices.push_back(vertex.jointIndices);
		mesh.jointWeights.push_back(vertex.jointWeights);
	}
}

String SourceModelImporter::ReadLine(std::ifstream& file, String& line)
{
	if (!file.eof())
	{
		do
		{
			std::getline(file, line);
		}
		while (!file.eof() && line.find("//") == 0);
	}
	return line;
}


/*
	
////////////////////////////////////////////////////////////////////////////////////////
//							***** QC MODEL LOADER *****
////////////////////////////////////////////////////////////////////////////////////////
Model SourceModelImporter::loadModelQC(const FileInfo& fileInfo)
{
	// Open the qc script file for reading.
	ifstream file;
	file.open(fileInfo.path);
	if (!file.is_open())
	{
		cerr << "Error loading QC script \"" << fileInfo.path << "\"" << endl;
		return NULL;
	}

	ModelResource* model = new ModelResource();
	
	vector<string> args;

	vector<DirectoryInfo> materialDirs;

	string line;
	while (!file.eof())
	{
		args.clear();
		getline(file, line);

		// Remove end-line comments.
		int commentPos = line.find_first_of(";#");
		if (commentPos != string::npos)
			line = line.substr(0, commentPos);
		commentPos = line.find("//");
		if (commentPos != string::npos)
			line = line.substr(0, commentPos);

		// Create list of command arguments.
		Util::strSplit(line, ' ', &args);
		if (args.empty())
			continue;

		if (args[0] == "$modelname")
		{
			string modelName = args[1];
		}
		else if (args[0] == "$cdmaterials")
		{
			for (uint i = 1; i < args.size(); ++i)
				materialDirs.push_back(MATERIALS_DIRECTORY.path + "/" + args[i]);
		}
		else if (args[0] == "$body")
		{
			FileInfo meshFileInfo = fileInfo.getDirectory().path + "/" + args[2];
			model->SetReferenceMesh(loadMeshSMD(meshFileInfo, materialDirs));
		}

		int i = line.find("$sequence");
		if (i != string::npos)
			cout << line << endl;
	}

	m_resources->addToRegisty(model);
	return model;
}

////////////////////////////////////////////////////////////////////////////////////////
//							***** SMD ANIMATION LOADER *****
////////////////////////////////////////////////////////////////////////////////////////

AnimationClip SourceModelImporter::loadAnimationSMD(const FileInfo& fileInfo, const Skeleton& skeletonRef)
{
	// Open the file for reading.
	ifstream file;
	file.open(fileInfo.path);
	if (!file.is_open())
		return NULL;

	String line;
	istringstream lineStream;

	ReadLine(file, line); // version 1

	// BLOCK 1: Skeleton joint hierarchy.
	if (ReadLine(file, line) != "nodes")
	{
		file.close();
		return NULL;
	}
	SkeletonInstance skeleton = new SkeletonResource();
	while (ReadLine(file, line) != "end")
	{
		lineStream.clear();
		lineStream.str(line);
		ReadJoint(lineStream, skeleton);
	}

	// BLOCK 2: Skeleton bind-pose joint transforms.
	if (ReadLine(file, line) != "skeleton")
	{
		delete skeleton.getResource();
		file.close();
		return NULL;
	}
	AnimationFrame frame(skeleton);
	AnimationClip clip = new AnimationClipResource();
	clip->setSkeleton(skeleton);
	while (ReadLine(file, line) != "end")
	{
		lineStream.clear();
		lineStream.str(line);

		if (line.find("time") != string::npos)
		{
			string garbage;
			int t;
			lineStream >> garbage;
			lineStream >> t;
			frame.setTime((float) t);
			if (t > 0)
			{
				frame.calcGlobalTransforms();
				clip->addFrame(frame);
			}
		}
		else
		{
			ReadJointKey(lineStream, frame);
		}
	}
	frame.calcGlobalTransforms();
	clip->addFrame(frame);

	// POST-PROCESS:
	if (skeletonRef != NULL)
	{
		Skeleton newSkeleton = skeletonRef;

		// Translate joints to reference skeleton.
		Array<int> lookup;
		for (int i = 0; i < newSkeleton->getNumJoints(); ++i)
		{
			bool found = false;
			for (int j = 0; j < skeleton->getNumJoints(); ++j)
			{
				if (newSkeleton->getJoint(i).name == skeleton->getJoint(j).name)
				{
					lookup.push_back(j);
					found = true;
					break;
				}
			}
			if (!found)
				lookup.push_back(-1); // Animation doesn't include this joint.
		}

		AnimationFrame newFrame(newSkeleton);
		clip->setSkeleton(newSkeleton);
		for (int i = 0; i < clip->getNumFrames(); ++i)
		{
			AnimationFrame& oldFrame = clip->getFrame(i);
			newFrame.setTime(oldFrame.getTime());

			for (int j = 0; j < newSkeleton->getNumJoints(); ++j)
			{
				int oldJointIndex = lookup[j];
				const SkeletonJoint& joint = newSkeleton->getJoint(j);

				if (oldJointIndex >= 0)
				{
					newFrame.setGlobalTransform(j, oldFrame.getGlobalTransform(oldJointIndex));
					newFrame.setLocalTransform(j, oldFrame.getLocalTransform(oldJointIndex));
				}
				else
				{
					newFrame.setLocalTransform(j, newSkeleton->getBindPose().localTransforms[j]);

					if (joint.parentIndex >= 0)
					{
						newFrame.setGlobalTransform(j, 
							newFrame.getGlobalTransform(joint.parentIndex) *
							newSkeleton->getBindPose().localTransforms[j]);
					}
					else
					{
						newFrame.setGlobalTransform(j, newSkeleton->getBindPose().jointTransforms[j]);
					}
				}

				newFrame.setSkinnedTransform(j, oldFrame.getGlobalTransform(oldJointIndex) * joint.invBindPose);
			}
			clip->setFrame(i, newFrame);
		}
		
		//delete skeleton.getResource();
		//m_model->addAnimation(clip);
	}
	else
	{
	}
	m_resources->addToRegisty(skeleton.getResource()); // TODO: <-- delete temp skeleton.

	m_resources->addToRegisty(clip.getResource());
	
	file.close();
	return clip;
}



////////////////////////////////////////////////////////////////////////////////////////
//							***** VMT MATERIAL LOADER *****
////////////////////////////////////////////////////////////////////////////////////////

Material SourceModelImporter::loadMaterialVMT(const FileInfo& fileInfo, const DirectoryInfo& textureDirectory)
{
	// Load the material from a VMT file.
	VTFLib::CVMTFile vmtFile;
	if (vmtFile.Load(fileInfo.path.c_str()) == vlFalse)
	{
		std::cerr << "Error loading VMT material \"" << fileInfo.path << "\"" << std::endl;
		return NULL;
	}
	
	MaterialResource* material = new MaterialResource();
	m_resources->addToRegisty(material);
	
	for (uint i = 0; i < vmtFile.GetRoot()->GetNodeCount(); ++i)
	{
		VTFLib::Nodes::CVMTNode* node = vmtFile.GetRoot()->GetNode(i);
		const char* name = node->GetName();

		if (node->GetType() == NODE_TYPE_STRING)
		{
			// Diffuse texture.
			if (SDL_strcasecmp(node->GetName(), "$basetexture") == 0)
			{
				FileInfo texFile = textureDirectory.path + "/" +
					((VTFLib::Nodes::CVMTStringNode*) node)->GetValue() + ".vtf";
				material->setDiffuseTexture(loadTextureVTF(texFile));
			}
		}
		else if (node->GetType() == NODE_TYPE_INTEGER)
		{
		}
		else if (node->GetType() == NODE_TYPE_SINGLE)
		{
		}
		else if (node->GetType() == NODE_TYPE_GROUP)
		{
		}
		else if (node->GetType() == NODE_TYPE_GROUP_END)
		{
		}
	}

	return material;
}



////////////////////////////////////////////////////////////////////////////////////////
//							***** VTF TEXTURE LOADER *****
////////////////////////////////////////////////////////////////////////////////////////

Texture SourceModelImporter::loadTextureVTF(const FileInfo& fileInfo)
{
	return m_resources->loadTextureVTF(fileInfo.path, fileInfo);
}
*/