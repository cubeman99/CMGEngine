#ifndef _CMG_PHYSICS_QUICK_HULL_H_
#define _CMG_PHYSICS_QUICK_HULL_H_

#include <cmgMath/types/cmgVector3f.h>
#include <list>
#include <vector>
#include <algorithm>

struct QuickHullFace;


//-----------------------------------------------------------------------------
// QuickHullList
//-----------------------------------------------------------------------------
template <typename T>
class QuickHullList
{
public:
	QuickHullList() :
		m_head(nullptr),
		m_size(0)
	{
	}

	QuickHullList(unsigned int numElements, T* elements)
	{
		Initialize(numElements, elements);
	}

	T* Head()
	{
		return m_head;
	}

	void Initialize(unsigned int numElements, T* elements)
	{
		m_head = nullptr;
		m_size = 0;
		T* element = elements;
		for (unsigned int i = 0; i < numElements; ++i, ++element)
			Push(element);
	}

	void Clear()
	{
		m_head = nullptr;
		m_size = 0;
	}

	bool Empty() const
	{
		return (m_head == nullptr);
	}
	
	unsigned int Size() const
	{
		return m_size;
	}

	void Push(T* element)
	{
		if (m_head != nullptr)
			m_head->prev = element;
		element->prev = nullptr;
		element->next = m_head;
		m_head = element;
		m_size++;
	}

	T* Pop()
	{
		if (m_head == nullptr)
			return nullptr;
		T* element = m_head;
		m_head = m_head->next;
		if (m_head != nullptr)
			m_head->prev = nullptr;
		m_size--;
		return element;
	}

	void Insert(T* insertAfter, T* element)
	{
		element->prev = insertAfter;
		element->next = insertAfter->next;
		if (insertAfter->next != nullptr)
			insertAfter->next->prev = element;
		insertAfter->next = element;
		m_size++;
	}

	// Compare returns true if first should be before second.
	template<class Compare>
	void InsertSorted(T* element, Compare compare)
	{
		if (m_head == nullptr || compare(element, m_head))
		{
			Push(element);
		}
		else
		{
			for (T* ptr = m_head; ptr != nullptr; ptr = ptr->next)
			{
				if (ptr->next == nullptr ||
					compare(element, ptr->next))
				{
					Insert(ptr, element);
					break;
				}
			}
		}
	}

	void Remove(T* element)
	{
		if (element == m_head)
			m_head = element->next;
		if (element->next != nullptr)
			element->next->prev = element->prev;
		if (element->prev != nullptr)
			element->prev->next = element->next;
		m_size--;
	}

private:
	T* m_head;
	unsigned int m_size;
};


//-----------------------------------------------------------------------------
// QuickHullPool
//-----------------------------------------------------------------------------
template <typename T>
class QuickHullPool
{
public:
	QuickHullPool() :
		m_data(nullptr),
		m_size(0)
	{
	}

	~QuickHullPool()
	{
		FreeMemory();
	}

	T* GetData()
	{
		return m_data;
	}

	void AllocateMemory(unsigned int size)
	{
		m_data = new T[size];
		m_size =  size;
		m_freeList.Initialize(size, m_data);
	}

	void FreeMemory()
	{
		m_size = 0;
		delete [] m_data;
		m_data = nullptr;
		m_freeList.Clear();
	}

	template <typename... Args>
	T* Alloc(Args... constructorArguments)
	{
		T* element = m_freeList.Pop();
		*element = T(constructorArguments...);
		return element;
	}

	void Free(T* element)
	{
		m_freeList.Push(element);
	}

private:
	T* m_data;
	unsigned int m_size;
	QuickHullList<T> m_freeList;
};


//-----------------------------------------------------------------------------
// QuickHullConvexity
//-----------------------------------------------------------------------------
enum class QuickHullConvexity
{
	k_concave = -1,
	k_coplanar = 0,
	k_convex = 1,
};


//-----------------------------------------------------------------------------
// QuickHullVertex
//-----------------------------------------------------------------------------
struct QuickHullVertex
{
	Vector3f position;
	QuickHullVertex* next;
	QuickHullVertex* prev;
	QuickHullFace* closestFace;
	float distToFace;

	QuickHullVertex();
	QuickHullVertex(Vector3f position);
};


//-----------------------------------------------------------------------------
// QuickHullEdge
//-----------------------------------------------------------------------------
//  
//      o          CCW winding order         o
//       \                                  ^
//        \                                /
//         \                              /
//          \           e.twin           /
//           v -----------------------> /
//           o <---------------------- o  e.tail
//          /        Half-Edge e        ^
//         /                             \
//        / e.next                 e.prev \
//       v                                 \
//      o               e.face              o
//     
//-----------------------------------------------------------------------------
struct QuickHullEdge
{
	QuickHullVertex* tail;
	QuickHullEdge* twin;
	QuickHullEdge* next;
	QuickHullEdge* prev;
	QuickHullFace* face;


