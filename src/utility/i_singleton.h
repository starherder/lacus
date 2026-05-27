#pragma once
#include <assert.h>

namespace utility
{

	// 单件类
	template<class T>
	class ISingleton
	{
	public:
		virtual ~ISingleton() {}

		static T& inst()
		{
			static T inst;
			return inst;
		}

		static T* instptr()
		{
			return &inst();
		}

	protected:
		ISingleton() = default;
		ISingleton(const ISingleton&) = delete;
		ISingleton& operator=(const ISingleton&) = delete;
		ISingleton(ISingleton&&) = delete;
		ISingleton& operator=(ISingleton&&) = delete;
	};

}