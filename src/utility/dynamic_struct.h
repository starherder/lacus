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

		DynamicStruct() : m_Data()
		{
		}

		DynamicStruct(const Data& val) : m_Data(val)
		{
		}

		template <typename T>
		DynamicStruct(const std::map<K, T>& val)
		{
			assignMap(val);
		}

		virtual ~DynamicStruct()
		{
		}

		inline Var& operator [] (const K& name)
		{
			return m_Data[name];
		}

		const Var& operator [] (const K& name) const
		{
			ConstIterator it = find(name);

			std::string info = name + " NOT found.";
			if (it == end()) throw std::exception(info);
			return it->second;
		}

		inline bool contains(const K& name) const
		{
			return find(name) != end();
		}

		inline Iterator find(const K& name)
		{
			return m_Data.find(name);
		}

		inline ConstIterator find(const K& name) const
		{
			return m_Data.find(name);
		}

		inline Iterator end()
		{
			return m_Data.end();
		}

		inline ConstIterator end() const
		{
			return m_Data.end();
		}

		inline Iterator begin()
		{
			return m_Data.begin();
		}

		inline ConstIterator begin() const
		{
			return m_Data.begin();
		}

		template <typename T>
		inline InsRetVal insert(const K& key, const T& value)
		{
			ValueType valueType(key, value);
			return insert(valueType);
		}

		inline InsRetVal insert(const ValueType& aPair)
		{
			return m_Data.insert(aPair);
		}

		inline SizeType erase(const K& key)
		{
			return m_Data.erase(key);
		}

		inline void erase(Iterator& it)
		{
			m_Data.erase(it);
		}

		inline void clear()
		{
			m_Data.clear();
		}

		inline void swap(DynamicStruct& other) noexcept
		{
			m_Data.swap(other.m_Data);
		}

		inline bool empty() const
		{
			return m_Data.empty();
		}

		SizeType size() const
		{
			return m_Data.size();
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
				throw NotFoundException("Key not found in DynamicStruct");
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
				m_Data.emplace(k, v);
			}
		}

		Data m_Data;
	};

}