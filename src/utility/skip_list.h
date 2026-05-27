/* ************************************************************************
> File Name:     skiplist.h
> Author:        程序员Carl
> 微信公众号:    代码随想录
> Created Time:  Sun Dec  2 19:04:26 2018
> Description:
> ************************************************************************/

#pragma once

#include <iostream>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <vector>

#include "random.h"

template<typename K, typename V>
class Node {

public:

	Node() {}

	Node(K k, V v, int);

	~Node();

	K getKey() const;

	V getValue() const;

	void setValue(V);

	// Linear array to hold pointers to next node of different level
	Node<K, V> **forward;

	int nodeLevel;

private:
	K key;
	V value;
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
	this->key = k;
	this->value = v;
	this->nodeLevel = level;

	this->forward = new Node<K, V>*[level + 1];

	memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
};

template<typename K, typename V>
Node<K, V>::~Node() {
	delete[] forward;
};

template<typename K, typename V>
K Node<K, V>::getKey() const {
	return key;
};

template<typename K, typename V>
V Node<K, V>::getValue() const {
	return value;
};
template<typename K, typename V>
void Node<K, V>::setValue(V value) {
	this->value = value;
};

// Class template for Skip list
template <typename K, typename V>
class SkipList {

public:
	SkipList(int);
	~SkipList();
	int getRandomLevel();
	Node<K, V>* createNode(K, V, int);
	int insertElement(K, V);
	void displayList();
	bool searchElement(K);
	void deleteElement(K);
	void dumpFile();
	void loadFile();
	void clear(Node<K, V>*);
	int size();

private:
	void getKeyValueFromString(const std::string& str, std::string* key, std::string* value);
	bool isValidString(const std::string& str);

	// Maximum level of the skip list
	int _maxLevel;

	// current level of skip list
	int _skipListLevel;

	// pointer to header node
	Node<K, V>* _header;

	// file operator
	std::ofstream _fileWriter;
	std::ifstream _fileReader;

	// skiplist current element count
	int _elementCount;

	// thread safety
	std::mutex _mtx;
};

// create new node
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(const K k, const V v, int level) {
	Node<K, V>* n = new Node<K, V>(k, v, level);
	return n;
}