	bool merged;

	QuickHullEdge();
	QuickHullEdge(QuickHullVertex* tail);

	QuickHullConvexity CalcConvexity(float epsilon);
};


//-----------------------------------------------------------------------------
// QuickHullFace
//-----------------------------------------------------------------------------
struct QuickHullFace
{
	Vector3f normal;
	Vector3f centroid;
	unsigned int numEdges;
	QuickHullEdge* edge;
	QuickHullFace* next;
	QuickHullFace* prev;
	std::list<QuickHullVertex*> conflictList;
	bool removed;
	bool visible;
	bool newFace;
	
	QuickHullFace();

	// Add edge to the end of the edges list (in counter clockwise order).
	void AddEdge(QuickHullEdge* newEdge);

	float GetDistance(const Vector3f& position);

	Vector3f GetCenter();
	
	QuickHullVertex* FindConvexVertex();

	void CalcNormal();
	void CreateEdges(unsigned int numVertices, QuickHullVertex** vertices,
		QuickHullPool<QuickHullEdge>& edgePool);

	void MergeAdjacentFace(QuickHullEdge* mergeEdge,
		std::vector<QuickHullFace*>& discardedFaces);
	QuickHullFace* ConnectHalfEdges(
		QuickHullEdge* edgePrev, QuickHullEdge* edge);
};


//-----------------------------------------------------------------------------
// QuickHull
//-----------------------------------------------------------------------------
class QuickHull
{
public:
	QuickHull();
	
	void Clear();

	void DoQuickHull(unsigned int numPoints, const Vector3f* points);

	bool BuildInitialHull(unsigned int numPoints, const Vector3f* points);
	void AllocateMemory(unsigned int numPoints);
	bool FindInitialHullVertices(QuickHullVertex** initialVertices);
	void CreateTetrahedronHull(QuickHullVertex** fourVertices);
	void LinkTwinEdges();

	QuickHullVertex* NextConflictVertex();

	void AddVertexToHull(QuickHullVertex* vertex);
	void BuildHorizon(
		std::vector<QuickHullEdge*>& horizon,
		QuickHullVertex* viewPoint);
	void SearchHorizon(
		std::vector<QuickHullEdge*>& horizon,
		QuickHullVertex* viewPoint,
		QuickHullEdge* firstEdge);
	void BuildNewFaces(
		QuickHullList<QuickHullFace>& newFaces,
		std::vector<QuickHullEdge*>& horizon,
		QuickHullVertex* viewPoint);
	bool MergeFaces(
		QuickHullList<QuickHullFace>& newFaces);
	bool MergeFaces();
	bool MergeAdjacentFaces(QuickHullFace* face);
	
	void MergeFace(QuickHullEdge* edge);
	void ConnectHalfEdges(QuickHullEdge* edgePrev, QuickHullEdge* edgeNext);
	
	void ResolveOrphans(
		QuickHullList<QuickHullFace>& newFaces);

	void DeleteFacePoints(QuickHullFace* face, QuickHullFace* absorbFace = nullptr);

	QuickHullFace* CreateFace(unsigned int numVertices, QuickHullVertex** vertices);
	void LinkEdgeTwins(QuickHullEdge* edgeA, QuickHullEdge* edgeB);

	void DiscardFace(QuickHullFace* face, QuickHullFace* absorbingFace = nullptr);
	void DiscardVertex(QuickHullVertex* vertex);

	void PrintFaceInfo(QuickHullFace* face);
	int GetFaceNumber(QuickHullFace* face);
	int GetEdgeNumber(QuickHullEdge* edge);
	void ValidateFace(QuickHullFace* face);
	void ValidateEdges();

public:
	QuickHullPool<QuickHullVertex> m_poolVertices;
	QuickHullPool<QuickHullEdge> m_poolEdges;
	QuickHullPool<QuickHullFace> m_poolFaces;
	
	//std::vector<QuickHullFace*> m_newFaces;
	std::vector<QuickHullEdge*> m_horizon;
	std::vector<QuickHullEdge*> m_mergedEdges;
	QuickHullFace* m_mergedFace;

	QuickHullList<QuickHullFace> m_newFaces;

	QuickHullList<QuickHullVertex> m_unclaimed;
	//QuickHullList<QuickHullVertex> m_claimed;
	QuickHullList<QuickHullVertex> m_conflictVertices; // vertices not yet added to the hull.
	QuickHullList<QuickHullVertex> m_hullVertices;
	QuickHullList<QuickHullFace> m_hullFaces;

	float m_tolerance;
};



#endif // _CMG_PHYSICS_QUICK_HULL_H_