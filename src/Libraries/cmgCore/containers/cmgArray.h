#ifndef _CMG_CORE_ARRAY_H_
#define _CMG_CORE_ARRAY_H_

#include <cmgCore/cmgBase.h>
#include <vector>
#include <set>
#include <map>

namespace cmg { namespace container {

template<class T_Array, typename T_Item>
inline int32 GetIndex(const T_Array& container, T_Item item)
{
	auto it = std::find(container.begin(), container.end(), item);
	if (it != container.end())
		return (it - container.begin());
	return -1;
}

template<class T_Array, typename T_Item>
inline bool Contains(const T_Array& container, T_Item item)
{
	auto it = std::find(container.begin(), container.end(), item);
	return (it != container.end());
}

template<class T_Array, typename T_Item>
inline bool EraseIfFound(T_Array& container, T_Item item)
{
	auto it = std::find(container.begin(), container.end(), item);
	if (it != container.end())
	{
		container.erase(it);
		return true;
	}
	return false;
}

};};

template <typename T>
using Set = std::set<T>;
template <typename T>
using Array = std::vector<T>;
template <typename K, typename V>
using Map = std::map<K, V>;

/*
template <typename T>
class Set : public std::set<T>
{
public:
	Set() {}

private:

};

template <typename T>
class Array : public std::vector<T>
{
public:
	Array() {}

private:

};

template <typename K, typename V>
class Map : public std::map<K, V>
{
public:
	Map() {}

private:

};
*/

#endif // _CMG_CORE_ARRAY_H_