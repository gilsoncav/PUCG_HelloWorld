#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Prototypes de funcoes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
// Variaveis globais
HINSTANCE hinst;		// Handle para a instancia da app
HDC hdc;				// Handle para a o Device Context
TCHAR ch = ' ';			// Character entered
RECT rect;				// struct para um Retangulo
PAINTSTRUCT ps;			// usado no WM_PAINT
bool vkKeys[256];

// Constantes
const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World"; // Titulo da barra de texto da janela
const int WINDOW_WIDTH = 400;		// Largura da janela
const int WINDOW_HEIGHT = 400;		// Altura da janela


/************************************************************************
 * Funcao auxiliar para verificar se ha uma outra instancia da aplicacao
 * rodando
 * 
 * Gilson Cavalcanti - gilson@blackmuppet.com
 ************************************************************************/
bool OutraInstancia() {
	HANDLE ourMutex;
	// Tenta criar um Mutex com uma string unica
	ourMutex = CreateMutex(NULL, true, "pucg_string_unica_mutex_5647389-7474749");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return true;    // Ja existe uma outra instancia
	return false;		// Somos a unica instancia
	
}

/************************************************************************
 * Ponto inicial para uma aplicacao Windows
 * Os parametros sao:
 * hInstance - aponta a instancia da aplicacao
 * hPrevInstance - sempre NULL parametro ja obsoleto
 * lpCmdLine - ponteiro para uma string null-terminated contendo os parametros de linha de comando
 * nCmdShow - determina como a janela deve ser exibida
 * 
 * Gilson Cavalcanti - gilson@blackmuppet.com
 ************************************************************************/
int WINAPI WinMain( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow) {
	MSG msg;

	// Verifica se ja existe uma outra instancia desta aplicacao
	if (OutraInstancia())
		return false;

	// Cria a janela 
	if (!CreateMainWindow(hInstance, nCmdShow))
		return false;

	// Main message loop
	int done = 0;
	while (!done) {
		// Faz um teste sem bloquear a execucao para escutar mensagens do Windows
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// Se ouviu uma mensagem de quit
			if (msg.message == WM_QUIT)
				done = 1;
			// Decodifica e passa mensagens para o WinProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

/************************************************************************
 * Funcao de Window event callback     
 * 
 * Gilson Cavalcanti - gilson@blackmuppet.com
 ************************************************************************/
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	short nVirtKey;			// Codigo da Virtual key
	const short SHIFTED = (short) 0x8000;

	switch (msg) {
		case WM_DESTROY:
			// Dizer ao Windows para matar este app
			PostQuitMessage(0);
			return 0;
		case WM_CHAR:
			switch (wParam) {
				case 0x08:			// Backspace
				case 0x09:			// Tab
				case 0x0A:			// Linefeed
				case 0x0D:			// Carriage return
				case 0x1B:			// Escape
					MessageBeep((UINT) -1);		// Faz um som, mas n�o mostra nada
					return 0;
				default:		// Caso seja um caractere mostravel (nao invisivel)
					ch = (TCHAR) wParam;		// Pega o caractere
					InvalidateRect(hwnd, NULL, TRUE);  // Force WM_PAINT
					return 0;
			}
		case WM_PAINT:   // A janela precisa de redesenhar
			hdc = BeginPaint(hwnd, &ps);	// Pega o Handle do Device Context
			GetClientRect(hwnd, &rect);		// Pega o retangulo equivalente a janela
			// Mostrando o carctere digitado
			TextOut(hdc, rect.right/2, rect.bottom/2, &ch, 1);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:							// Uma tecla foi pressionada
			vkKeys[wParam] = true;
			switch (wParam) {
				case VK_SHIFT:
					nVirtKey = GetKeyState(VK_LSHIFT);	// Ganha o status de um left shift
					if (nVirtKey & SHIFTED)
						vkKeys[VK_LSHIFT] = true;
				
					nVirtKey = GetKeyState(VK_RSHIFT);
					if (nVirtKey & SHIFTED)
						vkKeys[VK_RSHIFT] = true;
					break;
				case VK_CONTROL:
					nVirtKey = GetKeyState(VK_LCONTROL);
					if (nVirtKey & SHIFTED)
						vkKeys[VK_LCONTROL] = true;
					nVirtKey = GetKeyState(VK_RCONTROL);
					if (nVirtKey & SHIFTED)
						vkKeys[VK_RCONTROL] = true;
					break;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
			break;
		case WM_KEYUP:								// A tecla foi solta
			vkKeys[wParam] = false;
			switch (wParam) {
				case VK_SHIFT:
					nVirtKey = GetKeyState(VK_LSHIFT);
					if ((nVirtKey & SHIFTED) == 0)
						vkKeys[VK_LSHIFT] = false;
					nVirtKey = GetKeyState(VK_RSHIFT);
					if ((nVirtKey & SHIFTED) == 0)
						vkKeys[VK_RSHIFT] = false;
					break;
				case VK_CONTROL:
					nVirtKey = GetKeyState(VK_LCONTROL);
					if ((nVirtKey & SHIFTED) == 0)
						vkKeys[VK_LCONTROL] = false;
					nVirtKey = GetKeyState(VK_RCONTROL);
					if ((nVirtKey & SHIFTED) == 0)
						vkKeys[VK_RCONTROL] = false;
					break;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

/************************************************************************
 * Cria a janela principal     
 * 
 * Gilson Cavalcanti - gilson@blackmuppet.com
 ************************************************************************/
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASSEX wcx;
	HWND hwnd;

	// Preenche a structure da window class com parametros
	wcx.cbSize = sizeof(wcx);				// tamanho da structure
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// Redesenha a janela se o tamanho mudar
	wcx.lpfnWndProc = WinProc;				// Aponta para o metodo que vai servir como window event callback
	wcx.cbClsExtra = 0;						// Sem memoria extra de classe
	wcx.cbWndExtra = 0;						// Sem memoria extra de janela
	wcx.hInstance = hInstance;				// Apontador para a instancia
	wcx.hIcon = NULL;						// Icone grande do app
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // seta predefinida
	wcx.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH); // Background brush
	wcx.lpszMenuName = NULL;				// Nome do recurso de menu
	wcx.lpszClassName = CLASS_NAME;			// Nome da classe de janela
	wcx.hIconSm = NULL;						// Icone pequeno do app

	// Registra a classe de janela
	// RegisterClassEx retorna 0 em caso de erro
	if (RegisterClassEx(&wcx) == 0) // Se deu erro
		return false;
	// Cria janela
	hwnd = CreateWindow(
		CLASS_NAME,					// Nome da classe de janela
		APP_TITLE,					// Texto da barra de titulo
		WS_OVERLAPPEDWINDOW,		// Estilo de janela
		CW_USEDEFAULT,				// Posicao horizontal padrao da janela 
		CW_USEDEFAULT,				// Posicao vertical padrao da janela
		WINDOW_WIDTH,				// Largura da janela
		WINDOW_HEIGHT,				// Altura da janela
		(HWND) NULL,				// Nao ha janela pai
		(HMENU) NULL,				// Nao ha menu
		hInstance,					// Handle para a instancia da aplicacao
		(LPVOID) NULL);				// Sem parametros de janela
	// Se houve algum erro criando a janela
	if(!hwnd)
		return false;
	// Exibir a janela
	ShowWindow(hwnd, nCmdShow);
	// Envia uma mensagem WM_PAINT para o window procedure
	UpdateWindow(hwnd);
	return true;
}

