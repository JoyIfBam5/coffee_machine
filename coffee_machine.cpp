#include <algorithm>
#include <cassert>

#include "exceptions.h"
#include "coffee_machine.h"

std::string coffee_machine::make_coffee(std::string name)
{
	const auto& recipe_for_coffee = manager->get_recipe(name);
	for (auto ingredient : recipe_for_coffee)
		manager->use_ingredient(ingredient.first, ingredient.second);

	return name;
}

std::map<std::string, unsigned> coffee_machine::get_available_coffees() const
{
	const auto& ingredients = manager->get_ingredients();
	const auto& coffees = manager->get_coffees();
	std::map<std::string, unsigned> result;

	for (auto coffee : coffees)
	{
		auto& recipe_for_coffee = manager->get_recipe(coffee);
		bool available = check_ingredients_availability(recipe_for_coffee, ingredients);
		if (available)
			result.insert( { coffee, manager->get_price(coffee) } );
	}
	return result;
}

void coffee_machine::order_coffee(std::string name)
{
	const auto coffees = get_available_coffees();

	if (coffees.find(name) != coffees.end())
	{
		unsigned price = manager->get_price(name);
		user_credit_sum = 0;

		user_credit_sum += ECoin_10gr*(*user_credit)[ECoin_10gr];
		user_credit_sum += ECoin_20gr*(*user_credit)[ECoin_20gr];
		user_credit_sum += ECoin_50gr*(*user_credit)[ECoin_50gr];
		user_credit_sum += ECoin_1zl*(*user_credit)[ECoin_1zl];
		user_credit_sum += ECoin_2zl*(*user_credit)[ECoin_2zl];
		user_credit_sum += ECoin_5zl*(*user_credit)[ECoin_5zl];

		if (user_credit_sum >= price)
			prepared_coffee = make_coffee(name);
		else
			delayed_coffee = name;
	}
	else
		throw value_error();
}

void coffee_machine::insert_coin(ECoin coin)
{
	(*user_credit)[coin]++;
}

std::map<ECoin, unsigned> coffee_machine::take_change()
{
	auto change = user_credit->convertToMap();
	if (prepared_coffee != no_coffee_available)
	{
		const unsigned price = manager->get_price(prepared_coffee);
		unsigned money_to_change = user_credit_sum - price;

		compute_change_with_greedy_approach(change, money_to_change);

		if (money_to_change > 0)
		{
			throw change_error();
		}
	}
	return change;
}

std::string coffee_machine::take_coffee()
{
	const auto coffee = prepared_coffee;
	prepared_coffee.clear();
	if (coffee != no_coffee_available)
		return coffee;
	else
	if (delayed_coffee != no_coffee_available)
	{
		order_coffee(delayed_coffee);
		delayed_coffee.clear();
		return prepared_coffee;
	}
	return no_coffee_available;
}

bool coffee_machine::check_ingredients_availability(
		const std::map<EIngredient, unsigned> &recipe_for_coffee,
		const abstract_storage<EIngredient, unsigned>& ingredients) const
{
	bool available = true;
	for (auto ingredient : recipe_for_coffee)
	{
		auto found_ingredient = ingredients.find(ingredient.first);
		assert(found_ingredient != nullptr);
		if (ingredient.second > *found_ingredient)
		{
			available = false;
			break;
		}
	}
	return available;
}

void coffee_machine::compute_change_with_greedy_approach(std::map<ECoin, unsigned> &change,
														 unsigned &money_to_change)
{
	for (auto coin_value_from_change = change.rbegin(); coin_value_from_change != change.rend();
		coin_value_from_change++)
	{
		const unsigned diff_coins = std::min(money_to_change / coin_value_from_change->first,
			coin_value_from_change->second);
		money_to_change -= coin_value_from_change->first * diff_coins;
		change[coin_value_from_change->first] = diff_coins;
	}
}
