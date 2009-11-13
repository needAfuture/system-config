
/*++

Copyright (c) 1990-1999 Microsoft Corporation, All Rights Reserved

Module Name:

    compui.c

++*/


#include <windows.h>
#include <immdev.h>
#include "imedefs.h"
#include <regstr.h>
#include "imewnd.h"

BOOL PASCAL FitInLazyOperation(	// fit in lazy operation or not
								  LPPOINT lpptOrg, LPPOINT lpptNearCaret,	// the suggested near caret position
								  LPRECT lprcInputRect, u32 uEsc)
{
	return false;
	POINT ptDelta, ptTol;
	RECT rcUIRect, rcInterRect;

	ptDelta.x = lpptOrg->x - lpptNearCaret->x;

	ptDelta.x = (ptDelta.x >= 0) ? ptDelta.x : -ptDelta.x;

	ptTol.x = sImeG.iParaTol * ncUIEsc[uEsc].iParaFacX +
		sImeG.iPerpTol * ncUIEsc[uEsc].iPerpFacX;

	ptTol.x = (ptTol.x >= 0) ? ptTol.x : -ptTol.x;

	if (ptDelta.x > ptTol.x) {
		return FALSE;
	}

	ptDelta.y = lpptOrg->y - lpptNearCaret->y;

	ptDelta.y = (ptDelta.y >= 0) ? ptDelta.y : -ptDelta.y;

	ptTol.y = sImeG.iParaTol * ncUIEsc[uEsc].iParaFacY +
		sImeG.iPerpTol * ncUIEsc[uEsc].iPerpFacY;

	ptTol.y = (ptTol.y >= 0) ? ptTol.y : -ptTol.y;

	if (ptDelta.y > ptTol.y) {
		return FALSE;
	}
	// build up the UI rectangle (composition window)
	rcUIRect.left = lpptOrg->x;
	rcUIRect.top = lpptOrg->y;
	rcUIRect.right = rcUIRect.left + lpImeL->xCompWi;
	rcUIRect.bottom = rcUIRect.top + lpImeL->yCompHi;

	if (IntersectRect(&rcInterRect, &rcUIRect, lprcInputRect)) {
		return FALSE;
	}

	return (TRUE);
}

void PASCAL GetNearCaretPosition(LPPOINT lpptFont,
								 u32 uEsc,
								 u32 uRot,
								 LPPOINT lpptCaret,
								 LPPOINT lpptNearCaret, 
								 BOOL fFlags)
{
	LONG lFontSize;
	LONG xWidthUI, yHeightUI, xBorder, yBorder;
	RECT rcWorkArea;

	if ((uEsc + uRot) & 0x0001) {
		lFontSize = lpptFont->x;
	} else {
		lFontSize = lpptFont->y;
	}

	xWidthUI = lpImeL->xCompWi;
	yHeightUI = lpImeL->yCompHi;
	xBorder = lpImeL->cxCompBorder;
	yBorder = lpImeL->cyCompBorder;

	if (fFlags & NEAR_CARET_FIRST_TIME) {
		lpptNearCaret->x = lpptCaret->x +
			lFontSize * ncUIEsc[uEsc].iLogFontFacX +
			sImeG.iPara * ncUIEsc[uEsc].iParaFacX +
			sImeG.iPerp * ncUIEsc[uEsc].iPerpFacX;

		if (ptInputEsc[uEsc].x >= 0) {
			lpptNearCaret->x += xBorder * 2;
		} else {
			lpptNearCaret->x -= xWidthUI - xBorder * 2;
		}

		lpptNearCaret->y = lpptCaret->y +
			lFontSize * ncUIEsc[uEsc].iLogFontFacY +
			sImeG.iPara * ncUIEsc[uEsc].iParaFacY +
			sImeG.iPerp * ncUIEsc[uEsc].iPerpFacY;

		if (ptInputEsc[uEsc].y >= 0) {
			lpptNearCaret->y += yBorder * 2;
		} else {
			lpptNearCaret->y -= yHeightUI - yBorder * 2;
		}
	} else {
		lpptNearCaret->x = lpptCaret->x +
			lFontSize * ncAltUIEsc[uEsc].iLogFontFacX +
			sImeG.iPara * ncAltUIEsc[uEsc].iParaFacX +
			sImeG.iPerp * ncAltUIEsc[uEsc].iPerpFacX;

		if (ptAltInputEsc[uEsc].x >= 0) {
			lpptNearCaret->x += xBorder * 2;
		} else {
			lpptNearCaret->x -= xWidthUI - xBorder * 2;
		}

		lpptNearCaret->y = lpptCaret->y +
			lFontSize * ncAltUIEsc[uEsc].iLogFontFacY +
			sImeG.iPara * ncAltUIEsc[uEsc].iParaFacY +
			sImeG.iPerp * ncAltUIEsc[uEsc].iPerpFacY;

		if (ptAltInputEsc[uEsc].y >= 0) {
			lpptNearCaret->y += yBorder * 2;
		} else {
			lpptNearCaret->y -= yHeightUI - yBorder * 2;
		}
	}

	rcWorkArea = get_wa_rect();

	if (lpptNearCaret->x < rcWorkArea.left) {
		lpptNearCaret->x = rcWorkArea.left;
	} else if (lpptNearCaret->x + xWidthUI > rcWorkArea.right) {
		lpptNearCaret->x = rcWorkArea.right - xWidthUI;
	}

	if (lpptNearCaret->y < rcWorkArea.top) {
		lpptNearCaret->y = rcWorkArea.top;
	} else if (lpptNearCaret->y + yHeightUI > rcWorkArea.bottom) {
		lpptNearCaret->y = rcWorkArea.bottom - yHeightUI;
	}

	return;
}

