#pragma once

class WindowListener
{
public:
	virtual bool tick() = 0;
	virtual void onResized(int newWidth, int newHeight) { }
	virtual void onKeyDown(int keycode) { }
};
