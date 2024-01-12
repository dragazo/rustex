# rustex

Mutexes are meant to be used for guarding access to shared memory.
However, with C/C++ style mutexes - even with RAII lock/unlock wrappers - you have to remember to lock the mutex that is responsible for a specific piece of shared memory.

As shared-memory parallel projects get larger and larger, more mutexes will be added to reduce contention - and each mutex corresponds to a different piece of memory.
Now you not only have to remember to lock the mutex(es) prior to use, you also have to remember which ones correspond to the data you want to access.
And heaven forbid the underlying data structure change so that you need to lock a different or an additional mutex...

Enter `rustex`. A mutex pattern designed after that of the Rust programming language.
With this design pattern, the data that is guarded by a mutex is actually *inside* that mutex object.
To access the data, the typical lock methods return a handle that acts as an RAII unlock wrapper as well as being a smart pointer to the guarded data, which can be accessed by multiple owners for reading or a single owner for read/write.

Assuming you don't abuse the system and circumvent the handle, this makes it impossible to not lock a mutex that you should have.
It also makes it easy to see when you lock a mutex that didn't need to be locked (unused handle).
It even prevents you from misusing the read/write access that your ownership of the lock gives you, which is something the typical C/C++ style mutexes simply cannot do.

Here's an example of use:

```cpp
rustex::mutex<std::string> file_name; // a rustex mutex that holds/guards a string

void foo()
{
    auto handle = file_name.lock_mut(); // lock for mutable ownership (unique read/write access)
    
    if (handle->empty()) *handle = "default.txt"; // read/write the string through the handle
    // ...
}
void bar()
{
    auto handle = file_name.lock(); // lock for immutable ownership (shared read access)
    
    if (handle->empty()) throw std::runtime_error("empty file name"); // read the string through the handle
    // ...
}
```

# Adding rustex to your project

## Simple

1. Drop [rustex.h](https://raw.githubusercontent.com/dragazo/rustex/master/rustex.h) wherever you like in your source tree

## CMake Classic

```sh
git clone https://github.com/dragazo/rustex.git
mkdir -p rustex/build
cd rustex/build
cmake ..
sudo make install
```

## CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    rustex
    GIT_REPOSITORY https://github.com/dragazo/rustex.git
)
FetchContent_MakeAvailable(rustex)
```
