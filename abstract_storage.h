#ifndef _ABSTRACT_STORAGE_
#define _ABSTRACT_STORAGE_

#include <memory>

template<class Key, class Value>
class abstract_storage
{
public:
	
	virtual Value & operator[](const Key &key) = 0;
	virtual const std::shared_ptr<Value> find(const Key &key) const = 0;

	virtual ~abstract_storage() {}
};

#endif

