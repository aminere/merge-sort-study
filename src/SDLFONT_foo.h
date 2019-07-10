// SDLFONT_foo class 
// Based on my D3DFONT class for Directx
// by Amine Rehioui
// http://www.aui.ma/~A.Rehioui

#ifndef __SDLFONT_FOO_H_
#define __SDLFONT_FOO_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "SDL.h"

#define MAX_FONT_BUFFER_SIZE 400

class SDLFONT_foo
{
public:

  SDLFONT_foo()
  { 
    m_pFontMap = NULL;
	m_pBuffer = new char[MAX_FONT_BUFFER_SIZE];	
  }
 
  bool LoadMap(char*, int, int, int); 
  void Destroy(); 
  
  void DrawText(SDL_Surface*,int, int, int, const char*, ...);
  int FontWidth() { return m_FontWidth; }
  int FontHeight() { return m_FontHeight; }

private:

  SDL_Surface *m_pFontMap;
  
  char *m_pBuffer;
  int tempx, tempy; size_t _strlen; // variables needed in DrawText()
  int m_FontWidth, m_FontHeight;
  SDL_Rect m_SrcRect, m_DestRect;
};

#endif








