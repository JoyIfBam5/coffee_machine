#ifndef _STORAGE_
#define _STORAGE_

#include "abstract_storage.h"
#include <utility>
#include <map>
#include <initializer_list>

template <class Key, class Value>
class storage : public abstract_storage<Key, Value>
{
public:
	storage() {}

	storage(std::initializer_list<std::pair<const Key,Value>> list)
		: internal_storage(list)
	{
	}

	Value & operator[](const Key &key)
	{
		return internal_storage[key];
	}

	const std::shared_ptr<Value> find(const Key &key) const
	{
		auto iterator_to_element = internal_storage.find(key);
		if (iterator_to_element == internal_storage.end())
			return nullptr;
		else
			return std::make_shared<Value>(iterator_to_element->second);
	}

private:
	std::map<Key, Value> internal_storage;
};

#endif
