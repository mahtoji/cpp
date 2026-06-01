class Singleton {
	private:
		mutex m;
		Singleton *ptr;
		Singelton() {}
	public:
		Singleton& operator=(Singleton &s) = delete;
		Singleton(const Singelton& s) = delete;
		Singleton* GetInstance() {
			lock_guard<mutex> lg(m);
			if (ptr == nullptr) {
				ptr = new Singleton();
			}
			return ptr;
		}
};
