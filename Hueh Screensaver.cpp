#include <windows.h>
#include <scrnsave.h>
#include <time.h>

UINT uTimerID; //id for the timer

const int huehAmount = 146;
HBITMAP huehs[huehAmount];

const int gridW = 20, gridH = 15; //width and height of the hueh grid
int huehGrid[gridW][gridH];

const int huehW = 104, huehH = 80; //size of a hueh
int huehGW, huehGH; //size of a hueh in the grid

const int changeSpeed = 7;
int changeCounter = 0;

int buildupSpeed = 1;
int buildupCounter = 0;
int buildupAmount = 0;

bool bgCleared = false;

void drawHueh(int X, int Y, int huehID, HDC targetDC) {
	HDC dcMem = CreateCompatibleDC(targetDC);
	HGDIOBJ oldSelect = SelectObject(dcMem, huehs[huehID]);

	StretchBlt(targetDC, X * huehGW, Y * huehGH, huehGW, huehGH, dcMem, 0, 0, huehW, huehH, SRCCOPY);

	SelectObject(dcMem, oldSelect);
	DeleteDC(dcMem);
}

void clearBG(HWND hWnd) {
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	HDC hdc = GetDC(hWnd);
	HBRUSH brush = CreateSolidBrush(RGB(1, 2, 3));
	FillRect(hdc, &clientRect, brush);
	DeleteObject(brush);
	ReleaseDC(hWnd, hdc);
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			//load huehs
			for (int i = 0; i < huehAmount; i++) {
				huehs[i] = LoadBitmap((HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(101 + i));
			}

			//set hueh sizes:
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			huehGW = clientRect.right / gridW;
			huehGH = clientRect.bottom / gridH;

			//seed random
			srand(time(NULL));

			//distribute random huehs
			for (int i = 0; i < gridH; i++) {
				for (int j = 0; j < gridW; j++) {
					huehGrid[j][i] = -1;
				}
			}

			//set window transparency
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWnd, RGB(1, 2, 3), 0, LWA_COLORKEY);

			//setup timer
			uTimerID = SetTimer(hWnd, 1, 1, NULL);
			break;
		}

		case WM_ERASEBKGND: {
			clearBG(hWnd);
			break;
		}

		case WM_TIMER: {
			//clear background on startup
			if (!bgCleared) {
				bgCleared = true;
				clearBG(hWnd);
			}

			//draw huehGrid
			HDC hdc = GetDC(hWnd);

			for (int i = 0; i < gridH; i++) {
				for (int j = 0; j < gridW; j++) {
					if(huehGrid[j][i] != -1) {
						drawHueh(j, i, huehGrid[j][i], hdc);
					}
				}
			}
			ReleaseDC(hWnd, hdc);


			if (buildupAmount < gridW * gridH) { //fade huehs in
				buildupCounter++;
				if (buildupCounter > buildupSpeed) {
					buildupCounter = 0;

					huehGrid[buildupAmount % gridW][buildupAmount / gridW] = rand() % huehAmount;

					buildupAmount++;
				}
			}
			
			//change huehs
			changeCounter++;
			if (changeCounter >= changeSpeed) {
				changeCounter = 0;

				int x = rand() % gridW, y = rand() % gridH;
				if (huehGrid[x][y] != -1) {
					huehGrid[x][y] = rand() % huehAmount;
				}
			}
			
			break;
		}

		case WM_DESTROY: {
			if (uTimerID) {
				KillTimer(hWnd, uTimerID);
			}

			for (int i = 0; i < huehAmount; i++) {
				DeleteObject(huehs[i]);
			}
			break;
		}
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	return false;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst) {
	return true;
}