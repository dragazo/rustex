#ifndef DRAGAZO_RUSTEX_H
#define DRAGAZO_RUSTEX_H

#include <mutex>
#include <shared_mutex>
#include <memory>
#include <type_traits>
#include <optional>

namespace rustex
{
	// a rust-style mutex - holds a value of type T and manages access via guards around SharedLockable.
	// in order to ensure the soundness of the data contract, you should not access the stored value except directly via the handles.
	template<typename T, typename SharedLockable = std::shared_mutex>
	class mutex
	{
	private: // -- detail -- //

		template<typename T_cv, template<typename> typename Locker>
		class guard_common
		{
		private: // -- detail -- //

			T_cv *ptr;
			Locker<SharedLockable> lock;

			friend class mutex;
			explicit guard_common(const mutex &m) : ptr(std::addressof(m.raw_val)), lock(m.raw_mut) {}
			explicit guard_common(const mutex &m, std::adopt_lock_t) : ptr(std::addressof(m.raw_val)), lock(m.raw_mut, std::adopt_lock) {}

		public: // -- access -- //

			T_cv *operator->() const noexcept { return ptr; }
			T_cv &operator*() const noexcept { return *ptr; }
		};

		mutable SharedLockable raw_mut;
		mutable T raw_val;

	public: // -- guards -- //

		// when these guards are moved-from they are considered invalid and should not be used at all afterwards (like rust)
		typedef guard_common<T, std::unique_lock>       mut_guard;
		typedef guard_common<const T, std::shared_lock> guard;

	public: // -- intrinsics -- //

		// constructs the wrapped object of type T with the specified arguments
		template<typename ...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		constexpr explicit mutex(Args &&...args) : raw_val(std::forward<Args>(args)...) {}

		mutex(const mutex&) = delete;
		mutex(mutex&&) = delete;

		mutex &operator=(const mutex&) = delete;
		mutex &operator=(mutex&&) = delete;

	public: // -- access -- //

		// returns a handle that facilitates shared readonly access of the wrapped value
		[[nodiscard]] guard lock() const { return guard { *this }; }
		// returns a handle that facilitates unique read/write access of the wrapped value
		[[nodiscard]] mut_guard lock_mut() const { return mut_guard { *this }; }

		// attempts to return a handle that facilitates shared readonly access of the wrapped value. returns nullopt on failure.
		[[nodiscard]] std::optional<guard> try_lock() const
		{
			if (raw_mut.try_lock_shared()) return guard { *this, std::adopt_lock };
			else return std::nullopt;
		}
		// attempts to return a handle that facilitates unique read/write access of the wrapped value. returns nullopt on failure.
		[[nodiscard]] std::optional<mut_guard> try_lock_mut() const
		{
			if (raw_mut.try_lock()) return mut_guard { *this, std::adopt_lock };
			else return std::nullopt;
		}
	};
}

#endif
