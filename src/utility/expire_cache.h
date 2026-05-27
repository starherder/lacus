#pragma once

#include <map>
#include <chrono>
#include <string>

#include "signalslot.h"


namespace utility
{

	template<typename K, typename V>
	class ExpireCache
	{
		using Clock = std::chrono::steady_clock;

		template<typename ValType>
		struct CacheValue
		{
			Clock::time_point time_stamp;
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
		ExpireCache(int ms) : _expireMs(ms)
		{}
		ExpireCache(const ExpireCache& cache) = delete;

		void setExpireTime(int ms)
		{
			_expireMs = ms;
		}

		void set(const K& key, const V& val)
		{
			auto it = _data.find(key);
			if (it == _data.end())
			{
				_data.emplace(key, CacheValue<V>{ Clock::now(), val });
				on_add.emit(key, val);
			}
			else
			{
				auto& v = it->second;
				v.time_stamp = Clock::now();
				v.value = val;
				on_update.emit(key, val);
			}

			update();
		}

		void remove(const K& key)
		{
			auto it = _data.find(key);
			if (it == _data.end())
			{
				return;
			}

			on_del.emit(key, it->second.value);
			_data.erase(it);

			update();
		}

		void update()
		{
			auto now = Clock::now();
			for (auto it = _data.begin(); it != _data.end();)
			{
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.time_stamp);

				if (elapsed.count() >= _expireMs)
				{
					on_del.emit(it->first, it->second.value);
					it = _data.erase(it);
				}
				else
				{
					it++;
				}
			}
		}

		bool has(const K& key) const
		{
			auto it = _data.find(key);
			return (it != _data.end());
		}

		V& operator[](const K& key)
		{
			auto it = _data.find(key);
			if (it != _data.end())
			{
				return it->second.value;
			}

			auto result = _data.emplace(key, CacheValue<V>{ Clock::now(), V{} });
			return result.first->second.value;
		}

		size_t size() const
		{
			return _data.size();
		}

		iterator find(const K& key)
		{
			return _data.find(key);
		}

		const_iterator find(const K& key) const
		{
			return _data.find(key);
		}

		iterator begin()
		{
			return _data.begin();
		}

		iterator end()
		{
			return _data.end();
		}

		const_iterator begin() const
		{
			return _data.cbegin();
		}

		const_iterator end() const
		{
			return _data.cend();
		}

		iterator erase(iterator it)
		{
			return _data.erase(it);
		}

		bool empty() const
		{
			return _data.empty();
		}


	private:
		int _expireMs = 60000; // 1 minute

		std::map<K, CacheValue<V>> _data;
	};
}
