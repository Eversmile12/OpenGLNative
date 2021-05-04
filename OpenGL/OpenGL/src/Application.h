#ifndef _H_APPLICATION_
#define _H_APPLICATION_

class Application {
private:
	Application(const Application&);
	Application& operator=(const Application&);
public:
	inline Application(){}
	inline virtual ~Application(){}
	inline virtual void Initialize(){}
	inline virtual void Update(float inDeltaTime){}
	inline virtual void Render(float inAspectRation){}
	inline virtual void Shutdown(){}
};

#endif // !_H_APPLICATION_
