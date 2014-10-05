#ifndef _ABSTRACT_COFFEE_MACHINE_
#define _ABSTRACT_COFFEE_MACHINE_

#include <map>
#include <string>
#include "coin.h"

class abstract_coffee_machnie
{
public:
	virtual std::map<std::string, unsigned> get_available_coffees() const = 0;
	virtual void order_coffee(std::string name) = 0;
	virtual void insert_coin(ECoin coin) = 0;
	virtual std::map<ECoin, unsigned > take_change() = 0;
	virtual std::string take_coffee() = 0;
	virtual std::string make_coffee(std::string name) = 0;

	virtual ~abstract_coffee_machnie() {}
};

#endif
