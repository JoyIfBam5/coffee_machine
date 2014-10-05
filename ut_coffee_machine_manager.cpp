#include "gtest/gtest.h"

#include "exceptions.h"
#include "coffee_machine_manager.h"
#include "storage.h"
#include <memory>

const storage<EIngredient, unsigned> ingredients =
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

//TEST(ut_coffee_machine_manager, set_get_ingredients)
//{
//	// Given:
//	coffee_machine_manager manager;

//	// When:
//	auto shared_ing = std::make_shared<storage<EIngredient, unsigned>>(ingredients);
//	manager.set_ingredients(shared_ing);

//	// Then:
//	auto foo = static_cast<std::shared_ptr<abstract_storage<EIngredient, unsigned>>>(shared_ing);
//	EXPECT_EQ(manager.get_ingredients(),  *foo);
//}

TEST(ut_coffee_machine_manager, add_recipe)
{
	// Given:
	coffee_machine_manager manager;
	std::set<std::string> coffees = {"cappucino"};

	// When:
	manager.add_recipe("cappucino", 250u, cappucino);

	// Then:
	EXPECT_EQ(manager.get_coffees(), coffees);
	EXPECT_EQ(manager.get_price("cappucino"), 250u);
	EXPECT_EQ(manager.get_recipe("cappucino"), cappucino);
}

TEST(ut_coffee_machine_manager, get_price_ok)
{
	// Given:
	coffee_machine_manager manager;
	manager.add_recipe("cappucino", 250u, cappucino);

	// When:
	unsigned price = manager.get_price("cappucino");

	// Then:
	EXPECT_EQ(price, 250u);
}

TEST(ut_coffee_machine_manager, get_price_nok)
{
	// Given:
	coffee_machine_manager manager;

	// When/Then:
	EXPECT_THROW(manager.get_price("cappucino"), value_error);
}

TEST(ut_coffee_machine_manager, get_recipe_ok)
{
	// Given:
	coffee_machine_manager manager;
	manager.add_recipe("cappucino", 250u, cappucino);

	// When:
	std::map<EIngredient, unsigned> recipe = manager.get_recipe("cappucino");

	// Then:
	EXPECT_EQ(recipe, cappucino);
}

TEST(ut_coffee_machine_manager, get_recipe_nok)
{
	// Given:
	coffee_machine_manager manager;

	// When/Then:
	EXPECT_THROW(manager.get_recipe("cappucino"), value_error);
}


