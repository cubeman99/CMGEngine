#ifndef _CMG_GRAPHICS_SCENE_NODE_H_
#define _CMG_GRAPHICS_SCENE_NODE_H_

#include <vector>
#include <cmgMath/cmg_math.h>

class SceneNode;


class SceneNodeList
{
public:
	SceneNodeList();
	SceneNodeList(const SceneNodeList& copy);
	~SceneNodeList();

	void Add(SceneNode* node);
	void Remove(SceneNode* node);
	void Clear();
	void Sort();

private:
	std::vector<SceneNode*> m_nodes;
};


class SceneNode
{
	friend class SceneNodeList;

public:
	SceneNode();

private:

	SceneNode* m_parent;
	SceneNodeList m_children;

	Transform3f m_localTransform;
	int m_nodeId;
};


#endif // _CMG_GRAPHICS_SCENE_NODE_H_