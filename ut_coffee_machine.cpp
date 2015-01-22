#include "gtest/gtest.h"

#include "coffee_machine.h"
#include "coffee_machine_manager.h"
#include "mock_coffee_machine_manager.h"
#include "mock_storage.h"
#include "mock_coffee_machine.h"
#include "exceptions.h"
#include "storage.h"

using namespace testing;

namespace ut
{
	namespace constant
	{
		const storage<EIngredient, unsigned> ingredients =
		{
			{ EIngredient_Water, 750 },
			{ EIngredient_Coffee, 200 },
			{ EIngredient_Sugar, 1000 },
			{ EIngredient_Milk, 75 }
		};

		const std::map<EIngredient, unsigned> cappucino_recipe =
		{
			{ EIngredient_Water, 150 },
			{ EIngredient_Coffee, 10 },
			{ EIngredient_Sugar, 20 },
			{ EIngredient_Milk, 50 }
		};

		const std::map<EIngredient, unsigned> espresso_recipe =
		{
			{ EIngredient_Water, 150 },
			{ EIngredient_Coffee, 100 },
			{ EIngredient_Sugar, 20 },
			{ EIngredient_Milk, 75 }
		};

		const std::map<EIngredient, unsigned> some_strange_coffee_recipe =
		{
			{ EIngredient_Water, 1 },
			{ EIngredient_Coffee, 100 },
			{ EIngredient_Sugar, 999 },
			{ EIngredient_Milk, 1 }
		};

		const std::map<EIngredient, unsigned> strong_espresso_recipe =
		{
			{ EIngredient_Water, 150 },
			{ EIngredient_Coffee, 201 },
			{ EIngredient_Sugar, 20 },
			{ EIngredient_Milk, 75 }
		};
	}

	struct coffee_machine_fixture : public testing::Test
	{
		void reset_mocks()
		{
			mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
			mock_manager.reset(new mock_coffee_machine_manager());
			user_credit_.reset(new mock_storage<ECoin, unsigned>());
		}

		void reset_mock_coffee_machine()
		{
			mock_coffee_machine_.reset(
						new mock_coffee_machine(mock_manager, user_credit_));
		}

		void expect_index_operators_from_coffee_machine_constructor()
		{
			static std::vector<std::pair<ECoin, unsigned>> coins_and_quantities =
				{ { ECoin_10gr, 0 }, { ECoin_20gr, 0 }, { ECoin_50gr, 0 },
				{ ECoin_1zl, 0 }, { ECoin_2zl, 0 }, { ECoin_5zl, 0 } };

			for (size_t i = 0;i < coins_and_quantities.size(); i++)
				EXPECT_CALL(*user_credit_, indexOperator(coins_and_quantities[i].first))
					.WillRepeatedly(ReturnRef(coins_and_quantities[i].second));
		}

		void expect_getter_calls_from_get_available_coffees(const std::set<std::string>
													 &expected_coffees)
		{
			EXPECT_CALL(*mock_manager, get_ingredients())
				.WillOnce(ReturnRef(*mock_storage_));

			EXPECT_CALL(Const(*mock_manager), get_coffees())
				.WillOnce(ReturnRef(expected_coffees));
		}

		void expect_calls_from_get_available_coffees(
				const std::vector<std::pair<EIngredient, unsigned>> &ingredients_and_quantities,
				const std::vector<std::pair<std::string, unsigned>> &coffees_and_price,
				const std::vector<std::map<EIngredient, unsigned>> &recipes,
				const std::vector<std::string> &coffees)
		{
			for (size_t i = 0; i < ingredients_and_quantities.size(); i++)
				EXPECT_CALL(Const(*mock_storage_), find(ingredients_and_quantities[i].first))
					.WillRepeatedly(Return(std::make_shared<unsigned>(ingredients_and_quantities[i].second)));

			for (size_t i = 0; i < coffees_and_price.size(); i++)
				EXPECT_CALL(Const(*mock_manager), get_price(coffees_and_price[i].first))
					.WillOnce(Return(coffees_and_price[i].second));

			for (size_t i = 0; i < recipes.size(); i++)
				EXPECT_CALL(Const(*mock_manager), get_recipe(coffees[i]))
					.WillOnce(ReturnRef(recipes[i]));
		}

