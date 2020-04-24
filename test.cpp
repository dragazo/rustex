#include <iostream>
#include <type_traits>
#include <vector>
#include <stdexcept>

#undef NDEBUG
#include <cassert>

#include "rustex.h"

static_assert(!std::is_copy_constructible_v<rustex::mutex<int>>, "intrinsics error");
static_assert(!std::is_copy_assignable_v<rustex::mutex<int>>, "intrinsics error");
static_assert(!std::is_move_constructible_v<rustex::mutex<int>>, "intrinsics error");
static_assert(!std::is_move_assignable_v<rustex::mutex<int>>, "intrinsics error");

static_assert(!std::is_default_constructible_v<rustex::mutex<int>::mut_guard>, "intrinsics error");
static_assert(!std::is_copy_constructible_v<rustex::mutex<int>::mut_guard>, "intrinsics error");
static_assert(!std::is_copy_assignable_v<rustex::mutex<int>::mut_guard>, "intrinsics error");
static_assert(std::is_move_constructible_v<rustex::mutex<int>::mut_guard>, "intrinsics error");
static_assert(std::is_move_assignable_v<rustex::mutex<int>::mut_guard>, "intrinsics error");

static_assert(!std::is_default_constructible_v<rustex::mutex<int>::guard>, "intrinsics error");
static_assert(!std::is_copy_constructible_v<rustex::mutex<int>::guard>, "intrinsics error");
static_assert(!std::is_copy_assignable_v<rustex::mutex<int>::guard>, "intrinsics error");
static_assert(std::is_move_constructible_v<rustex::mutex<int>::guard>, "intrinsics error");
static_assert(std::is_move_assignable_v<rustex::mutex<int>::guard>, "intrinsics error");

static_assert(std::is_same_v<decltype(std::declval<rustex::mutex<int>::mut_guard>().operator*()), int&>, "dereference type error");
static_assert(std::is_same_v<decltype(std::declval<rustex::mutex<int>::mut_guard>().operator->()), int*>, "dereference type error");

static_assert(std::is_same_v<decltype(std::declval<rustex::mutex<int>::guard>().operator*()), const int&>, "dereference type error");
static_assert(std::is_same_v<decltype(std::declval<rustex::mutex<int>::guard>().operator->()), const int*>, "dereference type error");

rustex::mutex<int> global_val{ 563 };

int main() try
{
	{
		assert(*global_val.lock() == 563);
		*global_val.lock_mut() = -32;
		{
			auto h1 = global_val.lock();
			auto h2 = global_val.lock();

			assert(*h1 == -32);
			assert(&*h1 == &*h2);
		}
	}
	{
		rustex::mutex<int> v(55);
		assert(*v.lock() == 55);

		*v.lock_mut() = 66;
		assert(*v.lock() == 66);
		
		{
			auto q = v.lock();
			assert(!v.try_lock_mut());
		}
		{
			auto q = v.lock_mut();
			assert(!v.try_lock_mut());
			assert(!v.try_lock());
		}
		{
			(void)v.try_lock_mut();
		}
		{
			(void)v.try_lock();
		}

		*v.lock_mut() = 77;
		assert(*v.lock() == 77);

		{
			auto h = v.lock();
			assert(*h == 77);
			{
				auto h2 = std::move(h);
				assert(*h2 == 77);
			}
		}
	}
	{
		rustex::mutex<std::vector<int>> m(6);
		const auto &const_m = m;
		{
			auto h = const_m.lock();
			assert(h->size() == 6);

			auto h2 = const_m.lock();
			assert(h2->size() == 6);
		}
		{
			auto h = const_m.lock_mut();
			assert(h->size() == 6);
		}
		{
			(void)const_m.try_lock_mut();
		}
		{
			(void)const_m.try_lock();
		}
	}

	std::cout << "completed all tests successfully\n";

	return 0;
}
catch (const std::exception &ex)
{
	std::cerr << "UNHANDLED EXCEPTION: " << ex.what() << '\n';
	return 666;
}
catch (...)
{
	std::cerr << "UNHANDLED EXCEPTION OF UNKNOWN TYPE\n";
	return 999;
}
