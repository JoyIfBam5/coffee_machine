#include <stdexcept>

#include "exceptions.h"
#include "coffee_machine_manager.h"

unsigned coffee_machine_manager::get_price(const std::string& coffee) const
{
	const auto found = coffees.find(coffee);
	if (found != coffees.end()) 
		return price.at(*found);
	else 
		throw value_error();
}

const std::map<EIngredient, unsigned>& coffee_machine_manager::get_recipe(const std::string& coffee) const
{
	const auto found = coffees.find(coffee);
	if (found != coffees.end()) 
		return recipe.at(*found);
	else 
		throw value_error();
}

long int coffee_machine_manager::use_ingredient(EIngredient ingredient, unsigned amount)
{
	const auto found = ingredients->find(ingredient);
	if (found != nullptr) 
	{
		if ((*ingredients)[ingredient] >= amount) 
			return (*ingredients)[ingredient] -= amount;
		else 
			return -1;
	} 
	else 
		throw value_error();
}
