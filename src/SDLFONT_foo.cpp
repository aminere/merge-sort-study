// SDLFONT_foo class 
// Based on my D3DFONT class for Directx
// by Amine Rehioui
// http://www.aui.ma/~A.Rehioui

#include "SDLFONT_foo.h"

bool SDLFONT_foo::LoadMap(char *pFileName, int FontWidth, int FontHeight, int GridSize)
{      
  m_FontWidth = FontWidth; 
  m_FontHeight = FontHeight;
 
  printf("Loading font map %s..\n", pFileName);
  m_pFontMap = SDL_LoadBMP(pFileName);
  if(m_pFontMap == NULL) return false;
    
  //m_pFontMap->flags |= SDL_SRCCOLORKEY;
     
  return true;
}

void SDLFONT_foo::Destroy()
{
  printf("Destroying font stuff..\n");
  
  if(m_pBuffer!=NULL) { delete[] m_pBuffer; m_pBuffer = NULL; }
  
  if(m_pFontMap) { SDL_FreeSurface(m_pFontMap); m_pFontMap = NULL; }
}
    
void SDLFONT_foo::DrawText(SDL_Surface* dest, int x, int y, int color, const char *format, ...)
{
  tempx = x;
  tempy = y;
  memset(m_pBuffer, 0, MAX_FONT_BUFFER_SIZE); 
  va_list msg; 		
  va_start(msg, format);
  vsprintf(m_pBuffer, format, msg);
  va_end(msg);       
  
  _strlen = strlen(m_pBuffer);
  for(size_t current=0; current<_strlen; current++)
  {    
    if(m_pBuffer[current]=='\n') 
	{
	  tempy+=m_FontHeight;
	  tempx = x;
	  continue;
	} 

	// draw character
	m_SrcRect.y = m_FontHeight * ( ((int)m_pBuffer[current]) / 16 );
	m_SrcRect.x = (((int)m_pBuffer[current])%16) * m_FontWidth;
	m_SrcRect.w = m_FontWidth;
	m_SrcRect.h = m_FontHeight;

	m_DestRect.x = tempx;
	m_DestRect.y = tempy;
	m_DestRect.w = m_FontWidth;
	m_DestRect.h = m_FontHeight;	

	SDL_BlitSurface(m_pFontMap, &m_SrcRect, dest, &m_DestRect);
	//

    tempx += m_FontWidth-4;  
  }    
}

