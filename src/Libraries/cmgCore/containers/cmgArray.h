#ifndef _CMG_CORE_ARRAY_H_
#define _CMG_CORE_ARRAY_H_

#include <vector>
#include <set>


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


#endif // _CMG_CORE_ARRAY_H_