		void expect_calls_from_get_available_coffees_coffee_not_available(
				const std::vector<std::pair<EIngredient, unsigned>> &ingredients_and_quantities)
		{
			for (size_t i = 0; i < ingredients_and_quantities.size(); i++)
				EXPECT_CALL(Const(*mock_storage_), find(ingredients_and_quantities[i].first))
					.WillRepeatedly(Return(std::make_shared<unsigned>(ingredients_and_quantities[i].second)));

			EXPECT_CALL(Const(*mock_manager), get_recipe("strong espresso"))
				.WillOnce(ReturnRef(constant::strong_espresso_recipe));
		}

		void expect_get_available_coffees_call_from_order_coffee(
				const std::map<std::string, unsigned> &actual_coffees_and_price)
		{
			EXPECT_CALL(*mock_coffee_machine_, get_available_coffees())
				.WillOnce(Return(actual_coffees_and_price));
		}

		void expect_calls_from_order_coffee(int get_available_coffees_times, int get_price_times,
									const std::map<std::string, unsigned> &coffee_and_price,
									const std::string &coffee_name, const unsigned price)
		{
			EXPECT_CALL(*mock_coffee_machine_, get_available_coffees())
				.Times(get_available_coffees_times)
				.WillRepeatedly(Return(coffee_and_price));
			EXPECT_CALL(*mock_manager, get_price(coffee_name))
				.Times(get_price_times)
				.WillRepeatedly(Return(price));
		}

		void expect_extra_calls_from_order_coffee(
				const std::vector<std::pair<ECoin, unsigned>> expected_credit_for_order_coffee,
				const std::string &coffee_name)
		{
			static std::vector<std::pair<ECoin, unsigned>> expected_credit_for_order_coffee2;
			expected_credit_for_order_coffee2 = expected_credit_for_order_coffee;

			for (size_t i = 0;i < expected_credit_for_order_coffee2.size(); i++)
				EXPECT_CALL(*user_credit_, indexOperator(expected_credit_for_order_coffee2[i].first))
					.WillRepeatedly(ReturnRef(expected_credit_for_order_coffee2[i].second));

			EXPECT_CALL(*mock_coffee_machine_, make_coffee(coffee_name))
				.WillOnce(Return(coffee_name));
		}

		void expect_convertToMap_from_take_change(const std::map<ECoin, unsigned> &expected_change)
		{
			EXPECT_CALL(*user_credit_, convertToMap())
				.WillOnce(Return(expected_change));
		}

		std::shared_ptr<mock_coffee_machine_manager> mock_manager;
		std::shared_ptr<mock_storage<EIngredient, unsigned>> mock_storage_;
		std::shared_ptr<mock_storage<ECoin, unsigned>> user_credit_;
		std::shared_ptr<mock_coffee_machine> mock_coffee_machine_;
	};

	TEST_F(coffee_machine_fixture, get_available_coffees_empty)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();

		const coffee_machine lcoffee_machine(mock_manager, user_credit_);
		const std::set<std::string> expected_coffees;
		const std::map<std::string, unsigned> expected_coffees_and_price;

