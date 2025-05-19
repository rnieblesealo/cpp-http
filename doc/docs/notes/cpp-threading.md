---
sidebar_position: 3
---

# CPP - Threading

We create a thread like this: `std::thread(func, arg1, arg2, arg3, ...)`
> First arg is a function, subsequent args are params we pass to that function

### `.detach()` and `.join()`

When doing `std::thread()` we must also call either `detach()` or `join()` to specify whether the thread will async or block 

```
std::thread(...).detach() 
std::thread(...).join()
```

`detach()` makes the thread asynchronous
- It is now unmanaged; *its own thing*

`join()` makes the thread blocking
- It ensures that our main thread waits for the `.join()`ed thread to finish before moving on   

```cpp title="threading-example.cpp"
void foo();

std::thread t(foo);     // execute foo immediately

// ...do other stuff

t.join();               // even if the other stuff is done first, 
                        // we can't go past here unless foo also finishes
```

**Bear in mind that a new thread begins running immediately after creating it, not when we call `.detach()` or `.join()`!**

If we never call `.detach()` or `.join()`, C++ will call `std::terminate()`, crashing our program
- C++ doesn't know what the f**k to do with the thread
> Do I wait for it to finish? Do I let it do its own thing?
- So as a response, it blows everything up because we weren't explicit

### Mutex Locks

Threads may share variables

One of these is `stdout`

This means that many threads running in parallel may write to `stdout` at the same time

```
If thread 1 tries to write "foo",
and thread 2 tries to write "bar",
and both happen to try and write at the same time,
We may not get "foobar",
But instead "fboaor" in stdout
```

To prevent this from happening, we can use `std::mutex()`:

```cpp title="main-thread.cpp"
std::mutex m;

void safePrint(const std::string& s){
    m.lock();       // lock all other threads from running the following 
    fmt::print(s);
    m.unlock();     // unlock
}

std::thread(safePrint, "foo").detach();
std::thread(safePrint, "bar").detach();

// stdout: "foobar" or "barfoo" depending on which runs first
```

- When a thread acquires a `.lock()` on a `std::mutex`, it blocks other threads from acquiring that mutex 
    - The other threads will have to wait at the same `.lock()` call until the mutex is unlocked by its owning thread
    - As a result, code enclosed by `.lock()` and `.unlock()` is held off from running!

But what if something goes wrong before we get to `.unlock()`? 

We're f**ked!

Unless we use `std::lock_guard`:

```cpp title="main-thread.cpp"
std::mutex m;

void safePrint(const std::string& s){
    std::lock_guard<std::mutex> lock(m); // using this instead!
    fmt::print(s);
}

std::thread(safePrint, "foo").detach();
std::thread(safePrint, "bar").detach();

// stdout: same effect; "foobar" or "barfoo"
```

- This is a safer version of the lock unlock pattern
- It automatically unlocks mutex when it goes out of scope 
    - If we early return, except, etc. the lock is still released
    - **BUT if something hard-crashes in the mutex owner thread, we're f\*\*ked; it'll never get released**
