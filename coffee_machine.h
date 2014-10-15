#ifndef _COFFEE_MACHINE_H_
#define _COFFEE_MACHINE_H_

#include <map>
#include <memory>
#include <functional>
#include <assert.h>

#include "coin.h"
#include "abstract_coffee_machine_manager.h"
#include "abstract_coffee_machnie.h"
#include "abstract_storage.h"

/*!
 * \brief Coffee machine main class
 *
 * This class simulates user interface of real coffee machine. It provides
 * methods for menu browsing, coffee choosing, coin insertion etc.
 */
class coffee_machine : public abstract_coffee_machnie
{
public :

	coffee_machine(std::shared_ptr<abstract_coffee_machine_manager> manager_,
				   std::shared_ptr<abstract_storage<ECoin, unsigned>> user_credit_) :
		manager(manager_),
		user_credit(user_credit_),
		prepared_coffee(""),
		delayed_coffee(""),
		user_credit_sum(0)
		{
			assert(user_credit != nullptr);
			(*user_credit)[ECoin_10gr] = 0;
			(*user_credit)[ECoin_20gr] = 0;
			(*user_credit)[ECoin_50gr] = 0;
			(*user_credit)[ECoin_1zl] = 0;
			(*user_credit)[ECoin_2zl] = 0;
			(*user_credit)[ECoin_5zl] = 0;
		}

	~coffee_machine() {}

	/*!
	 * \brief Return map of coffe name and its price
	 *
	 * The map returned by this method contains coffees that can be ordered by
	 * calling order_coffee() method.
	 */
	std::map<std::string, unsigned> get_available_coffees() const;

	/*!
	 * \brief Order coffee
	 *
	 * This method is called to start coffee production process. If there
	 * already are coins inserted and credit >= chosen coffee price, production
	 * starts immediately. Otherwise, coffee machine awaits for coin insertion
	 * and coffee production is delayed until required credit is reached.
	 *
	 * The name of coffee should be one of returned by get_available_coffees()
	 * method. If the name is invalid, value_error exception is thrown.
	 */
	void order_coffee(std::string name);

	/*!
	 * \brief Insert single coin to coffee machine
	 *
	 * This method increases user credit according to value of inserted coin.
	 *
	 * \param coin The coin that is inserted
	 */
	void insert_coin(ECoin coin);

	/*!
	 * \brief Take change from coffee machine
	 *
	 * This method returns map of coins that are returned by coffee machine.
	 * The results can be following:
	 *
	 * 1) No coins were inserted -> empty map is returned
	 * 2) n-coins were inserted, but no coffee was ordered -> n-coins are
	 *    returned
	 * 3) n-coins were inserted and coffee was ordered -> apropriate change is
	 *    returned
	 *
	 * If it turns out to be impossible to return proper change (ex. there's
	 * not enough coins in the system or the price requires coins not accepted
	 * by the machine), change_error is thrown.
	 */
	std::map<ECoin, unsigned > take_change();

	/*!
	 * \brief Pick up ordered coffee
	 * \returns Ordered coffee's name or nothing if there is no coffee to take
	 */
	std::string take_coffee();

	std::string make_coffee(std::string name);

private:
	const std::shared_ptr<abstract_coffee_machine_manager> manager;
	//std::map<ECoin, unsigned> user_credit;
	std::shared_ptr<abstract_storage<ECoin, unsigned>> user_credit;
	std::string prepared_coffee, delayed_coffee;
	unsigned user_credit_sum;
};

#endif
