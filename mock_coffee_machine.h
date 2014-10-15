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

//using ::testing::_;
//using ::testing::Invoke;
//
//// delegating calls to real object is quite tricky.
//// ref: CookBook
//// Maybe I should derive by coffee_machine and mock only get_available_coffees
//class mock_coffee_machine : public abstract_coffee_machnie
//{
//public:
//
//	mock_coffee_machine(std::shared_ptr<abstract_coffee_machine_manager> manager_)
//		: m_coffee_machine(manager_)
//	{}
//
//	MOCK_CONST_METHOD0(get_available_coffees, std::map<std::string, unsigned>());
//	MOCK_METHOD1(order_coffee, void(std::string name));
//	MOCK_METHOD1(insert_coin, void(ECoin coin));
//	MOCK_METHOD0(take_change, std::map<ECoin, unsigned >());
//	MOCK_METHOD0(take_coffee, std::string());
//
//	// Delegates the default actions of the methods to a coffee_machine object.
//	// This must be called *before* the custom ON_CALL() statements.
//	void delegate_to_real() 
//	{
//		ON_CALL(*this, order_coffee(_))
//			.WillByDefault(Invoke(&m_coffee_machine, &coffee_machine::order_coffee));
//	/*	ON_CALL(*this, insert_coin(_))
//			.WillByDefault(Invoke(&m_coffee_machine, &coffee_machine::insert_coin));
//		ON_CALL(*this, take_change())
//			.WillByDefault(Invoke(&m_coffee_machine, &coffee_machine::take_change));
//		ON_CALL(*this, take_coffee())
//			.WillByDefault(Invoke(&m_coffee_machine, &coffee_machine::take_coffee));*/
//	}
//
//private:
//	coffee_machine m_coffee_machine; //it's real object so all calls inside m_coffee_machine will be real too 
//};

#endif
