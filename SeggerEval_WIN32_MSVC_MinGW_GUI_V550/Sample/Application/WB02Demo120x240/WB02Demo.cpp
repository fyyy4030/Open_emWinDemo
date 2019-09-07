#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#else
//#include <windows.h>
#endif
#include "WB02Demo.h"
#include "WindowDLG.c"


static void _cbWin(WM_MESSAGE* pMsg) {
	WM_MOTION_INFO *pInfo;
	WM_HWIN hWin;
	WINDOW_DATA* pData;
	GUI_PID_STATE State;
	int i, x0, y0, Index, NumItems;

	hWin = pMsg->hWin;
	NumItems = 8;
	//NumItems = GUI_COUNTOF(_bgimgs);
	WM_GetUserData(hWin, &pData, sizeof(WINDOW_DATA*));

	if (pData) {
		if (pData->hTimer == 0) {
			pData->hTimer = WM_CreateTimer(hWin, 0, 0, 0);
			pData->Job = JOB_ANIM_IN_DROP;
			//_DrawIndicators(pData);
		}
	}

	switch (pMsg->MsgId) {
	case WM_PAINT:
		//GUI_Clear();
#if 0
		GUI_DrawBitmap(_bgimgs[0], 0, 0);
#else
		for (i = 0; i <= NumItems; i++) {
			Index = (i == NumItems) ? 0 : i;

#ifndef EN_ROLL_AXIS_Y
			x0 = pData->xPos - i * pData->xSize;
			if ((x0 > -(int)pData->xSize) && (x0 < (int)pData->xSize)) {
				GUI_DrawBitmap(_bgimgs[Index], x0, 0);
		}
#else
			y0 = pData->yPos - i * pData->ySize;
			if ((y0 > -(int)pData->ySize) && (y0 < (int)pData->ySize)) {
				GUI_DrawBitmap(_bgimgs[Index], 0, y0);
			}
#endif
		}
#endif
		GUI_Log("WM_PAINT\n");
		break;
	case WM_TIMER:
		GUI_Log("WM_TIMER\n");
		//_OnTimer(pData);
		break;
	case WM_MOTION:
		//pData->TimeLastTouch = GUI_GetTime();
		GUI_Log("WM_MOTION\n");

		GUI_PID_GetCurrentState(&State);
		if (State.Pressed) {
#ifndef EN_ROLL_AXIS_Y
			if (State.y != 0) {
#else
			if (State.x != 0) {
#endif
				if (pData->hAnimBackground) {
					GUI_ANIM_Stop(pData->hAnimBackground);
					GUI_ANIM_Delete(pData->hAnimBackground);
					pData->hAnimBackground = 0;
				}
			}
		}

		pData->TimeLastTouch = GUI_GetTime();

		pInfo = (WM_MOTION_INFO*)pMsg->Data.p;
		switch (pInfo->Cmd) {
		case WM_MOTION_INIT:
			GUI_Log("WM_MOTION_INIT\n");
			pInfo->Flags = WM_MOTION_MANAGE_BY_WINDOW;
#ifndef EN_ROLL_AXIS_Y
			pInfo->SnapX = pData->xSize;
#else
			pInfo->SnapY = pData->ySize;
#endif
			pInfo->Period = 300;
			break;
		case WM_MOTION_MOVE:
			GUI_Log("WM_MOTION_MOVE\n");
#ifndef EN_ROLL_AXIS_Y
			pData->xPos += pInfo->dx;
			if (pData->xPos >= (int)(NumItems * pData->xSize)) {
				pData->xPos -= NumItems * pData->xSize;
			}
			if (pData->xPos < 0) {
				pData->xPos += NumItems * pData->xSize;
			}
			//
			// Calculate distance
			//
			pData->Diff = pData->xPos - pData->IndexCity * pData->xSize;
			if ((pData->IndexCity == 0) && (pData->Diff > pData->xSize)) {
				pData->Diff -= pData->xSize * NumItems;
			}
#else
			pData->yPos += pInfo->dy;
			if (pData->yPos >= (int)(NumItems * pData->ySize)) {
				pData->yPos -= NumItems * pData->ySize;
			}
			if (pData->yPos < 0) {
				pData->yPos += NumItems * pData->ySize;
			}
			//
			// Calculate distance
			//
			pData->Diff = pData->yPos - pData->IndexCity * pData->ySize;
			if ((pData->IndexCity == 0) && (pData->Diff > pData->ySize)) {
				pData->Diff -= pData->ySize * NumItems;
			}
#endif
			pData->Diff = (pData->Diff > 0) ? pData->Diff : -pData->Diff;
			WM_InvalidateWindow(pData->hText);
			if (pInfo->FinalMove) {
				//
				// After last move timer method should show forecast
				//
				pData->HasStopped = 1;
			}
			else {

			}
			WM_Invalidate(hWin);
			break;
		case WM_MOTION_GETPOS:
			GUI_Log("WM_MOTION_GETPOS\n");
#ifndef EN_ROLL_AXIS_Y
			pInfo->xPos = pData->xPos;
#else
			pInfo->yPos = pData->yPos;
#endif
			break;
		}

		break;
	case WM_INIT_DIALOG:
		GUI_Log("WM_INIT_DIALOG\n");
		break;
	default:
		//GUI_Log("Hello defalut.\n");
		WM_DefaultProc(pMsg);
		break;
	}
}

static void _ClearScreen(void) {
	GUI_Clear();
}

static void _InitData(WINDOW_DATA* pData) {
	int i, NumItems, xSizeScreen, xSize;

	NumItems = GUI_COUNTOF(pData->aPara);
	xSizeScreen = LCD_GetXSize();
	xSize = xSizeScreen / NumItems;
	for (i = 0; i < NumItems; i++) {
		pData->aPara[i].xSizeScreen = xSizeScreen;
		pData->aPara[i].xSize = xSize;
		pData->aPara[i].xPos = (xSize * i) + xSize / 2;
		pData->aPara[i].pData = pData;
		pData->aPara[i].Index = i;
	}
}

void MainTask(void) {
	WINDOW_DATA* pData;
	static WINDOW_DATA Data;

	pData = &Data;

	GUI_Init();
#if 0
	if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
		GUI_ErrorOut("Not enough memory available.");
		return;
	}
#endif

#if 0
	//LCD_X_Config();
	GUI_SetColor(COLOR_LIGHTBROWN);
	GUI_DispStringAt("WB02", 50, 50); // Disp text
#else
	WM_HWIN hDlg1;
	WM_MULTIBUF_Enable(1);
	_ClearScreen();
	WM_MOTION_Enable(1);

	pData->xSize = LCD_GetXSize();
	pData->ySize = LCD_GetYSize();
	pData->TimeLastTouch = GUI_GetTime();
	
	/* Sets the new size of a window. */
	//WM_SetSize(WM_HBKWIN, pData->xSize, pData->ySize);

	/* Creates a window of a specified size at a specified location. */
	//pData->hWin = WM_CreateWindow(0, 0, Data.xSize, Data.ySize, WM_CF_SHOW, _cbWin, sizeof(WINDOW_DATA*));
	
	/* Creates a window as a child window. */
#ifndef EN_ROLL_AXIS_Y
	pData->hWin = WM_CreateWindowAsChild(0, 0, Data.xSize, Data.ySize, WM_HBKWIN, WM_CF_MOTION_X | WM_CF_SHOW, _cbWin, sizeof(WINDOW_DATA*));
#else
	pData->hWin = WM_CreateWindowAsChild(0, 0, Data.xSize, Data.ySize, WM_HBKWIN, WM_CF_MOTION_Y | WM_CF_SHOW, _cbWin, sizeof(WINDOW_DATA*));
#endif
	//GUI_SelectLayer(1);
#if 0
	hDlg1 = CreateWindow();
#endif
	
	_InitData(pData);

	WM_SetUserData(pData->hWin, &pData, sizeof(WINDOW_DATA*));

	GUI_CURSOR_Show();

	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("WB02", 50, 50); // Disp text
#endif
	while (1) {
		GUI_Delay(100);
	}
}
