#ifndef _ABSTRACT_COFFE_MACHINE_MANAGER_H_
#define _ABSTRACT_COFFE_MACHINE_MANAGER_H_

#include <set>
#include <map>
#include <iostream>
#include <memory>

#include "ingredient.h"
#include "abstract_storage.h"

// gmock is problematic when encountering commas in return value
//typedef std::map<EIngredient, unsigned> ingredient_map;

/*!
 * \brief Coffee machine internal interface
 */
class abstract_coffee_machine_manager
{
public :

	virtual ~abstract_coffee_machine_manager() {}

	virtual void set_ingredients(std::shared_ptr<abstract_storage<EIngredient, unsigned>> ingredients) = 0;

	virtual void add_recipe(const std::string& coffee, unsigned price,
		const std::map<EIngredient, unsigned>& recipe) = 0;

	/*!
	 * \brief Get names of all registered coffees
	 */
	virtual const std::set<std::string>& get_coffees() const = 0;

	/*!
	 * \brief Get price of specified coffee
	 */
	virtual unsigned get_price(const std::string& coffee) const = 0;

	/*!
	 * \brief Get map of ingredients required by specified coffee
	 *
	 * This method should return map of ingredients that are required to make a
	 * coffee. Coffee cannot be made if one or more required ingredients are
	 * missing (i.e. if required ingredient amout > total ingredient amount).
	 *
	 * \param coffee Name of coffee (should by one from map returned by
	 *				 get_coffees() method)
	 */
	virtual const std::map<EIngredient, unsigned>& get_recipe(const std::string& coffee) const = 0;

	/*!
	 * \brief Return map of total ingredients available on coffee machine
	 *
	 * This method returns current state of available ingredients. This is used
	 * along with the get_coffee_ingredients() method to calculate list of
	 * available coffees.
	 */
	virtual abstract_storage<EIngredient, unsigned> &get_ingredients() = 0;

	/*!
	 * \brief Decrease total amount of specified ingredient by given value
	 *
	 * This should be called for every required ingredient by coffee machine
	 * when coffee is being prepared. This returns following values:
	 *
	 * 1) >0 if operation succeeded
	 * 2) 0 if operation succeeded, but there is no more ingredient left
	 * 3) -1 if operation failed
	 *
	 * \param ingredient Ingredient to be used
	 * \param amount Amount of ingredient to be used
	 */
	virtual long int use_ingredient(EIngredient ingredient, unsigned amount) = 0;
};

#endif // _ABSTRACT_COFFE_MACHINE_MANAGER_H_
