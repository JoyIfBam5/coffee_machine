#ifndef _MOCK_COFFEE_MACHINE_
#define _MOCK_COFFEE_MACHINE_

#include <string>
#include <gmock/gmock.h>

#include "coffee_machine.h"
#include "abstract_coffee_machine_manager.h"
#include "coffee_machine.h"

class mock_coffee_machine : public coffee_machine
{
public:

	mock_coffee_machine(std::shared_ptr<abstract_coffee_machine_manager> manager_,
						std::shared_ptr<abstract_storage<ECoin, unsigned>> user_credit_)
			: coffee_machine(manager_, user_credit_)
		{}

	MOCK_CONST_METHOD0(get_available_coffees, std::map<std::string, unsigned>());
	MOCK_METHOD1(make_coffee, std::string (std::string name));
};

#endif
