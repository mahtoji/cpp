/*
 * 1. Event Handlers
 * 2. Event flag - set has occured, reset has not occured
 * 3. Queue all event handlers, to an event
 * 4. On setting event, call each event handler in queue
 * 5. On queuing handler, call it, if event alrady occured
 */

class EventHandler {
	map<string, bool> evFlag;
	map<string, vector<function<void()>>> handlerVec;
	mutex m;
	void RegisterEventHandler(string event, function<void()> handler) {
		bool runIt = false;
		{
			lock_guard<mutex> lg(m);
			if(handlerVec.count(event) == 0) {
				evFlag[event] = false;
			}
			handlerVec[event].push_back(handler);
			runIt = evFlag[event];
		}
		if (runIt == true) handler();
	}
	void PostEvent(string event) {
		vector<function<void()>> hv;
		{
			lock_guard<mutex> lg(m);
			evFlag[event] = true;
			hv = handlerVec[event];
		}
		for (auto h : hv) {
			h();
		}
	}
};
