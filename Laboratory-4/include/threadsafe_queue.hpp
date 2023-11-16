#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template<typename T>
class threadsafe_queue
{
  private:
      mutable std::mutex _m;  
      std::queue<T> data_queue;
      std::condition_variable _cv;

  public:
    threadsafe_queue() {}

    threadsafe_queue(const threadsafe_queue& other)
    {
	 std::lock_guard<std::mutex> lk(other.m);
     data_queue = other.data_queue;
    }

    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    void push(T new_value)
    {
	 std::lock_guard<std::mutex> lock(_m);
     data_queue.push(new_value);
     _cv.notify_one();
    }

    // if the queue is empty, it returns false, otherwise
    // it writes the front of the queue in 'value' and returns true
    bool try_pop(T& value)
    {
    std::unique_lock<std::mutex> lock(_m);
     if (data_queue.empty())
     return false;
        value = data_queue.front();
        data_queue.pop();
     return true;

    }

    void wait_and_pop(T& value)
    {
	 std::unique_lock<std::mutex> lock(_m);
     _cv.wait(lock,[this]{return !data_queue.empty();});
     value=data_queue.front();
     data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
	std::unique_lock<std::mutex> lk(_m);
    _cv.wait(lk,[this]{return !data_queue.empty();});
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
    }

    bool empty() const
    {
	 std::lock_guard<std::mutex> lock(_m);
     return data_queue.empty();
    }
};
