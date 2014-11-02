#include "gtest/gtest.h"
#include <memory>

#include "exceptions.h"
#include "coffee_machine_manager.h"
#include "storage.h"

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

TEST(ut_coffee_machine_manager, set_get_ingredients)
{
	// Given:
	coffee_machine_manager manager;

	// When:
	const auto wrapped_ingredients =
			std::make_shared<storage<EIngredient, unsigned>>(ingredients);
	manager.set_ingredients(wrapped_ingredients);

	// Then:
	EXPECT_EQ(ingredients.convertToMap(), manager.get_ingredients().convertToMap());
}

TEST(ut_coffee_machine_manager, add_recipe)
{
	// Given:
	coffee_machine_manager manager;
	std::set<std::string> coffees = {"cappucino"};

	// When:
	manager.add_recipe("cappucino", 250u, cappucino);

	// Then:
	EXPECT_EQ(coffees, manager.get_coffees());
	EXPECT_EQ(250u, manager.get_price("cappucino"));
	EXPECT_EQ(cappucino, manager.get_recipe("cappucino"));
}

TEST(ut_coffee_machine_manager, get_price_ok)
{
	// Given:
	coffee_machine_manager manager;
	manager.add_recipe("cappucino", 250u, cappucino);

	// When:
	const unsigned price = manager.get_price("cappucino");

	// Then:
	EXPECT_EQ(250u, price);
}

TEST(ut_coffee_machine_manager, get_price_nok)
{
	// Given:
	const coffee_machine_manager manager;

	// When/Then:
	EXPECT_THROW(manager.get_price("cappucino"), value_error);
}

TEST(ut_coffee_machine_manager, get_recipe_ok)
{
	// Given:
	coffee_machine_manager manager;
	manager.add_recipe("cappucino", 250u, cappucino);

	// When:
	const auto recipe = manager.get_recipe("cappucino");

	// Then:
	EXPECT_EQ(cappucino, recipe);
}

TEST(ut_coffee_machine_manager, get_recipe_nok)
{
	// Given:
	const coffee_machine_manager manager;

	// When/Then:
	EXPECT_THROW(manager.get_recipe("cappucino"), value_error);
}