BOOL PASCAL AdjustCompPosition(
	input_context& ic, 
	LPPOINT lpptOrg, 
	LPPOINT lpptNew)
{
	POINT ptNearCaret, ptOldNearCaret;
	u32 uEsc, uRot;
	RECT rcUIRect, rcInputRect, rcInterRect;
	POINT ptFont;

	// we need to adjust according to font attribute
	if (ic->lfFont.A.lfWidth > 0) {
		ptFont.x = ic->lfFont.A.lfWidth * 2;
	} else if (ic->lfFont.A.lfWidth < 0) {
		ptFont.x = -ic->lfFont.A.lfWidth * 2;
	} else if (ic->lfFont.A.lfHeight > 0) {
		ptFont.x = ic->lfFont.A.lfHeight;
	} else if (ic->lfFont.A.lfHeight < 0) {
		ptFont.x = -ic->lfFont.A.lfHeight;
	} else {
		ptFont.x = lpImeL->yCompHi;
	}

	if (ic->lfFont.A.lfHeight > 0) {
		ptFont.y = ic->lfFont.A.lfHeight;
	} else if (ic->lfFont.A.lfHeight < 0) {
		ptFont.y = -ic->lfFont.A.lfHeight;
	} else {
		ptFont.y = ptFont.x;
	}

	// if the input char is too big, we don't need to consider so much
	if (ptFont.x > lpImeL->yCompHi * 8) {
		ptFont.x = lpImeL->yCompHi * 8;
	}
	if (ptFont.y > lpImeL->yCompHi * 8) {
		ptFont.y = lpImeL->yCompHi * 8;
	}

	if (ptFont.x < sImeG.xChiCharWi) {
		ptFont.x = sImeG.xChiCharWi;
	}

	if (ptFont.y < sImeG.yChiCharHi) {
		ptFont.y = sImeG.yChiCharHi;
	}
	// -450 to 450 index 0
	// 450 to 1350 index 1
	// 1350 to 2250 index 2
	// 2250 to 3150 index 3
	uEsc = (u32) ((ic->lfFont.A.lfEscapement + 450) / 900 % 4);
	uRot = (u32) ((ic->lfFont.A.lfOrientation + 450) / 900 % 4);

	// decide the input rectangle
	rcInputRect.left = lpptNew->x;
	rcInputRect.top = lpptNew->y;

	// build up an input rectangle from escapemment
	rcInputRect.right = rcInputRect.left + ptFont.x * ptInputEsc[uEsc].x;
	rcInputRect.bottom = rcInputRect.top + ptFont.y * ptInputEsc[uEsc].y;

	// be a normal rectangle, not a negative rectangle
	if (rcInputRect.left > rcInputRect.right) {
		LONG tmp;

		tmp = rcInputRect.left;
		rcInputRect.left = rcInputRect.right;
		rcInputRect.right = tmp;
	}

	if (rcInputRect.top > rcInputRect.bottom) {
		LONG tmp;

		tmp = rcInputRect.top;
		rcInputRect.top = rcInputRect.bottom;
		rcInputRect.bottom = tmp;
	}

	GetNearCaretPosition(&ptFont, uEsc, uRot, lpptNew, &ptNearCaret,
						 NEAR_CARET_FIRST_TIME);

	// 1st, use the adjust point
	// build up the new suggest UI rectangle (composition window)
	rcUIRect.left = ptNearCaret.x;
	rcUIRect.top = ptNearCaret.y;
	rcUIRect.right = rcUIRect.left + lpImeL->xCompWi;
	rcUIRect.bottom = rcUIRect.top + lpImeL->yCompHi;

	ptOldNearCaret = ptNearCaret;

	// OK, no intersect between the near caret position and input char
	if (IntersectRect(&rcInterRect, &rcUIRect, &rcInputRect)) {
	} else
		if (FitInLazyOperation(lpptOrg, &ptNearCaret, &rcInputRect, uEsc))
	{
		// happy ending!!!, don't change position
		return FALSE;
	} else {
		*lpptOrg = ptNearCaret;

		// happy ending!!
		return (TRUE);
	}

	// unhappy case
	GetNearCaretPosition(&ptFont, uEsc, uRot, lpptNew, &ptNearCaret, 0);

	// build up the new suggest UI rectangle (composition window)
	rcUIRect.left = ptNearCaret.x;
	rcUIRect.top = ptNearCaret.y;
	rcUIRect.right = rcUIRect.left + lpImeL->xCompWi;
	rcUIRect.bottom = rcUIRect.top + lpImeL->yCompHi;

	// OK, no intersect between the adjust position and input char
	if (IntersectRect(&rcInterRect, &rcUIRect, &rcInputRect)) {
	} else
		if (FitInLazyOperation(lpptOrg, &ptNearCaret, &rcInputRect, uEsc))
	{
		return FALSE;
	} else {
		*lpptOrg = ptNearCaret;

		return (TRUE);
	}

	*lpptOrg = ptOldNearCaret;

	return (TRUE);
}

