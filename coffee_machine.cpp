#include "exceptions.h"
#include "coffee_machine.h"
#include <algorithm>
#include <cassert>

std::string coffee_machine::make_coffee(std::string name)
{
	auto& recipe_for_coffee = manager->get_recipe(name);
	for (auto ingredient : recipe_for_coffee)
		manager->use_ingredient(ingredient.first, ingredient.second);

	return name;
}

std::map<std::string, unsigned> coffee_machine::get_available_coffees() const
{
	auto& ingredients = manager->get_ingredients();
	auto& coffees = manager->get_coffees();
	std::map<std::string, unsigned> result;

	for (auto coffee : coffees)
	{
		auto& recipe_for_coffee = manager->get_recipe(coffee);
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
		if (available)
			result.insert( { coffee, manager->get_price(coffee) } );
	}
	return result;
}

void coffee_machine::order_coffee(std::string name)
{
	auto coffees = get_available_coffees();

	if (coffees.find(name) != coffees.end())
	{
		unsigned price = manager->get_price(name);
		user_credit_sum = 0;

		user_credit_sum += ECoin_10gr*user_credit[ECoin_10gr];
		user_credit_sum += ECoin_20gr*user_credit[ECoin_20gr];
		user_credit_sum += ECoin_50gr*user_credit[ECoin_50gr];
		user_credit_sum += ECoin_1zl*user_credit[ECoin_1zl];
		user_credit_sum += ECoin_2zl*user_credit[ECoin_2zl];
		user_credit_sum += ECoin_5zl*user_credit[ECoin_5zl];

		if (user_credit_sum >= price)
		{
			prepared_coffee = make_coffee(name);
		}
		else
			delayed_coffee = name;
	}
	else
		throw value_error();

}

void coffee_machine::insert_coin(ECoin coin)
{
	user_credit[coin]++;
}

// greedy algorithm is used: begin with the greatest coin
std::map<ECoin, unsigned> coffee_machine::take_change()
{
	auto change = user_credit;
	if (prepared_coffee != "")
	{
		unsigned price = manager->get_price(prepared_coffee);
		unsigned money_to_change = user_credit_sum - price;

		for (auto coin_value_from_change = change.rbegin(); coin_value_from_change != change.rend();
			coin_value_from_change++)
		{
			unsigned diff_coins = std::min(money_to_change / coin_value_from_change->first,
				coin_value_from_change->second);
			money_to_change -= coin_value_from_change->first * diff_coins;
			change[coin_value_from_change->first] = diff_coins;
		}

		if (money_to_change > 0)
		{
			throw change_error();
		}
	}
	return change;
}

std::string coffee_machine::take_coffee()
{
	std::string coffee = prepared_coffee;
	prepared_coffee.clear();
	if (coffee != "")
		return coffee;
	else
		if (delayed_coffee != "")
		{
			order_coffee(delayed_coffee);
			delayed_coffee.clear();
			std::string coffee = prepared_coffee;
			//prepared_coffee.clear();
			return coffee;
		}
	return "";
}
