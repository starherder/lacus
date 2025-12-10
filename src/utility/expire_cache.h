#pragma once

#include <map>
#include <time.h>
#include <string>

#include "signalslot.h"


namespace utility
{

	template<typename K, typename V>
	class ExpireCache
	{
		template<typename ValType>
		struct CacheValue
		{
			std::clock_t time_stamp;
			ValType value;
		};

	public:
		using iterator = typename std::map<K, CacheValue<V>>::iterator;
		using const_iterator = typename std::map<K, CacheValue<V>>::const_iterator;

	public:
		sigslot::Signal<const K&, const V&> on_add;
		sigslot::Signal<const K&, const V&> on_update;
		sigslot::Signal<const K&, const V&> on_del;

	public:
		ExpireCache()
		{}
		ExpireCache(int ms) : m_ExpireMs(ms)
		{}
		ExpireCache(const ExpireCache& cache) = delete;


		void SetExpreTime(int ms)
		{
			m_ExpireMs = ms;
		}

		void Set(const K& key, const V& val)
		{
			auto it = m_Data.find(key);
			if (it == m_Data.end())
			{
				m_Data.emplace(key, CacheValue<V>{ std::clock(), val });
				on_add.emit(key, val);
			}
			else
			{
				auto& v = it->second;
				v.time_stamp = std::clock();
				v.value = val;
				on_update.emit(key, val);
			}

			Update();
		}

		void Remove(const K& key)
		{
			auto it = m_Data.find(key);
			if (it == m_Data.end())
			{
				return;
			}

			on_del.emit(key, it->second.value);
			m_Data.erase(it);

			Update();
		}

		void Update()
		{
			auto timestamp = std::clock();
			for (auto it = m_Data.begin(); it != m_Data.end();)
			{
				std::clock_t time_passed = (timestamp - it->second.time_stamp);

				if (time_passed >= m_ExpireMs)
				{
					on_del.emit(it->first, it->second.value);
					it = m_Data.erase(it);
				}
				else
				{
					it++;
				}
			}
		}

		bool Has(const K& key) const
		{
			auto it = m_Data.find(key);
			return (it != m_Data.end());
		}

		V& operator[](const K& key)
		{
			auto it = m_Data.find(key);
			if (it == m_Data.end())
			{
				return it->second.value;
			}

			m_Data.emplace(key, CacheValue<V>{ std::clock(), V{} });
			return m_Data.find(key)->second.value;
		}

		size_t size() const
		{
			return m_Data.size();
		}

		iterator find(const K& key)
		{
			return m_Data.find(key);
		}

		iterator begin()
		{
			return m_Data.begin();
		}

		iterator end()
		{
			return m_Data.end();
		}

		iterator erase(iterator it)
		{
			return m_Data.erase(it);
		}

		bool empty() const
		{
			return m_Data.empty();
		}

		const_iterator begin() const
		{
			return m_Data.cbegin();
		}

		const_iterator end() const
		{
			return m_Data.cend();
		}

		const_iterator find() const
		{
			return m_Data.find();
		}


	private:
		int m_ExpireMs = 60000; // 1 minute

		std::map<K, CacheValue<V>> m_Data;
	};
}