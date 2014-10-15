#ifndef _MOCK_STORAGE_H_
#define _MOCK_STORAGE_H_

#include <gmock/gmock.h>
#include "abstract_storage.h"

// mocking opeerator? is disallowed, workaround: https//code.google.com/p/googlemock/issues/detail?id=53
template<class Key, class Value>
class mock_storage : public abstract_storage<Key, Value>
{
public:
	 MOCK_METHOD1_T(indexOperator, Value & (const Key &key));

	 virtual Value &operator[](const Key &key)
	 {
		 return indexOperator(key);
	 }
	 MOCK_CONST_METHOD1_T(find, const std::shared_ptr<Value> (const Key &key));

	 MOCK_CONST_METHOD0_T(convertToMap, std::map<Key, Value> ());
};

#endif
