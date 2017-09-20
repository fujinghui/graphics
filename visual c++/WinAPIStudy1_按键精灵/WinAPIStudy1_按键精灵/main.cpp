#include <windows.h>
int main(void){
	POINT point;
	//RegisterHotKey()
	while (true)
	{
		GetCursorPos(&point);
		//mouse_event(MOUSEEVENTF_LEFTDOWN, point.x, point.y, 0, 0);
		//mouse_event(MOUSEEVENTF_LEFTUP, point.x, point.y, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 100, 100, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 100, 100, 0, 0);
		//Sleep(1000);
		//mouse_event(MOUSEEVENTF_LEFTDOWN, 500, 500, 0, 0);
		//mouse_event(MOUSEEVENTF_LEFTUP, 500, 500, 0, 0);

		Sleep(800);
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 800, 600, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 800, 600, 0, 0);
		Sleep(800);
	}
	return 0;
}