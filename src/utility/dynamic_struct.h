#pragma once

#include "dynamic_var.h"

#include <time.h>
#include <map>
#include <set>
#include <memory>

namespace utility
{
	template <typename K>
	class DynamicStruct
	{
	public:
		using Data = std::map<K, Var>;
		using NameSet = std::set<K>;
		using Iterator = typename Data::iterator;
		using ConstIterator = typename Data::const_iterator;
		using ValueType = typename DynamicStruct<K>::Data::value_type;
		using SizeType = typename DynamicStruct<K>::Data::size_type;
		using InsRetVal = std::pair<typename DynamicStruct<K>::Iterator, bool>;
		using Ptr = std::shared_ptr<DynamicStruct<K>>;

		DynamicStruct() : _data()
		{
		}

		DynamicStruct(const Data& val) : _data(val)
		{
		}

		template <typename T>
		DynamicStruct(const std::map<K, T>& val)
		{
			assignMap(val);
		}

		DynamicStruct(const DynamicStruct& other) : _data(other._data)
		{
		}

		DynamicStruct(DynamicStruct&& other) noexcept : _data(std::move(other._data))
		{
		}

		virtual ~DynamicStruct()
		{
		}

		inline Var& operator [] (const K& name)
		{
			return _data[name];
		}

		const Var& operator [] (const K& name) const
		{
			ConstIterator it = find(name);

			if (it == end()) return Var{};
			return it->second;
		}

		inline bool contains(const K& name) const
		{
			return find(name) != end();
		}

		inline Iterator find(const K& name)
		{
			return _data.find(name);
		}

		inline ConstIterator find(const K& name) const
		{
			return _data.find(name);
		}

		inline Iterator end()
		{
			return _data.end();
		}

		inline ConstIterator end() const
		{
			return _data.end();
		}

		inline Iterator begin()
		{
			return _data.begin();
		}

		inline ConstIterator begin() const
		{
			return _data.begin();
		}

		template <typename T>
		inline InsRetVal insert(const K& key, const T& value)
		{
			ValueType valueType(key, value);
			return insert(valueType);
		}

		inline InsRetVal insert(const ValueType& aPair)
		{
			return _data.insert(aPair);
		}

		inline SizeType erase(const K& key)
		{
			return _data.erase(key);
		}

		inline void erase(Iterator& it)
		{
			_data.erase(it);
		}

		inline void clear()
		{
			_data.clear();
		}

		inline void swap(DynamicStruct& other) noexcept
		{
			_data.swap(other._data);
		}

		inline bool empty() const
		{
			return _data.empty();
		}

		SizeType size() const
		{
			return _data.size();
		}

		inline NameSet members() const
		{
			NameSet keys;
			ConstIterator it = begin();
			ConstIterator itEnd = end();
			for (; it != itEnd; ++it) keys.insert(it->first);
			return keys;
		}

		inline Var getVar(const K& key) const
		{
			ConstIterator it = find(key);
			if (it == end())
			{
				throw std::exception("Key not found in DynamicStruct");
			}
			return it->second;
		}

		template<typename DefT = Var>
		inline Var getVar(const K& key, const DefT& defaultValue) const
		{
			ConstIterator it = find(key);
			if (it == end())
			{
				return defaultValue;
			}
			return it->second;
		}


	private:
		template <typename T>
		void assignMap(const T& map)
		{
			for (auto& [k, v] : map)
			{
				_data.emplace(k, v);
			}
		}

		Data _data;
	};

}