template<typename K, typename V>
int SkipList<K, V>::insertElement(const K key, const V value) {

	std::lock_guard<std::mutex> lock(_mtx);
	Node<K, V>* current = this->_header;

	// create update array and initialize it
	std::vector<Node<K, V>*> update(_maxLevel + 1, nullptr);

	// start form highest level of skip list
	for (int i = _skipListLevel; i >= 0; i--) {
		while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	// reached level 0 and forward pointer to right node, which is desired to insert key.
	current = current->forward[0];

	// if current node have key equal to searched key, we get it
	if (current != nullptr && current->getKey() == key) {
		std::cout << "key: " << key << ", exists" << std::endl;
		return 1;
	}

	// if current is NULL that means we have reached to end of the level
	// if current's key is not equal to key that means we have to insert node between update[0] and current node
	if (current == nullptr || current->getKey() != key) {

		// Generate a random level for node
		int randomLevel = getRandomLevel();

		// If random level is greater thar skip list's current level, initialize update value with pointer to header
		if (randomLevel > _skipListLevel) {
			for (int i = _skipListLevel + 1; i < randomLevel + 1; i++) {
				update[i] = _header;
			}
			_skipListLevel = randomLevel;
		}

		// create new node with random level generated
		Node<K, V>* insertedNode = createNode(key, value, randomLevel);

		// insert node
		for (int i = 0; i <= randomLevel; i++) {
			insertedNode->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = insertedNode;
		}
		std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
		_elementCount++;
	}
	return 0;
}

// Display skip list
template<typename K, typename V>
void SkipList<K, V>::displayList() {

	std::cout << "\n*****Skip List*****" << "\n";
	for (int i = 0; i <= _skipListLevel; i++) {
		Node<K, V>* node = this->_header->forward[i];
		std::cout << "Level " << i << ": ";
		while (node != nullptr) {
			std::cout << node->getKey() << ":" << node->getValue() << ";";
			node = node->forward[i];
		}
		std::cout << std::endl;
	}
}

// Dump data in memory to file
template<typename K, typename V>
void SkipList<K, V>::dumpFile() {

	std::cout << "dump_file-----------------" << std::endl;
	_fileWriter.open(kStoreFile);
	Node<K, V>* node = this->_header->forward[0];

	while (node != nullptr) {
		_fileWriter << node->getKey() << ":" << node->getValue() << "\n";
		std::cout << node->getKey() << ":" << node->getValue() << ";\n";
		node = node->forward[0];
	}

	_fileWriter.flush();
	_fileWriter.close();
	return;
}

// Load data from disk
template<typename K, typename V>
void SkipList<K, V>::loadFile() {

	_fileReader.open(kStoreFile);
	std::cout << "load_file-----------------" << std::endl;
	std::string line;
	std::string key;
	std::string value;
	while (getline(_fileReader, line)) {
		getKeyValueFromString(line, &key, &value);
		if (key.empty() || value.empty()) {
			continue;
		}
		insertElement(stoi(key), value);
		std::cout << "key:" << key << "value:" << value << std::endl;
	}
	_fileReader.close();
}

// Get current SkipList size
template<typename K, typename V>
int SkipList<K, V>::size() {
	return _elementCount;
}

template<typename K, typename V>
void SkipList<K, V>::getKeyValueFromString(const std::string& str, std::string* key, std::string* value) {

	if (!isValidString(str)) {
		return;
	}
	*key = str.substr(0, str.find(kDelimiter));
	*value = str.substr(str.find(kDelimiter) + 1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::isValidString(const std::string& str) {

	if (str.empty()) {
		return false;
	}
	if (str.find(kDelimiter) == std::string::npos) {
		return false;
	}
	return true;
}

// Delete element from skip list
template<typename K, typename V>
void SkipList<K, V>::deleteElement(K key) {

	std::lock_guard<std::mutex> lock(_mtx);
	Node<K, V>* current = this->_header;
	std::vector<Node<K, V>*> update(_maxLevel + 1, nullptr);

	// start from highest level of skip list
	for (int i = _skipListLevel; i >= 0; i--) {
		while (current->forward[i] != nullptr && current->forward[i]->getKey() < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];
	if (current != nullptr && current->getKey() == key) {

		// start for lowest level and delete the current node of each level
		for (int i = 0; i <= _skipListLevel; i++) {

			// if at level i, next node is not target node, break the loop.
			if (update[i]->forward[i] != current)
				break;

			update[i]->forward[i] = current->forward[i];
		}

		// Remove levels which have no elements
		while (_skipListLevel > 0 && _header->forward[_skipListLevel] == 0) {
			_skipListLevel--;
		}

		std::cout << "Successfully deleted key " << key << std::endl;
		delete current;
		_elementCount--;
	}
	return;
}

template<typename K, typename V>
bool SkipList<K, V>::searchElement(K key) {

	std::cout << "search_element-----------------" << std::endl;
	Node<K, V>* current = _header;

	// start from highest level of skip list
	for (int i = _skipListLevel; i >= 0; i--) {
		while (current->forward[i] && current->forward[i]->getKey() < key) {
			current = current->forward[i];
		}
	}

	// reached level 0 and advance pointer to right node, which we search
	current = current->forward[0];

	// if current node have key equal to searched key, we get it
	if (current and current->getKey() == key) {
		std::cout << "Found key: " << key << ", value: " << current->getValue() << std::endl;
		return true;
	}

	std::cout << "Not Found Key:" << key << std::endl;
	return false;
}

// construct skip list
template<typename K, typename V>
SkipList<K, V>::SkipList(int maxLevel) {

	this->_maxLevel = maxLevel;
	this->_skipListLevel = 0;
	this->_elementCount = 0;

	// create header node and initialize key and value to null
	K k;
	V v;
	this->_header = new Node<K, V>(k, v, _maxLevel);
};

template<typename K, typename V>
SkipList<K, V>::~SkipList() {

	if (_fileWriter.is_open()) {
		_fileWriter.close();
	}
	if (_fileReader.is_open()) {
		_fileReader.close();
	}

	if (_header->forward[0] != nullptr) {
		clear(_header->forward[0]);
	}
	delete(_header);

}
template <typename K, typename V>
void SkipList<K, V>::clear(Node<K, V>* cur)
{
	if (cur->forward[0] != nullptr) {
		clear(cur->forward[0]);
	}
	delete (cur);
}

template<typename K, typename V>
int SkipList<K, V>::getRandomLevel() {

	int k = 1;
	while (utility::RandomHelper::randomInt(0, 1)) {
		k++;
	}
	k = (k < _maxLevel) ? k : _maxLevel;
	return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
