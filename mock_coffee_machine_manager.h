#ifndef _MOCK_COFFEE_MACHINE_MANAGER_H_
#define _MOCK_COFFEE_MACHINE_MANAGER_H_

#include <gmock/gmock.h>
#include "abstract_coffee_machine_manager.h"
#include "abstract_storage.h"
#include <memory>

class mock_coffee_machine_manager : public abstract_coffee_machine_manager
{
public:
	MOCK_METHOD1(set_ingredients, void(std::shared_ptr<abstract_storage<EIngredient, unsigned>> ingredients));

	MOCK_METHOD3(add_recipe, void(const std::string& coffee, unsigned price,
		const std::map<EIngredient, unsigned>& recipe));

	MOCK_CONST_METHOD0(get_coffees, const std::set<std::string>& ());

	MOCK_CONST_METHOD1(get_price, unsigned(const std::string& coffee));

	MOCK_CONST_METHOD1(get_recipe, const std::map<EIngredient, unsigned>& (const std::string& coffee));

	MOCK_METHOD0(get_ingredients, abstract_storage<EIngredient, unsigned>& ());

	MOCK_METHOD2(use_ingredient, long int(EIngredient ingredient, unsigned amount));
};

#endif