		// When:
		expect_getter_calls_from_get_available_coffees(expected_coffees);
		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_all_coffees_are_available)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		const coffee_machine lcoffee_machine(mock_manager, user_credit_);

		const std::set<std::string> expected_coffees = 
			{ "cappucino", "espresso", "some strange coffee" };

		const std::vector<std::pair<EIngredient, unsigned>> ingredients_and_quantities =
			{ { EIngredient_Water, 750 }, { EIngredient_Coffee, 200 }, { EIngredient_Sugar, 1000 },
			{ EIngredient_Milk, 75 } };

		const std::vector<std::pair<std::string, unsigned>> coffees_and_price =
		{
			{ "cappucino", 250 }, { "espresso", 500 }, { "some strange coffee", 123 }
		};

		const std::vector<std::map<EIngredient, unsigned>> recipes =
			{ constant::cappucino_recipe, constant::espresso_recipe, 
				constant::some_strange_coffee_recipe };

		const std::vector<std::string> coffees = {"cappucino", "espresso",
			"some strange coffee" };

		// When:
		expect_getter_calls_from_get_available_coffees(expected_coffees);
		expect_calls_from_get_available_coffees(ingredients_and_quantities,
												coffees_and_price, recipes, coffees);
		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();
		
		// Then:
		const std::map<std::string, unsigned> expected_coffees_and_price(coffees_and_price.begin(), 
			coffees_and_price.end());
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_some_coffees_are_available)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		const coffee_machine lcoffee_machine(mock_manager, user_credit_);

		const std::set<std::string> expected_coffees = { "strong espresso", "cappucino", "espresso", 
			"some strange coffee" };

		const std::vector<std::string> coffees = { "strong espresso", "cappucino", "espresso",
			"some strange coffee" };

		const std::vector<std::pair<EIngredient, unsigned>> ingredients_and_quantities =
			{ { EIngredient_Water, 750 }, { EIngredient_Coffee, 200 }, { EIngredient_Sugar, 1000 }, 
			{ EIngredient_Milk, 75 } };

		const std::vector<std::pair<std::string, unsigned>> coffees_and_price =
		{
			{ "cappucino", 250 }, { "espresso", 500 }, { "some strange coffee", 123 }
		};

		const std::vector<std::map<EIngredient, unsigned>> recipes =
			{ constant::strong_espresso_recipe, constant::cappucino_recipe, constant::espresso_recipe,
				constant::some_strange_coffee_recipe };

		// When:
		expect_getter_calls_from_get_available_coffees(expected_coffees);
		expect_calls_from_get_available_coffees(ingredients_and_quantities,
												coffees_and_price, recipes, coffees);

		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		const std::map<std::string, unsigned> expected_coffees_and_price(coffees_and_price.begin(),
			coffees_and_price.end());
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_no_coffees_are_available)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		const coffee_machine lcoffee_machine(mock_manager, user_credit_);

		const std::set<std::string> expected_coffees = { "strong espresso" };

		const std::vector<std::pair<EIngredient, unsigned>> ingredients_and_quantities = 
			{ { EIngredient_Water, 750 }, { EIngredient_Coffee, 200 }, { EIngredient_Sugar, 1000 },
			{ EIngredient_Milk, 75 } };

		const std::map<std::string, unsigned> expected_coffees_and_price;

		// When:
		expect_getter_calls_from_get_available_coffees(expected_coffees);
		expect_calls_from_get_available_coffees_coffee_not_available(ingredients_and_quantities);
		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}
	
	TEST_F(coffee_machine_fixture, insert_coin)
	{
		// Given:
		mock_manager.reset(new mock_coffee_machine_manager());
		std::shared_ptr<abstract_storage<ECoin, unsigned>> user_credit(new storage<ECoin, unsigned>());
		coffee_machine lcoffee_machine(mock_manager, user_credit);

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 1 }, { ECoin_2zl, 0 }, { ECoin_1zl, 2 },
			{ ECoin_50gr, 0 }, { ECoin_20gr, 0 }, { ECoin_10gr, 2 }
		};
	
		// When:
		lcoffee_machine.insert_coin(ECoin_1zl);
		lcoffee_machine.insert_coin(ECoin_10gr);
		lcoffee_machine.insert_coin(ECoin_10gr);
		lcoffee_machine.insert_coin(ECoin_5zl);
		lcoffee_machine.insert_coin(ECoin_1zl);
		auto actual_change = lcoffee_machine.take_change();
	
		// Then:
		EXPECT_EQ(expected_change, actual_change);
	}

	TEST_F(coffee_machine_fixture, order_coffee_is_not_available)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		reset_mock_coffee_machine();
		const std::map<std::string, unsigned> actual_coffees_and_price;

		// When:
		expect_get_available_coffees_call_from_order_coffee(actual_coffees_and_price);

		// Then:
		EXPECT_THROW(mock_coffee_machine_->order_coffee("cappucino"), value_error);
	}
	
	TEST_F(coffee_machine_fixture, order_coffee_is_available_cash_is_ok)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		reset_mock_coffee_machine();
		const std::map<std::string, unsigned> coffee_and_price = { {"espresso", 100} };

		std::vector<std::pair<ECoin, unsigned>> expected_credit_for_order_coffee =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 0 }, { ECoin_20gr, 0 }, { ECoin_10gr, 0 }
		};

		// When:
		expect_calls_from_order_coffee(1, 1, coffee_and_price, "espresso", 100);
		expect_extra_calls_from_order_coffee(expected_credit_for_order_coffee, "espresso");

		mock_coffee_machine_->insert_coin(ECoin_1zl);
		mock_coffee_machine_->order_coffee("espresso");
		auto actual_coffee = mock_coffee_machine_->take_coffee();

		// Then:
		EXPECT_EQ("espresso", actual_coffee);
	}
	
	TEST_F(coffee_machine_fixture, take_change_credit_equal_change)
	{
		// Given:
		mock_manager.reset(new mock_coffee_machine_manager());
		user_credit_.reset(new mock_storage<ECoin, unsigned>());

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 0 }, { ECoin_20gr, 0 }, { ECoin_10gr, 0 }
		};

		expect_index_operators_from_coffee_machine_constructor();
		coffee_machine lcoffee_machine(mock_manager, user_credit_);

		// When:
		expect_convertToMap_from_take_change(expected_change);

		lcoffee_machine.insert_coin(ECoin_1zl);
		auto change = lcoffee_machine.take_change();

		// Then:
		EXPECT_EQ(1, change[ECoin_1zl]);
	}

	TEST_F(coffee_machine_fixture, take_change_is_available_not_enaugh_cash)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		reset_mock_coffee_machine();

		const std::map<std::string, unsigned> available_coffee_and_price = { { "some strange coffee", 101 } };

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 0 }, { ECoin_20gr, 0 }, { ECoin_10gr, 0 }
		};

		// When:
		expect_calls_from_order_coffee(2, 2, available_coffee_and_price,
									   "some strange coffee", 101);
		expect_convertToMap_from_take_change(expected_change);

		mock_coffee_machine_->insert_coin(ECoin_1zl);
		mock_coffee_machine_->order_coffee("some strange coffee");
		auto actual_coffee = mock_coffee_machine_->take_coffee();

		// Then:
		EXPECT_EQ("", actual_coffee);
		EXPECT_EQ(expected_change, mock_coffee_machine_->take_change());
	}
	
	TEST_F(coffee_machine_fixture, take_change_is_available_not_enaugh_cash_some_change)
	{
		// Given:
		reset_mocks();
		expect_index_operators_from_coffee_machine_constructor();
		reset_mock_coffee_machine();

		const std::map<std::string, unsigned> available_coffee_and_price = { { "some strange coffee", 70 } };

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 2 }, { ECoin_20gr, 1 }, { ECoin_10gr, 0 }
		};

		std::vector<std::pair<ECoin, unsigned>> expected_credit_for_order_coffee =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 3 }, { ECoin_20gr, 2 }, { ECoin_10gr, 0 }
		};

		// When:
		expect_calls_from_order_coffee(1, 2, available_coffee_and_price,
									   "some strange coffee", 70);
		expect_extra_calls_from_order_coffee(expected_credit_for_order_coffee,
											 "some strange coffee");
		expect_convertToMap_from_take_change(expected_change);
	
		mock_coffee_machine_->order_coffee("some strange coffee");

		auto actual_change = mock_coffee_machine_->take_change();
		auto actual_coffee = mock_coffee_machine_->take_coffee();
		
		// Then:
		EXPECT_EQ(expected_change, actual_change);
		EXPECT_EQ("some strange coffee", actual_coffee);
	}
}
