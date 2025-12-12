#pragma once

#include <map>
#include <vector>
#include <string>
#include <xutility>
#include <initializer_list>

namespace utility {

// 非排序map，元素顺序就是元素插入的顺序 
// std::map 会自动根据key值排序
// std::unordered_map 无序，也不按照记录插入顺序存放
template<typename KT, typename VT>
class unsort_map
{
public:
	using key_type = KT;
	using value_type = VT;

	using pair_type = std::pair<key_type, value_type>;
	using data_type = std::vector<pair_type>;

	using iterator = typename data_type::iterator;
	using const_iterator = typename data_type::const_iterator;
	using reverse_iterator = typename data_type::reverse_iterator;
	using const_reverse_iterator = typename data_type::const_reverse_iterator;

	unsort_map() { }
	~unsort_map() { }

	unsort_map(const unsort_map& _unsortmap)
	{
		m_data = _unsortmap.m_data;
	}

	unsort_map(unsort_map&& _unsortmap)
	{
		m_data = _unsortmap.m_data;
	}

	unsort_map(const std::initializer_list<pair_type>& il)
	{
		for (const auto& p : il) {
			m_data.push_back(p);
		}
	}

	unsort_map& operator=(unsort_map&& _unsortmap)
	{
		m_data = _unsortmap;
		return this;
	}

	iterator begin()
	{
		return m_data.begin();
	}

	iterator end()
	{
		return m_data.end();
	}

	reverse_iterator rbegin()
	{
		return m_data.rbegin();
	}

	reverse_iterator rend()
	{
		return m_data.rend();
	}

	const_reverse_iterator crbegin() const
	{
		return m_data.rbegin();
	}

	const_reverse_iterator crend() const
	{
		return m_data.rend();
	}


	const_iterator begin() const
	{
		return m_data.cbegin();
	}

	const_iterator end() const
	{
		return m_data.cend();
	}

	size_t size() const
	{
		return m_data.size();
	}

	bool empty() const
	{
		return m_data.empty();
	}

	void clear()
	{
		m_data.clear();
	}

	void insert(const pair_type& p)
	{
		m_data.push_back(p);
	}
	
	void insert(pair_type&& p)
	{
		m_data.push_back(std::forward<pair_type>(p));
	}

	void insert(iterator it, const pair_type& p)
	{
		m_data.insert(it, p);
	}

	void insert(iterator it, pair_type&& p)
	{
		m_data.insert(it, std::forward<pair_type>(p));
	}
	
	value_type& at(const key_type& key)
	{
		for (auto& p : m_data) {
			if (p.first == key) {
				return p.second;
			}
		}
		std::_Xout_of_range("invalid UnSortMap<K, T> key");
	}

	value_type& operator[](const key_type& key)
	{
		if (find(key) == end())
		{
			insert({ key, value_type() });
		}
		return at(key);
	}

	iterator find(const key_type& key)
	{
		for (auto it = m_data.begin(); it != m_data.end(); it++) {
			if (it->first == key) {
				return it;
			}
		}
		return m_data.end();
	}

	const_iterator find(const key_type& key) const
	{
		for (auto it = m_data.begin(); it != m_data.end(); it++) {
			if (it->first == key) {
				return it;
			}
		}
		return m_data.end();
	}

	iterator erase(iterator it)
	{
		return m_data.erase(it);
	}

	iterator erase(const key_type& key)
	{
		for (auto it = m_data.begin(); it != m_data.end(); it++) {
			if (it->first == key) {
				return m_data.erase(it);
			}
		}
		return m_data.end();
	}


private:
	data_type m_data;
};

}