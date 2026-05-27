#pragma once

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
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

	unsort_map() = default;
	~unsort_map() = default;

	unsort_map(const unsort_map& _unsortmap)
	{
		_data = _unsortmap._data;
	}

	unsort_map(unsort_map&& _unsortmap) noexcept
	{
		_data = std::move(_unsortmap._data);
	}

	unsort_map(const std::initializer_list<pair_type>& il)
	{
		for (const auto& p : il) {
			_data.push_back(p);
		}
	}

	unsort_map& operator=(unsort_map&& _unsortmap) noexcept
	{
		_data = std::move(_unsortmap._data);
		return *this;
	}

	iterator begin()
	{
		return _data.begin();
	}

	iterator end()
	{
		return _data.end();
	}

	reverse_iterator rbegin()
	{
		return _data.rbegin();
	}

	reverse_iterator rend()
	{
		return _data.rend();
	}

	const_reverse_iterator crbegin() const
	{
		return _data.rbegin();
	}

	const_reverse_iterator crend() const
	{
		return _data.rend();
	}


	const_iterator begin() const
	{
		return _data.cbegin();
	}

	const_iterator end() const
	{
		return _data.cend();
	}

	size_t size() const
	{
		return _data.size();
	}

	bool empty() const
	{
		return _data.empty();
	}

	void clear()
	{
		_data.clear();
	}

	void insert(const pair_type& p)
	{
		_data.push_back(p);
	}

	void insert(pair_type&& p)
	{
		_data.push_back(std::move(p));
	}

	void insert(iterator it, const pair_type& p)
	{
		_data.insert(it, p);
	}

	void insert(iterator it, pair_type&& p)
	{
		_data.insert(it, std::move(p));
	}

	value_type& at(const key_type& key)
	{
		for (auto& p : _data) {
			if (p.first == key) {
				return p.second;
			}
		}
		throw std::out_of_range("invalid unsort_map<K, T> key");
	}

	const value_type& at(const key_type& key) const
	{
		for (const auto& p : _data) {
			if (p.first == key) {
				return p.second;
			}
		}
		throw std::out_of_range("invalid unsort_map<K, T> key");
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
		for (auto it = _data.begin(); it != _data.end(); it++) {
			if (it->first == key) {
				return it;
			}
		}
		return _data.end();
	}

	const_iterator find(const key_type& key) const
	{
		for (auto it = _data.begin(); it != _data.end(); it++) {
			if (it->first == key) {
				return it;
			}
		}
		return _data.end();
	}

	iterator erase(iterator it)
	{
		return _data.erase(it);
	}

	iterator erase(const key_type& key)
	{
		for (auto it = _data.begin(); it != _data.end(); it++) {
			if (it->first == key) {
				return _data.erase(it);
			}
		}
		return _data.end();
	}


private:
	data_type _data;
};

}
