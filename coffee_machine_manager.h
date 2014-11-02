#ifndef _COFFE_MACHINE_MANAGER_H_
#define _COFFE_MACHINE_MANAGER_H_

#include <map>
#include <set>
#include <string>

#include "ingredient.h"
#include "abstract_coffee_machine_manager.h"
#include "abstract_storage.h"


class coffee_machine_manager : public abstract_coffee_machine_manager
{
  public:
	inline void set_ingredients(std::shared_ptr<abstract_storage<EIngredient, unsigned>> ingredients_)
	{
		ingredients = ingredients_;
	}

	inline void add_recipe(const std::string& coffee, unsigned price_,
			const std::map<EIngredient, unsigned>& recipe_)
	{
		coffees.insert(coffee);
		price[coffee] = price_;
		recipe[coffee] = recipe_;
	}

	inline const std::set<std::string>& get_coffees() const
	{
		return coffees;
	}

	unsigned get_price(const std::string& coffee) const;

	const std::map<EIngredient, unsigned>& get_recipe(const std::string& coffee) const;

	abstract_storage<EIngredient, unsigned>& get_ingredients()
	{
		return *ingredients;
	}

	long int use_ingredient(EIngredient ingredient, unsigned amount);

  private:
	std::set<std::string> coffees;
	std::map<std::string, unsigned> price;
	std::map<std::string, std::map<EIngredient, unsigned>> recipe;
	std::shared_ptr<abstract_storage<EIngredient, unsigned>> ingredients;
};

#endif
