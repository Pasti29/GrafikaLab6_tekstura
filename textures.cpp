//
// Created by karol on 06.12.2021.
//

#include "textures.h"

/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
// FileName, alokuje pamięć i zwraca wskaźnik (pBits) do bufora, w którym
// umieszczone są dane.
// Ponadto udostępnia szerokość (ImWidth), wysokość (ImHeight) obrazu
// tekstury oraz dane opisujące format obrazu według specyfikacji OpenGL
// (ImComponents) i (ImFormat).
// Jest to bardzo uproszczona wersja funkcji wczytującej dane z pliku TGA.
// Działa tylko dla obrazów wykorzystujących 8, 24, or 32-bitowy kolor.
// Nie obsługuje plików w formacie TGA kodowanych z kompresją RLE.
/*************************************************************************************/


GLbyte *
textures::LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat) {


// Struktura dla nagłówka pliku TGA
#pragma pack(1)
	typedef struct {
		GLbyte idlength;
		GLbyte colormaptype;
		GLbyte datatypecode;
		unsigned short colormapstart;
		unsigned short colormaplength;
		unsigned char colormapdepth;
		unsigned short x_orgin;
		unsigned short y_orgin;
		unsigned short width;
		unsigned short height;
		GLbyte bitsperpixel;
		GLbyte descriptor;
	} TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte *pbitsperpixel = nullptr;



// Wartości domyślne zwracane w przypadku błędu
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == nullptr){
		return nullptr;
	}


// Przeczytanie nagłówka pliku
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

// Odczytanie szerokości, wysokości i głębi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32) {
		return nullptr;
	}


// Obliczenie rozmiaru bufora w pamięci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

// Alokacja pamięci dla danych obrazu
	pbitsperpixel = (GLbyte *) malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == nullptr) {
		return nullptr;
	}

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
		free(pbitsperpixel);
		return nullptr;
	}

// Ustawienie formatu OpenGL
	switch (sDepth) {
		case 3:
			*ImFormat = GL_BGR_EXT;
			*ImComponents = GL_RGB8;
			break;
		case 4:
			*ImFormat = GL_BGRA_EXT;
			*ImComponents = GL_RGBA8;
			break;
		case 1:
			*ImFormat = GL_LUMINANCE;
			*ImComponents = GL_LUMINANCE8;
			break;
		default:
			break;
	}
	fclose(pFile);
	return pbitsperpixel;
}