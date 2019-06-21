#ifndef _CMG_CORE_ARRAY_H_
#define _CMG_CORE_ARRAY_H_

#include <cmgCore/cmgBase.h>
#include <vector>
#include <set>
#include <map>


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