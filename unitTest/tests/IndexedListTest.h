// TrackedIndexedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string>
#include <queue>
#include <functional>
#include "../../common/IndexedList.h"
#include "../catch.hpp"

namespace IndexedListTest
{

	class Element
	{
	public:
		int z;
	private:
		float y;
	};

	struct Point
	{
		Point(float x, float y) : x(x), y(y)
		{
		}
		float x;
		float y;
	};

	class IndexChangedHandler
	{
	public:
		void handleIndexChange(size_t oldIndex, size_t newIndex)
		{
			changes.push_back({ oldIndex, newIndex });
		}
		std::vector<std::pair<size_t, size_t>> changes;
	};
	//test init lists

	TEST_CASE("IndexedList", "[IndexedList]") {

		std::allocator_traits<std::allocator<int>> t;

		SECTION("default construction") {
			IndexedList<int> intList;
			IndexedList<Element> eList;
			SECTION("checking state after default construction") {
				REQUIRE(intList.size() == 0);
				REQUIRE(eList.size() == 0);
				REQUIRE(intList.begin() == intList.end());
				size_t endLength = intList.end() - intList.begin();
				REQUIRE(endLength == 0);
			}
		}
		SECTION("construction with initial values") {
			SECTION("with initialization list")
			{
				IndexedList<Point> list{ {0.1, -0.1}, {0.2, -0.2}, {0.3, -0.3} };
				IndexedList<int> listInt{ 0,1,2,3 };
				for (int i = 0; i < 4; ++i)
				{
					REQUIRE(listInt[i] == i);
				}

			}
			SECTION("copy const list")
			{
				IndexedList<Point> list{ {0.1, -0.1}, {0.2, -0.2}, {0.3, -0.3} };
				IndexedList<Point> listO(list);
			}
		}

		SECTION("assignment") {
			IndexedList<int> list;

			SECTION("with vect")
			{
				std::vector<int> v{ 0, 1, 2, 3, 4, 5 };
				SECTION("non const")
				{
					list.assign(v.begin(), v.end());
					auto lItr = list.begin();
					auto vItr = v.begin();
					for (size_t i = 0; i < 6; ++i)
					{
						REQUIRE(*lItr == *vItr);
						++vItr;
						++lItr;
					}
				}
				SECTION("const")
				{
					list.assign(v.cbegin(), v.cend());
					auto lItr = list.begin();
					auto vItr = v.begin();
					for (size_t i = 0; i < 6; ++i)
					{
						REQUIRE(*lItr == *vItr);
						++vItr;
						++lItr;
					}
				}
			}
			SECTION("with ini list")
			{
				std::vector<int> v{ 0, 1, 2, 3, 4, 5 };
				list.assign({ 0, 1, 2, 3, 4, 5 });
				auto lItr = list.begin();
				auto vItr = v.begin();
				for (size_t i = 0; i < 6; ++i)
				{
					REQUIRE(*lItr == *vItr);
					++vItr;
					++lItr;
				}
			}
			SECTION("copy with count")
			{
				std::vector<int> v{ 3, 3, 3 };
				const int three = 3;
				list.assign(3, three);
				auto lItr = list.begin();
				auto vItr = v.begin();
				for (size_t i = 0; i < 3; ++i)
				{
					REQUIRE(*lItr == *vItr);
					++vItr;
					++lItr;
				}
			}
		}
		SECTION("swap")
		{
			IndexedList<int> list1{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
			IndexedList<int> list2;
			REQUIRE(list1.size() == 16);
			REQUIRE(list2.size() == 0);
			list1.swap(list2);
			REQUIRE(list2.size() == 16);
			REQUIRE(list1.size() == 0);
			int intValue = 0;
			for (auto each : list2)
			{
				REQUIRE(each == intValue);
				++intValue;
			}

		}
		SECTION("size") {
			SECTION("with initialization list")
			{
				IndexedList<Point> list{ {0.1, -0.1}, {0.2, -0.2}, {0.3, -0.3} };
				REQUIRE(list.size() == 3);
				list.pop_back();
				REQUIRE(list.size() == 2);
				list.push_back({ 0.1, 0.1 });
				REQUIRE(list.size() == 3);
				list.clear();
				REQUIRE(list.size() == 0);
			}
			SECTION("empty list")
			{
				IndexedList<Point> list;
				REQUIRE(list.size() == 0);
			}
		}
		SECTION("insert, remove") {
			IndexedList<std::string> list{ "red", "velvet", "sucks" };
			IndexedList<std::string> listInt{ "1", "2", "3", "4" };
			IndexedList<Point> structList{ {0.1, 0.1}, {0.2, 0.2}, {0.3, 0.3} };
			std::string* redPtr = &list[0];
			std::string* velPtr = &list[1];
			std::string* sucPtr = &list[2];

			auto redItr = list.begin();
			auto velItr = list.begin() + 1;
			auto sucItr = list.begin() + 2;
			SECTION("various inserts") {
				structList.emplace_front(-0.001, 0.001);
				structList.emplace_back(0.001, -0.001);
				REQUIRE(structList.front().x == -0.001f);
				REQUIRE(structList.front().y == 0.001f);
				REQUIRE(structList.back().x == 0.001f);
				REQUIRE(structList.back().y == -0.001f);
			}
			SECTION("remove front") {
				list.pop_front();
				list.pop_front();
				REQUIRE(list.size() == 1);
				REQUIRE(list[0] == "sucks");
				SECTION("add front") {
					list.push_front("Pink");
					list.push_front("Black");
					REQUIRE(list.size() == 3);
					REQUIRE(list[0] == "Black");
					REQUIRE(list[1] == "Pink");
					REQUIRE(list[2] == "sucks");
				}
				SECTION("check ptr, itr")
				{
					*sucPtr == "red";
					*sucItr == "red";
				}
			}
			SECTION("remove back") {
				list.pop_back();
				list.pop_back();
				REQUIRE(list.size() == 1);
				REQUIRE(list[0] == "red");
				SECTION("add front") {
					list.push_back("is");
					list.push_back("not");
					list.push_back("Pink");
					REQUIRE(list.size() == 4);
					REQUIRE(list[0] == "red");
					REQUIRE(list[1] == "is");
					REQUIRE(list[2] == "not");
					REQUIRE(list[3] == "Pink");
				}
				SECTION("check ptr, itr")
				{
					*redItr == "red";
					*redPtr == "red";
				}
			}
			SECTION("swap and replace") {
				auto secItr = listInt.begin() + 1;
				listInt.swapAndErase(secItr);
				//makes sure that after swap, the swapped element's iterator is valid
				REQUIRE(*secItr == "4");
				//1 4 3
				REQUIRE(listInt[0] == "1");
				REQUIRE(listInt[1] == "4");
				REQUIRE(listInt[2] == "3");
				REQUIRE(listInt.size() == 3);
				SECTION("swap and replace when size()== 1") {
					listInt.swapAndErase(listInt.begin());
					listInt.swapAndErase(listInt.begin());
					listInt.swapAndErase(listInt.begin());
					REQUIRE(listInt.size() == 0);

				}
			}

			SECTION("for default generated move/copy") {
				auto secItr = structList.begin() + 1;
				structList.swapAndErase(secItr);
				REQUIRE(structList.size() == 2);

			}

		}

		SECTION("no move copy when resizing") {
			IndexedList<std::string> list{ "red", "velvet", "sucks" };
			std::string hubba("Hubba bubba");
			std::string* ptr0 = &list[0];
			std::string* ptr1 = &list[1];
			std::string* ptr2 = &list[2];
			for (size_t i = 0; i < 5000; ++i)
			{
				list.push_back(hubba);
				list.push_front(hubba);
			}
			REQUIRE(*ptr0 == "red");
			REQUIRE(*ptr1 == "velvet");
			REQUIRE(*ptr2 == "sucks");
			REQUIRE(list.size() == 3 + 5000 * 2);
		}
		SECTION("iterators")
		{
			IndexedList<int> list{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
			std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

			const IndexedList<int> cList{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
			const std::vector<int> cV{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
			SECTION("for each")
			{
				auto vItr = v.begin();
				for (auto each : list)
				{
					REQUIRE(*vItr == each);
					++vItr;
				}
			}
			SECTION("inc, dec")
			{
				auto lItr = v.begin();
				auto vItr = v.begin();
				for (auto each : list)
				{
					REQUIRE(*vItr == *lItr);
					++vItr;
					++lItr;
				}
			}
			SECTION("reverse")
			{
				auto lItr = v.rbegin();
				auto vItr = v.rbegin();
				for (auto each : list)
				{
					REQUIRE(*vItr == *lItr);
					++vItr;
					++lItr;
				}

			}

		}

		SECTION("indexChanged callbacks") {
			//tracks all changes
			IndexChangedHandler handler;

			IndexedList<int> list{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			list.addIndexChangedCallback(std::bind(&IndexChangedHandler::handleIndexChange, &handler, std::placeholders::_1, std::placeholders::_2));

			SECTION("not optimal") {
				//delete all except 0
				auto itr = list.cbegin() + 1;
				size_t oldSize = list.size();
				for (size_t i = 1; i < oldSize; ++i)
				{
					itr = list.swapAndErase(itr);
				}
				size_t newSize = list.size();
				REQUIRE(list.size() == 1);
				//first swap is 1 and 10 so 0 10 2 3 4 5 6 7 8 9
				REQUIRE(handler.changes.size() == 9);
				REQUIRE(handler.changes[0] == std::pair<size_t, size_t>(10, 1));
				REQUIRE(handler.changes[1] == std::pair<size_t, size_t>(9, 1));
				REQUIRE(handler.changes[2] == std::pair<size_t, size_t>(8, 1));
				REQUIRE(handler.changes[3] == std::pair<size_t, size_t>(7, 1));
				REQUIRE(handler.changes[4] == std::pair<size_t, size_t>(6, 1));
				REQUIRE(handler.changes[5] == std::pair<size_t, size_t>(5, 1));
				REQUIRE(handler.changes[6] == std::pair<size_t, size_t>(4, 1));
				REQUIRE(handler.changes[7] == std::pair<size_t, size_t>(3, 1));
				REQUIRE(handler.changes[8] == std::pair<size_t, size_t>(2, 1));
				//optimized so that no swap occurs if not needed
				//REQUIRE(handler.changes[9] == std::pair<size_t, size_t>(1, 1));
			}
			SECTION("optimal") {
				//delete all except 0
				auto itr = list.cbegin() + 1;
				size_t oldSize = list.size();
				list.swapAndErase(list.cbegin() + 1, list.cend());
				size_t newSize = list.size();
				REQUIRE(list.size() == 1);
				//first swap is 1 and 10 so 0 10 2 3 4 5 6 7 8 9
				REQUIRE(handler.changes.size() == 0);
				//optimized so that no swap occurs if not needed
			}


		}
	}



}