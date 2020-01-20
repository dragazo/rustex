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
	public: // -- types -- //

		// a guard object for unique read/write access
		class mut_guard
		{
		private: // -- data -- //

			std::unique_lock<SharedLockable> m;
			T *v;

			friend class mutex;
			mut_guard(SharedLockable &a, T *b) : m(a), v(b) {}
			mut_guard(SharedLockable &a, T *b, std::adopt_lock_t) : m(a, std::adopt_lock), v(b) {}

		public: // -- access -- //

			mut_guard(const mut_guard&) = delete;
			mut_guard &operator=(const mut_guard&) = delete;

			mut_guard(mut_guard &&other) noexcept = default;
			mut_guard &operator=(mut_guard&&) = default;

			T *operator->() const noexcept { return v; }
			T &operator*() const noexcept { return *v; }
		};
		// a guard object for shared readonly access
		class guard
		{
		private: // -- data -- //

			std::shared_lock<SharedLockable> m;
			const T *v;

			friend class mutex;
			guard(SharedLockable &a, const T *b) : m(a), v(b) {}
			guard(SharedLockable &a, const T *b, std::adopt_lock_t) : m(a, std::adopt_lock), v(b) {}

		public: // -- access -- //

			guard(const guard&) = default;
			guard &operator=(const guard&) = default;

			guard(guard &&other) noexcept = default;
			guard &operator=(guard&&) = default;

			const T *operator->() const noexcept { return v; }
			const T &operator*() const noexcept { return *v; }
		};

	private: // -- data -- //

		mutable SharedLockable raw_mut;
		mutable T raw_val;

	public: // -- intrin -- //

		// constructs the wrapped object of type T with the specified (forwarded) arguments
		template<typename ...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
		explicit mutex(Args &&...args) : raw_val(std::forward<Args>(args)...) {}

		mutex(const mutex&) = delete;
		mutex(mutex&&) = delete;

		mutex &operator=(const mutex&) = delete;
		mutex &operator=(mutex&&) = delete;

	public: // -- data access -- //

		// returns a handle that facilitates shared readonly access of the wrapped value
		[[nodiscard]] guard lock() const& { return { raw_mut, std::addressof(raw_val) }; }
		// returns a handle that facilitates unique read/write access of the wrapped value
		[[nodiscard]] mut_guard lock_mut() const& { return { raw_mut, std::addressof(raw_val) }; }

		void lock() && = delete;
		void lock_mut() && = delete;

		// attempts to return a handle that facilitates shared readonly access of the wrapped value. returns nullopt on failure.
		[[nodiscard]] std::optional<guard> try_lock() const&
		{
			if (raw_mut.try_lock_shared()) return guard { raw_mut, std::addressof(raw_val), std::adopt_lock };
			else return std::nullopt;
		}
		// attempts to return a handle that facilitates unique read/write access of the wrapped value. returns nullopt on failure.
		[[nodiscard]] std::optional<mut_guard> try_lock_mut() const&
		{
			if (raw_mut.try_lock()) return mut_guard { raw_mut, std::addressof(raw_val), std::adopt_lock };
			else return std::nullopt;
		}

		void try_lock() && = delete;
		void try_lock_mut() && = delete;
	};
}

#endif