void PASCAL SetCompPosition(input_context& ic)
{
	POINT ptWnd;
	BOOL fChange = FALSE;

	ptWnd.x = 0;
	ptWnd.y = 0;

	ClientToScreen(g_hCompWnd, &ptWnd);

	if (1) {
		POINT ptNew;			// new position of UI

		ptNew.x = ic->cfCompForm.ptCurrentPos.x;
		ptNew.y = ic->cfCompForm.ptCurrentPos.y;
		ClientToScreen((HWND) ic->hWnd, &ptNew);
		fChange = AdjustCompPosition(ic, &ptWnd, &ptNew);
	} else {
		//fixme
		BHJDEBUG(" fixme");
	}

	if (!fChange) {
		return;
	}
	SetWindowPos(g_hCompWnd, NULL,
				 ptWnd.x, ptWnd.y,
				 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);

	return;
}

void PASCAL MoveDefaultCompPosition(HWND hUIWnd)
{
	if (!g_hCompWnd) {
		return;
	}

	HIMC hIMC = (HIMC) GetWindowLongPtr(hUIWnd, IMMGWLP_IMC);



	input_context ic(hIMC);
	if (!ic) {
		return;
	}

	SetCompPosition(ic);
	return;
}

void PASCAL ShowComp(int nShowCmd)
{
	ShowWindow(g_hCompWnd, nShowCmd);
	return;
}

void PASCAL StartComp(HWND hUIWnd)
{
	EnterLeaveDebug(); 
	HIMC hIMC;
	

	hIMC = (HIMC) GetWindowLongPtr(hUIWnd, IMMGWLP_IMC);

	input_context ic(hIMC);
	if (!ic) {
		return;
	}

	if (!g_hCompWnd) {
		g_hCompWnd = CreateWindowEx(0, szCompClassName, NULL, WS_POPUP | WS_DISABLED,
								  0, 0, 400, 60, hUIWnd,
								  (HMENU) NULL, hInst, NULL);
	}

	SetCompPosition(ic);
	ShowComp(SW_SHOWNOACTIVATE);

	return;
}

void PASCAL EndComp()
{
	ShowComp(SW_HIDE);

	return;
}

static void DestroyCompWindow()
{
	g_hCompWnd = (HWND) NULL;
	return;
}

void PASCAL PaintCompWindow(HDC hDC)
{
	EnterLeaveDebug(); 
	BHJDEBUG(" g_comp_str is %s", g_comp_str.c_str());

	RECT rcWnd;
	GetClientRect(g_hCompWnd, &rcWnd);

	Rectangle(hDC, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);

	if (g_comp_str.size()) {
		wstring wstr = to_wstring(g_comp_str);
		ExtTextOut(hDC, 10, 1, 0, 0, wstr.c_str(), wstr.size(), NULL);
		BHJDEBUG(" g_comp_str size is %d, wstr size is %d", g_comp_str.size(), wstr.size());
	} 
	return;
}

LRESULT CALLBACK CompWndProc(HWND hWnd, u32 uMsg, WPARAM wParam, LPARAM lParam)
{

	//BHJDEBUG("received msg %s", msg_name(uMsg));

	if (!g_hCompWnd) {
		g_hCompWnd = hWnd;
	} else if (g_hCompWnd != hWnd) {
		BHJDEBUG(" Error: CompWndProc hWnd %x is not g_hCompWnd %x", hWnd, g_hCompWnd);	
		exit(-1);
	}
	
	switch (uMsg) {
	case WM_DESTROY:
		DestroyCompWindow();
		break;
	case WM_SETCURSOR:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_IME_NOTIFY:
		BHJDEBUG(" wm_ime_notify wp %x, lp %x", wParam, lParam);
		// must not delete this case, because DefWindowProc will hang the IME
		break;
	case WM_PAINT:
		{
			HDC hDC;
			PAINTSTRUCT ps;

			hDC = BeginPaint(g_hCompWnd, &ps);
			PaintCompWindow(hDC);
			EndPaint(g_hCompWnd, &ps);
		}
		break;
	case WM_MOUSEACTIVATE:
		return (MA_NOACTIVATE);
	default:
		//BHJDEBUG(" msg %s not handled", msg_name(uMsg));
		return DefWindowProc(g_hCompWnd, uMsg, wParam, lParam);
	}
	return (0L);
}
