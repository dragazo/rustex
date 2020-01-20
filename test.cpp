#include <cassert>

#include "rustex.h"

int main()
{
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

		*v.lock_mut() = 77;
		assert(*v.lock() == 77);
	}

	return 0;
}
