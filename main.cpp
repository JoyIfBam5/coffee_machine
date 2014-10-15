#include <iostream>
#include <memory>
#include <gtest/gtest.h>

#include "storage.h"
#include "coin.h"
#include "coffee_machine.h"
#include "coffee_machine_manager.h"

const storage<EIngredient, unsigned> ingredients  =
{
		{EIngredient_Water, 750},
		{EIngredient_Coffee, 200},
		{EIngredient_Sugar, 1000},
		{EIngredient_Milk, 500}
};

const std::map<EIngredient, unsigned> cappucino =
{
		{EIngredient_Water, 150},
		{EIngredient_Coffee, 10},
		{EIngredient_Sugar, 20},
		{EIngredient_Milk, 50}
};

void dummy_test_case()
{
	std::shared_ptr<abstract_coffee_machine_manager> manager(new coffee_machine_manager());
	std::shared_ptr<abstract_storage<ECoin, unsigned>> user_credit(new storage<ECoin, unsigned>());
	manager->set_ingredients(std::make_shared<storage<EIngredient, unsigned>>(ingredients));
	manager->add_recipe("cappucino", 200, cappucino);

	coffee_machine machine(manager, user_credit);
	std::map<std::string, unsigned> available_coffees = machine.get_available_coffees();
	machine.order_coffee("cappucino");

	machine.insert_coin(ECoin_2zl);
	machine.insert_coin(ECoin_1zl);

	std::string coffee = machine.take_coffee();
	auto change = machine.take_change();

	if (coffee != "")
		std::cout << "I'm drinking wonderful " << coffee << " coffee." << std::endl;
	else
		std::cout << "Hmm, I guess coffee machine has broken..." << std::endl;
}

int main(int argc, char **argv)
{
	dummy_test_case();
	::testing::InitGoogleTest(&argc, argv);
	int status = RUN_ALL_TESTS();
	getchar();
	return status;
}
