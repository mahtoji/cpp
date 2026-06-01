class SpinLock {
	atomic_flag flag = ATOMIC_FLAG_INIT;

	void lock() {
		while (flag.test_and_set(memory_order_acquire)) { ; }
	}

	void unlock() {
		flag.clear(memory_order_release);
	}
};


class Mutex {
	SpinLock s;
	bool flag = false; //are we locked or not

	void lock() {
		bool need_to_wait = true;
		while (need_to_wait == true) {
			s.lock();
			if (flag == false) {
				need_to_wait = false;
				flag = true;
			}
			s.unlock();
			this_thread::sleep_for(1ms);
		}
	}
	void unlock() {
		s.lock();
		flag = false;
		s.unlock();
	}
};
