#ifndef _FCLASS_H_
#define _FCLASS_H_

#include "femy.h"
class FWindow{
public:
	FWindow(){
		FInitWindowClass();
	}
	~FWindow(){
		//Destory window class
		UnregisterClass(_T("femy"), this->wnex.hInstance);
	}
	void FInitWindowClass(){
		wnex.cbSize = sizeof(WNDCLASSEX);
		wnex.style = CS_HREDRAW | CS_VREDRAW;
		wnex.lpfnWndProc = NULL;
		wnex.cbClsExtra = 0;			//�����฽���ڴ�
		wnex.cbWndExtra = 0;
		wnex.hInstance = hInstance;
		wnex.hIcon = NULL;				//û��ͼ��
		wnex.hCursor = LoadCursor(NULL, IDC_ARROW);			//��� LoadCursor(_In_opt_ HINSTANCE hInstance, _In_opt_ LPCTSTR lpCursorName);
		wnex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		//������ˢ
		wnex.lpszMenuName = NULL;		//����Ҫ�˵�
		wnex.lpszClassName = _T("femy");	//�����������
	}
	void FUnRegisterWindowClass(void){
		UnregisterClass(wnex.lpszClassName, this->hInstance);
	}
	void FRegisterWindowClass(void){
		if (!RegisterClassEx(&wnex))
			MessageBox(NULL, L"error", L"title", MB_OK);
	}
	void FCreateWindow(void){
		//::CreateWindow();
		this->hWnd = CreateWindow(_T("femy"), _T("femy"), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, this->wnex.hInstance, NULL);
		if (this->hWnd == NULL)
			MessageBox(NULL, L"Create Windows Error", L"title", MB_OK);
	}
	void FShowWindow(_In_ int nCmdShow){
		ShowWindow(this->hWnd, nCmdShow);
		UpdateWindow(this->hWnd);
	}
	void FSetClassName(LPCTSTR name){
		this->wnex.lpszClassName = name;
	}
	void FSetIcon(HICON hIcon){
		this->wnex.hIcon = hIcon;
	}
	void FSetHInstance(HINSTANCE hInstance){
		this->hInstance = hInstance;
		this->wnex.hInstance = hInstance;
	}
	void FSetWndProc(WNDPROC WndProc){
		this->wnex.lpfnWndProc = WndProc;
	}
	void FSetCursor(HCURSOR hCursor){
		this->wnex.hCursor = hCursor;
	}
	void FSetBackgroundHBrush(HBRUSH hBrush){
		this->wnex.hbrBackground = hBrush;
	}
	//���ܺ���
	void TMoveWindow(int x, int y, int width, int height){
		MoveWindow(this->hWnd, x, y, width, height, true);
	}
	void TMoveWindow(int x, int y){
		MoveWindow(this->hWnd, x, y, CW_USEDEFAULT, CW_USEDEFAULT, true);
	}


	HWND hWnd;
private:
	HINSTANCE hInstance;
	WNDCLASSEX wnex;
};

//D3D������
class DInputClass{
private:
	IDirectInput8 *m_pDirectInput;
	IDirectInputDevice8 *m_KeyboardDevice;
	char m_keyBuffer[256];						//���ڴ洢������Ϣ

	IDirectInputDevice8 *m_MouseDevice;			//����豸
	DIMOUSESTATE m_MouseState;					//����ֵ�洢�Ľṹ��
public:
	HRESULT Init(HWND hwnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	void GetInput();							//���ڻ�ȡ������Ϣ�ĺ���
	bool IsKeyDown(int iKey);
	bool IsMouseButtonDown(int button);
	float MouseDX();
	float MouseDY();
	float MouseDZ();
public:
	DInputClass(void);
	~DInputClass(void);

};
DInputClass::DInputClass(){
	m_pDirectInput = NULL;
	m_KeyboardDevice = NULL;
	ZeroMemory(m_keyBuffer, sizeof(char)* 256);
	m_MouseDevice = NULL;
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
}
HRESULT DInputClass::Init(HWND hwnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags){
	HRESULT hr;
	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL));
	//�����豸�ĳ�ʼ��
	HR(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_KeyboardDevice, NULL));
	HR(m_KeyboardDevice->SetCooperativeLevel(hwnd, keyboardCoopFlags));
	HR(m_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard));								//���ü��̵�����ģʽ
	m_KeyboardDevice->Acquire();
	m_KeyboardDevice->Poll();
	//MessageBox(NULL, TEXT("input init error!"), TEXT("Error"), MB_OK);
	//��������豸�ĳ�ʼ��
	HR(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_MouseDevice, NULL));
	HR(m_MouseDevice->SetCooperativeLevel(hwnd, mouseCoopFlags));
	HR(m_MouseDevice->SetDataFormat(&c_dfDIMouse));										//������������ģʽ
	m_MouseDevice->Acquire();
	m_MouseDevice->Poll();
	//MessageBox(NULL, TEXT("error"), TEXT("error"), MB_OK);
	return S_OK;
}
void DInputClass::GetInput(){
	HRESULT hr;
	hr = m_KeyboardDevice->GetDeviceState(sizeof(m_keyBuffer), (void**)&m_keyBuffer);
	if (hr)
	{
		m_KeyboardDevice->Acquire();
		m_KeyboardDevice->GetDeviceState(sizeof(m_keyBuffer), (LPVOID)m_keyBuffer);
	}

	hr = m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_MouseState);
	if (hr)
	{
		m_MouseDevice->Acquire();
		m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_MouseState);
	}
}
bool DInputClass::IsKeyDown(int iKey){
	if (m_keyBuffer[iKey] & 0x80)
		return true;
	else
		return false;
}
bool DInputClass::IsMouseButtonDown(int button){
	return (m_MouseState.rgbButtons[button] & 0x80) != 0;
}
float DInputClass::MouseDX(){
	return (float)m_MouseState.lX;
}
float DInputClass::MouseDY(){
	return (float)m_MouseState.lY;
}
float DInputClass::MouseDZ(){
	return (float)m_MouseState.lZ;
}
DInputClass::~DInputClass(void){
	if (m_KeyboardDevice != NULL)
		m_KeyboardDevice->Unacquire();
	if (m_MouseDevice != NULL)
		m_MouseDevice->Unacquire();
	SAFE_RELEASE(m_KeyboardDevice);
	SAFE_RELEASE(m_MouseDevice);
}
#endif