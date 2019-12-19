#ifndef _CMG_SHARED_POINTER_H_
#define _CMG_SHARED_POINTER_H_

#include <cmgCore/cmgBase.h>
#include <memory>

namespace cmg
{

	template <class T>
	using shared_ptr = std::shared_ptr<T>;

	template <class T>
	using unique_ptr = std::unique_ptr<T>;

	template <class T>
	using weak_ptr = std::weak_ptr<T>;

	template <class T, class... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{
		return std::make_shared<T>(args);
	}

};

#endif // _CMG_SHARED_POINTER_H_