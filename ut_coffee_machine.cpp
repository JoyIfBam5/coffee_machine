#include "gtest/gtest.h"

#include "coffee_machine.h"
#include "coffee_machine_manager.h"
#include "mock_coffee_machine_manager.h"
#include "mock_storage.h"
#include "mock_coffee_machine.h"
#include "exceptions.h"

/*
	ROADMAP:
		1. add more OOP, DI and plugin style a'la Uncle Bob. Refactor tests and add mocks. //DONE
			* user_credit and change should be injected and mocked
			* refactor all tests. Add When/Then. // DONE
			* port on linux. Check coverage by gcov.
		2. add multithreading <-- HARD :)
*/

/*
	* mock is derived so it is normal object with all fields and methods (maybe some overriden)

	* mock_manager is std::shared_ptr<mock_coffee_machine_manager> not 
		std::shared_ptr<abstract_coffee_machine_manager>! Why?

	* ReturnRef and Const must be included separetly (or all namespace testing)

	* some strange compilation errors ()happen during EXPECT_CALL get_ingredients when 
	  shared_ptr<abstract_storage<EIngredient, unsigned>> get_ingredients()
	  so I changed type to abstract_storage<EIngredient, unsigned>> &get_ingredients()

	* always derived : public IFoo, always public! In abother case compilation error from 
	  manager->set_ingredients(std::make_shared<storage<EIngredient, unsigned>>(ingredients));
	  ref: http//stackoverflow.com/questions/10472848/conversion-from-derived-to-base-exists-but-is-inaccessible

	* mock_storage_ is not injected but all works. Why?
	* for storage interface I need:
		-> operator[];
		-> constructor with initializer_list;
		-> iterators for range-based for; problem because of iterators;

	* be careful with auto, without & we call implicilty copy constructor

	* mock_coffee_machine was created for testing own method calls in some method call 
	  (like get_available_coffees in order_coffee)

	* without mocking I need interface like add_recipe/set_ingreddients to transfer some data to coffee machine.
	  With mocks I can simulate that by EXPECT_CALLS. So should I add set_ingreddients/add_recipe to tests?
	  Not actually, because it is redundant - set_ingredients/add_recipe have own UT.
	  With the other hand I can't mock everything so I need some 'real' calls from interface to inject 'real' data.

	* mocking may be really time-consuming if logic is non-trivial. Mocking (in gmock) may force us to 
	  reimplement/refactor some parts of code only for introducing interfaces or allowing DI. 
	  Uncle Bob avoids mocking:)

	PORTING ON LINUX:

	* be careful with *.la objects for gtest/gmock. That's libtool files, not static linked libraries (*.a).
	  I should find right lib-s in hidden lib/.lib :)

*/

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
		void SetUp()
		{

		}

		std::shared_ptr<mock_coffee_machine_manager> mock_manager;
		std::shared_ptr<mock_storage<EIngredient, unsigned>> mock_storage_;
	};

	TEST_F(coffee_machine_fixture, get_available_coffees_empty)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		const coffee_machine lcoffee_machine(mock_manager);
		const std::set<std::string> expected_coffees;
		const std::map<std::string, unsigned> expected_coffees_and_price;

		// When:
		EXPECT_CALL(*mock_manager, get_ingredients())
			.WillOnce(ReturnRef(*mock_storage_));

		EXPECT_CALL(Const(*mock_manager), get_coffees())
			.WillOnce(ReturnRef(expected_coffees));
		
		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_all_coffees_are_available)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		const coffee_machine lcoffee_machine(mock_manager);

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

		// When:
		EXPECT_CALL(*mock_manager, get_ingredients())
			.WillOnce(ReturnRef(*mock_storage_));

		EXPECT_CALL(Const(*mock_manager), get_coffees())
			.WillOnce(ReturnRef(expected_coffees));

		for (size_t i = 0; i < ingredients_and_quantities.size(); i++)
			EXPECT_CALL(Const(*mock_storage_), find(ingredients_and_quantities[i].first))
				.WillRepeatedly(Return(std::make_shared<unsigned>(ingredients_and_quantities[i].second)));
		
		for (size_t i = 0; i < coffees_and_price.size(); i++)
			EXPECT_CALL(Const(*mock_manager), get_price(coffees_and_price[i].first))
				.WillOnce(Return(coffees_and_price[i].second));

		for (size_t i = 0; i < recipes.size(); i++)
			EXPECT_CALL(Const(*mock_manager), get_recipe(coffees_and_price[i].first))
				.WillOnce(ReturnRef(recipes[i]));

		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();
		
		// Then:
		const std::map<std::string, unsigned> expected_coffees_and_price(coffees_and_price.begin(), 
			coffees_and_price.end());
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_some_coffees_are_available)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		const coffee_machine lcoffee_machine(mock_manager);

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
		EXPECT_CALL(*mock_manager, get_ingredients())
			.WillOnce(ReturnRef(*mock_storage_));

		EXPECT_CALL(Const(*mock_manager), get_coffees())
			.WillOnce(ReturnRef(expected_coffees));

		for (size_t i = 0; i < ingredients_and_quantities.size(); i++)
			EXPECT_CALL(Const(*mock_storage_), find(ingredients_and_quantities[i].first))
				.WillRepeatedly(Return(std::make_shared<unsigned>(ingredients_and_quantities[i].second)));

		for (size_t i = 0; i < coffees_and_price.size(); i++)
			EXPECT_CALL(Const(*mock_manager), get_price(coffees_and_price[i].first))
				.WillOnce(Return(coffees_and_price[i].second));

		for (size_t i = 0; i < recipes.size(); i++)
			EXPECT_CALL(Const(*mock_manager), get_recipe(coffees[i]))
				.WillOnce(ReturnRef(recipes[i]));

		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		const std::map<std::string, unsigned> expected_coffees_and_price(coffees_and_price.begin(),
			coffees_and_price.end());
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}

	TEST_F(coffee_machine_fixture, get_available_coffees_no_coffees_are_available)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		const coffee_machine lcoffee_machine(mock_manager);

		const std::set<std::string> expected_coffees = { "strong espresso" };

		const std::vector<std::pair<EIngredient, unsigned>> ingredients_and_quantities = 
			{ { EIngredient_Water, 750 }, { EIngredient_Coffee, 200 }, { EIngredient_Sugar, 1000 },
			{ EIngredient_Milk, 75 } };

		const std::map<std::string, unsigned> expected_coffees_and_price;

		// When:
		EXPECT_CALL(*mock_manager, get_ingredients())
			.WillOnce(ReturnRef(*mock_storage_));

		EXPECT_CALL(Const(*mock_manager), get_coffees())
			.WillOnce(ReturnRef(expected_coffees));

		for (size_t i = 0; i < ingredients_and_quantities.size(); i++)
		{
			EXPECT_CALL(Const(*mock_storage_), find(ingredients_and_quantities[i].first))
				.WillRepeatedly(Return(std::make_shared<unsigned>(ingredients_and_quantities[i].second)));
		}

		EXPECT_CALL(Const(*mock_manager), get_recipe("strong espresso"))
			.WillOnce(ReturnRef(constant::strong_espresso_recipe));

		auto actual_coffees_and_price = lcoffee_machine.get_available_coffees();

		// Then:
		EXPECT_EQ(expected_coffees_and_price, actual_coffees_and_price);
	}
	
	TEST_F(coffee_machine_fixture, insert_coin)
	{
		// Given:
		mock_manager.reset(new mock_coffee_machine_manager());
		coffee_machine lcoffee_machine(mock_manager);

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
	
	// Why the hell I don't get uninteresting mock call or sth like this. When I dont set EXPECT_CALL.
	// Only std::runtime_error?
	TEST_F(coffee_machine_fixture, order_coffee_is_not_available)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		std::shared_ptr<mock_coffee_machine> lmock_coffee_machine(new mock_coffee_machine(mock_manager));
		const std::map<std::string, unsigned> actual_coffees_and_price;

		// When:
		EXPECT_CALL(*lmock_coffee_machine, get_available_coffees())
			.WillOnce(Return(actual_coffees_and_price));

		// Then:
		EXPECT_THROW(lmock_coffee_machine->order_coffee("cappucino"), value_error);
	}
	
	TEST_F(coffee_machine_fixture, order_coffee_is_available_cash_is_ok)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		std::shared_ptr<mock_coffee_machine> lmock_coffee_machine(new mock_coffee_machine(mock_manager));
		const std::map<std::string, unsigned> coffee_and_price = { {"espresso", 100} };

		// When:
		EXPECT_CALL(*lmock_coffee_machine, get_available_coffees())
			.WillOnce(Return(coffee_and_price));
		EXPECT_CALL(*mock_manager, get_price("espresso"))
			.WillOnce(Return(100));
		EXPECT_CALL(*lmock_coffee_machine, make_coffee("espresso"))
			.WillOnce(Return("espresso"));

		lmock_coffee_machine->insert_coin(ECoin_1zl);
		lmock_coffee_machine->order_coffee("espresso");
		auto actual_coffee = lmock_coffee_machine->take_coffee();

		// Then:
		EXPECT_EQ("espresso", actual_coffee);
	}
	
	TEST_F(coffee_machine_fixture, take_change_credit_equal_change)
	{
		// Given:
		mock_manager.reset(new mock_coffee_machine_manager());
		coffee_machine lcoffee_machine(mock_manager);

		// When:
		lcoffee_machine.insert_coin(ECoin_1zl);
		auto change = lcoffee_machine.take_change();

		// Then:
		EXPECT_EQ(1, change[ECoin_1zl]);
	}

	TEST_F(coffee_machine_fixture, take_change_is_available_not_enaugh_cash)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		std::shared_ptr<mock_coffee_machine> lmock_coffee_machine(new mock_coffee_machine(mock_manager));

		const std::map<std::string, unsigned> available_coffee_and_price = { { "some strange coffee", 101 } };

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 0 }, { ECoin_20gr, 0 }, { ECoin_10gr, 0 }
		};

		// When:
		EXPECT_CALL(*lmock_coffee_machine, get_available_coffees())
			.Times(2)
			.WillRepeatedly(Return(available_coffee_and_price));
		EXPECT_CALL(*mock_manager, get_price("some strange coffee"))
			.Times(2)
			.WillRepeatedly(Return(101));

		lmock_coffee_machine->insert_coin(ECoin_1zl);
		lmock_coffee_machine->order_coffee("some strange coffee");
		auto actual_coffee = lmock_coffee_machine->take_coffee();

		// Then:
		EXPECT_EQ("", actual_coffee);
		EXPECT_EQ(expected_change, lmock_coffee_machine->take_change());
	}
	
	TEST_F(coffee_machine_fixture, take_change_is_available_not_enaugh_cash_some_change)
	{
		// Given:
		mock_storage_.reset(new mock_storage<EIngredient, unsigned>());
		mock_manager.reset(new mock_coffee_machine_manager());
		std::shared_ptr<mock_coffee_machine> lmock_coffee_machine(new mock_coffee_machine(mock_manager));

		const std::map<std::string, unsigned> available_coffee_and_price = { { "some strange coffee", 70 } };

		const std::map<ECoin, unsigned> expected_change =
		{
			{ ECoin_5zl, 0 }, { ECoin_2zl, 0 }, { ECoin_1zl, 1 },
			{ ECoin_50gr, 2 }, { ECoin_20gr, 1 }, { ECoin_10gr, 0 }
		};

		// When:
		EXPECT_CALL(*lmock_coffee_machine, get_available_coffees())
			.WillOnce(Return(available_coffee_and_price));
		EXPECT_CALL(*mock_manager, get_price("some strange coffee"))
			.Times(2)
			.WillRepeatedly(Return(70));
		EXPECT_CALL(*lmock_coffee_machine, make_coffee("some strange coffee"))
			.WillOnce(Return("some strange coffee"));
	
		lmock_coffee_machine->insert_coin(ECoin_1zl);
		lmock_coffee_machine->insert_coin(ECoin_20gr);
		lmock_coffee_machine->insert_coin(ECoin_20gr);
		lmock_coffee_machine->insert_coin(ECoin_50gr);
		lmock_coffee_machine->insert_coin(ECoin_50gr);
		lmock_coffee_machine->insert_coin(ECoin_50gr);
		lmock_coffee_machine->order_coffee("some strange coffee");

		auto actual_change = lmock_coffee_machine->take_change();
		auto actual_coffee = lmock_coffee_machine->take_coffee();
		
		// Then:
		EXPECT_EQ(expected_change, actual_change);
		EXPECT_EQ("some strange coffee", actual_coffee);
	}

}
