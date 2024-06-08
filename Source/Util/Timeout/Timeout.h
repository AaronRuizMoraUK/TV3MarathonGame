#ifndef TIMEOUT_H
#define TIMEOUT_H

/**
* Time out utilitiy
*/

class Timeout {
protected:
	float timeToElapse;
	float elapsedTime;

public:
	Timeout();
	Timeout(float pTimeToElapse);
	virtual ~Timeout();

	void init(float pTimeToElapse);
	inline void update(float deltaTime) { elapsedTime += deltaTime; }
	inline bool isFinished() const { return elapsedTime >= timeToElapse; }
	inline void restart() { elapsedTime = 0.0f; }

	inline float getElapsedTime() const { return elapsedTime; }
	inline float getTotalTime() const { return timeToElapse; }
};

#endif // TIMEOUT_H
