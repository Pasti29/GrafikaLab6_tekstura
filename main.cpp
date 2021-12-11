//*********************************************************************
//
//  PLIK ŹRÓDŁOWY:		main.cpp
//
//  OPIS:				Program do Laboratorium nr 6 rysujący na
//						ekranie oteksturowany obiekt trójkąta,
//                      ostrosłupa lub jajka.
//
//  AUTOR:				Karol Pastewski, 252798@student.edu.pl
//
//  DATA
//	MODYFIKACJI:        12.12.2021r.
//
//  PLATFORMA:			System operacyjny:  Microsoft Windows 11
//						Środowisko:         Microsoft Visual 2019
//
//  MATERIAŁY			Dokumentacja OpenGL
//	ŹRÓDŁOWE:			Dokumentacja GLUT
//						www.zsk.ict.pwr.wroc.pl
//
//  UŻYTE BIBLIOTEKI	cmath — obsługuje matematyczne wzory i stałe
//  NIESTANDARDOWE
//
//*********************************************************************

#define _USE_MATH_DEFINES

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "textures.h"

using namespace std;

using point3 = float[3];

struct VERTEXES {
	float point3[3];
	float normalized[3];
	float texture[2];
};

VERTEXES **ARRAY = nullptr; // niezainicjalizowana tablica dwuwymiarowa; patrz funkcję 'setArray()'
int N;                      // wielkość tablicy zainicjowana w funkcji 'setArray()'
static int objectMode = 1;
// 1 - wyświetlenie trójkąta;
// 2 - wyświetlenie wielościanu
// 3 - wyświetlenie jajka
static int mouseStatus = 0;
// stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty zostać lewy klawisz
// 2 - naciśnięty prawy klawisz
static boolean chosenTexture = 1;
static unsigned short displayPyramidWall = 0b0001'1111;

static float objectRotation[] = {0.0, 0.0};
static float objectRotationBeforeZoom[] = {0.0, 0.0};

static float pix2angle;     // przelicznik pikseli na stopnie

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static float delta_x = 0;        // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy

static int y_pos_old = 0;       // poprzednia pozycja kursora myszy
static float delta_y = 0;        // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy


static float viewerZ = 10.0;
static float help = 10.0;

// Parametry położenia obiektu na układzie współrzędnych
static float centerX = 0.0;
static float centerY = 0.0;
static float centerZ = 0.0;

// Parametry źródła światła
static float lightPosition[] = {0.0, 0.0, 10.0, 1.0};

// Zmienne dla obrazu tekstury
GLbyte *pBytes;
GLint ImWidth, ImHeight, ImComponents;
GLenum ImFormat;

void drawWhiteTriangles() {
	for (int i = 0; i < N - 1; i++) {
		for (int j = 0; j < N - 1; j++) {

			//!!! Ważne dla glCullFace(). Działa tylko przed i po zdefiniowaniu prymitywu. !!!
			// Czyli przed glBegin() i po glEnd().

			// Bez tego warunku tylko połowa jajka wyświetli się po dobrej stronie, a
			// połowa po wewnętrznej stronie.

			(i >= N / 2) ? glCullFace(GL_FRONT) : glCullFace(GL_BACK);

			glColor3f(1.0, 1.0, 1.0);

			glBegin(GL_TRIANGLES);

			glNormal3fv(ARRAY[i][j].normalized);
			glTexCoord2fv(ARRAY[i][j].texture);
			glVertex3fv(ARRAY[i][j].point3);

			glNormal3fv(ARRAY[i + 1][j + 1].normalized);
			glTexCoord2fv(ARRAY[i + 1][j + 1].texture);
			glVertex3fv(ARRAY[i + 1][j + 1].point3);

			glNormal3fv(ARRAY[i][j + 1].normalized);
			glTexCoord2fv(ARRAY[i][j + 1].texture);
			glVertex3fv(ARRAY[i][j + 1].point3);

			glEnd();

			if (i == N / 2 - 1) glCullFace(GL_FRONT);
			// Bez tej linijki na górze jajka pojawi się mała dziura stworzona z trójkątów,
			// widoczne są one z drugiej strony, więc trzeba obrócić CullFace

			glBegin(GL_TRIANGLES);
			glNormal3fv(ARRAY[i][j].normalized);
			glTexCoord2fv(ARRAY[i][j].texture);
			glVertex3fv(ARRAY[i][j].point3);

			glNormal3fv(ARRAY[i + 1][j].normalized);
			glTexCoord2fv(ARRAY[i + 1][j].texture);
			glVertex3fv(ARRAY[i + 1][j].point3);

			glNormal3fv(ARRAY[i + 1][j + 1].normalized);
			glTexCoord2fv(ARRAY[i + 1][j + 1].texture);
			glVertex3fv(ARRAY[i + 1][j + 1].point3);
			glEnd();
		}
	}
}

void Triangle() {
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5.0, -5.0, 0.0);

	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5.0, -5.0, 0.0);

	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0, 5.0, 0.0);

	glEnd();
	glCullFace(GL_FRONT);
	glBegin(GL_TRIANGLES);

	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-5.0, -5.0, 0.0);

	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5.0, -5.0, 0.0);

	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0, 5.0, 0.0);

	glEnd();
	glCullFace(GL_BACK);
}


void Pyramid() {
	glColor3f(1.0, 1.0, 1.0);
	if (displayPyramidWall & (0b1 << 4)) {
		glCullFace(GL_FRONT);
		glBegin(GL_POLYGON);

		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-4.0, -3.0, 4.0);

		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4.0, -3.0, 4.0);

		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(4.0, -3.0, -4.0);

		glNormal3f(0.0, -1.0, 0.0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-4.0, -3.0, -4.0);

		glEnd();
		glCullFace(GL_BACK);
	}
	glBegin(GL_TRIANGLES);
	if (displayPyramidWall & (0b1 << 3)) {
		// Pierwszy trójkąt
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-4.0, -3.0, 4.0);

		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4.0, -3.0, 4.0);

		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0, 4.0, 0.0);
	}
	if (displayPyramidWall & (0b1 << 2)) {
		//Drugi trójkąt
		glNormal3f(1.0, 0.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4.0, -3.0, 4.0);

		glNormal3f(1.0, 0.0, 0.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(4.0, -3.0, -4.0);

		glNormal3f(1.0, 0.0, 0.0);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0, 4.0, 0.0);
	}
	if (displayPyramidWall & (0b1 << 1)) {
		//Trzeci trójkąt
		glNormal3f(0.0, 0.0, -1.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(4.0, -3.0, -4.0);

		glNormal3f(0.0, 0.0, -1.0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-4.0, -3.0, -4.0);

		glNormal3f(0.0, 0.0, -1.0);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0, 4.0, 0.0);
	}
	if (displayPyramidWall & 0b1) {
		//Czwarty trójkąt
		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-4.0, -3.0, -4.0);

		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-4.0, -3.0, 4.0);

		glNormal3f(-1.0, 0.0, 0.0);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0, 4.0, 0.0);
	}
	glEnd();
}


// Funkcja obsługująca rysowanie jajka
void Egg() {
	drawWhiteTriangles();
}


void renderScene() {
	if(chosenTexture) {
		// Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
		pBytes = textures::LoadTGAImage("tekstura2.tga", &ImWidth,
		                                &ImHeight, &ImComponents, &ImFormat);

		//Zdefiniowanie tekstury 2-D
		glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight,
		             0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

		free(pBytes);
	} else {
		// Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
		pBytes = textures::LoadTGAImage("tekstura.tga", &ImWidth,
		                                &ImHeight, &ImComponents, &ImFormat);

		//Zdefiniowanie tekstury 2-D
		glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight,
		             0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

		free(pBytes);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();    // czyszczenie macierzy bieżącej

	// Obliczanie azymutu oraz elewacji dla obracanego obiektu
	if (mouseStatus == 1) {      // jeśli lewy klawisz myszy wciśnięty
		objectRotation[0] += delta_x * pix2angle;
		objectRotation[1] += delta_y * pix2angle;

		if (objectRotation[0] > 360.0) objectRotation[0] -= 360.0;
		else if (objectRotation[0] < 0) objectRotation[0] += 360.0;
		if (objectRotation[1] > 360) objectRotation[1] -= 360.0;
		else if (objectRotation[1] < 0) objectRotation[1] += 360.0;

	} else if (mouseStatus == 2) {       // jeśli prawy klawisz myszy wciśnięty
		help += delta_y / 10;
		if (help > 1) viewerZ = help;
		if (help > 20) viewerZ = help = 20;
	}

	gluLookAt(0.0, 0.0, viewerZ, centerX, centerY, centerZ, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glRotatef(objectRotation[0], 0.0, 1.0, 0.0);
	glRotatef(objectRotation[1], 1.0, 0.0, 0.0);

	if (help > 1) {
		objectRotationBeforeZoom[0] = objectRotation[0];
		objectRotationBeforeZoom[1] = objectRotation[1];
	} else if (help < 1) {

		if (objectRotationBeforeZoom[1] >= 45 && objectRotationBeforeZoom[1] <= 135) {
			glTranslatef(0.0, 1 - help, 0.0);
		} else if (objectRotationBeforeZoom[1] >= 225 && objectRotationBeforeZoom[1] <= 315) {
			glTranslatef(0.0, -(1 - help), 0.0);
		} else {
			if (objectRotationBeforeZoom[0] <= 45 || objectRotationBeforeZoom[0] >= 315) {
				glTranslatef(0.0, 0.0, 1 - help);
			}
			if (objectRotationBeforeZoom[0] <= 315 && objectRotationBeforeZoom[0] >= 225) {
				glTranslatef(1 - help, 0.0, 0.0);
			}
			if (objectRotationBeforeZoom[0] <= 225 && objectRotationBeforeZoom[0] >= 135) {
				glTranslatef(0.0, 0.0, -(1 - help));
			}
			if (objectRotationBeforeZoom[0] <= 135 && objectRotationBeforeZoom[0] >= 45) {
				glTranslatef(-(1 - help), 0.0, 0.0);
			}
		}
	}

	if (objectMode == 1) Triangle();
	else if (objectMode == 2) Pyramid();
	else Egg();

	glutSwapBuffers();
}

// Inicjalizacja współrzędnych punktów potrzebnych do wyświetlenia jajka
void setVertices() {
	for (int i = 0; i < N; i++) {
		float u = (float) i / (N - 1);
		float uPow5 = pow(u, 5);
		float uPow4 = pow(u, 4);
		float uPow3 = pow(u, 3);
		float uPow2 = pow(u, 2);
		float x, y, z;
		float x_u, x_v, y_u, y_v, z_u, z_v;
		float vector_x, vector_y, vector_z;

		y = 160 * uPow4 - 320 * uPow3 + 160 * uPow2;

		for (int j = 0; j < N; j++) {
			float v = (float) j / (N - 1);

			ARRAY[i][j].texture[0] = (i >= N / 2) ? 1 - u : u;
			ARRAY[i][j].texture[1] = (i >= N / 2) ? 1 - v : v;

			x = (-90 * uPow5 + 225 * uPow4 - 270 * uPow3 + 180 * uPow2 - 45 * u) * cos(M_PI * v);
			z = (-90 * uPow5 + 225 * uPow4 - 270 * uPow3 + 180 * uPow2 - 45 * u) * sin(M_PI * v);

			ARRAY[i][j].point3[0] = x;
			ARRAY[i][j].point3[1] = y - 5;
			ARRAY[i][j].point3[2] = z;

			x_u = (-450 * uPow4 + 900 * uPow3 - 810 * uPow2 + 360 * u - 45) * cos(M_PI * v);
			x_v = M_PI * (90 * uPow5 - 225 * uPow4 + 270 * uPow3 - 180 * uPow2 + 45) * sin(M_PI * v);
			y_u = 640 * uPow3 - 960 * uPow2 + 320 * u;
			y_v = 0;
			z_u = (-450 * uPow4 + 900 * uPow3 - 810 * uPow2 + 360 * u - 45) * sin(M_PI * v);
			z_v = -M_PI * (90 * uPow5 - 225 * uPow4 + 270 * uPow3 - 180 * uPow2 + 45) * cos(M_PI * v);

			vector_x = y_u * z_v - z_u * y_v;
			vector_y = z_u * x_v - x_u * z_v;
			vector_z = x_u * y_v - y_u * x_v;

			float vector_len = sqrt(pow(vector_x, 2) + pow(vector_y, 2) + pow(vector_z, 2));
			if (vector_len == 0) vector_len = 1;

			vector_x = vector_x / vector_len;
			vector_y = vector_y / vector_len;
			vector_z = vector_z / vector_len;

			ARRAY[i][j].normalized[0] = (i == N - 1) ? ARRAY[0][j].normalized[0] : vector_x;
			ARRAY[i][j].normalized[1] = (i == N - 1) ? ARRAY[0][j].normalized[1] : vector_y;
			ARRAY[i][j].normalized[2] = (i == N - 1) ? ARRAY[0][j].normalized[2] : vector_z;

		}
	}
}

// Funkcja inicjalizuje tablicę 'ARRAY' oraz wywołuje funkcje inicjalizujące
// kolory i współrzędne punktów
void setArray() {
	cout << "Podaj liczbe N = ";
	cin >> N;
	auto **newArray = new VERTEXES *[N];
	for (int i = 0; i < N; i++) {
		newArray[i] = new VERTEXES[N];
	}
	ARRAY = newArray;
	setVertices();
}

// Funkcja wyświetlająca w konsoli informacje o opcjach programu
void initProgram() {
	cout << "Program z laboratorium 6 - teksturowanie powierzchni obiektow\n";
	setArray();
	cout << "Odpowiednie klawisze zmieniaja widok modelu:\n";
	cout << "   Wyswietlany obiekt:\n";
	cout << "      'f' - trojkat (domyslne)\n";
	cout << "      'g' - piramida\n";
	cout << "      'h' - jajko\n";
	cout << "   Wybor tekstury:\n";
	cout << "      'a' - pierwsza tekstura (domyslne)\n";
	cout << "      's' - druga tekstura\n";
	cout << "   Wybrany tryb obracania:\n";
	cout << "      '1' - wlaczenie\\wylaczenie podstawy piramidy\n";
	cout << "      '2' - wlaczenie\\wylaczenie pierwszej sciany piramidy\n";
	cout << "      '3' - wlaczenie\\wylaczenie drugiej sciany piramidy\n";
	cout << "      '4' - wlaczenie\\wylaczenie trzeciej sciany piramidy\n";
	cout << "      '5' - wlaczenie\\wylaczenie czwartej sciany piramidy\n";
	cout << "   Sterowanie myszka:\n";
	cout << "      'Przytrzymaj LPM' - obrot obiektu\n";
	cout << "      'Przytrzymaj PPM' - zoom obiektu\n";
	cout << "   Operowanie programem:\n";
	cout << "      'esc' - wyjscie z programu\n";
}

// Funkcja obsługująca działanie programu za pomocą klawiszy klawiatury
void keys(unsigned char key, int x, int y) {
	if (key == '1') displayPyramidWall ^= 0b1 << 4;
	if (key == '2') displayPyramidWall ^= 0b1 << 3;
	if (key == '3') displayPyramidWall ^= 0b1 << 2;
	if (key == '4') displayPyramidWall ^= 0b1 << 1;
	if (key == '5') displayPyramidWall ^= 0b1;
	if (key == 'a') chosenTexture = 1;
	if (key == 's') chosenTexture = 0;
	if (key == 'f') objectMode = 1;
	if (key == 'g') objectMode = 2;
	if (key == 'h') objectMode = 3;
	if (key == (char) 0x1B) exit(0);    // Esc key

	renderScene(); // przerysowanie obrazu sceny
}

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) sś
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna.
void changeSize(GLsizei horizontal, GLsizei vertical) {

	pix2angle = 360.0 / (float) horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego

	if (horizontal <= vertical) {
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	} else {
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	}
	// Ustawienie wielkości okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

// Funkcja obsługuje wciśnięcie przycisku na myszce
void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;
		mouseStatus = 1;          // wciśnięty został lewy klawisz myszy
	} else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		y_pos_old = y;
		mouseStatus = 2;
	} else {
		mouseStatus = 0;          // nie został wciśnięty żaden klawisz
	}
}

// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych
void Motion(GLsizei x, GLsizei y) {

	delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
	delta_y = y - y_pos_old;

	x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie
	y_pos_old = y;

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

// Funkcja ustalająca stan renderowania
void myInit() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny

//          --<<OŚWIETLENIE>>--

// Definicja materiału, z jakiego zrobiony jest obiekt
	GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 0.5};
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia

	GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego

	GLfloat mat_shininess = {20.0};
	// współczynnik n opisujący połysk powierzchni

// Definicja stałych odpowiadających za osłabienie światła dla obu źródeł
	GLfloat att_constant = {1.0};
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear = {0.05};
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic = {0.001};
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

// Definicja pierwszego źródła światła
	GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = {0.85, 0.85, 0.85, 0.85};
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

// Ustawienie parametrów materiału
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


// Ustawienie parametrów pierwszego źródła światła
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

// Ustawienie opcji systemu oświetlania sceny
	glShadeModel(GL_SMOOTH); // włączenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // włączenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

//          --<<TEKSTUROWANIE>>--

// Teksturowanie będzie prowadzone tylko po jednej stronie ściany
	glEnable(GL_CULL_FACE);


// Zwolnienie pamięci
	//free(pBytes);

// Włączenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

// Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	renderScene(); // przerysowanie obrazu sceny
}

// Główny punkt wejścia programu. Program działa w trybie konsoli
int main() {

	initProgram();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);

	glutCreateWindow("Teksturowanie powierzchni");

	glutDisplayFunc(renderScene);
	// Określenie, że funkcja renderScene będzie funkcją zwrotną
	// (callback function). Będzie ona wywoływana za każdym razem,
	// gdy zajdzie potrzeba przerysowania okna

	glutReshapeFunc(changeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// za zmiany rozmiaru okna

	glutKeyboardFunc(keys);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu klawiatury

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy

	myInit();
	// Funkcja myInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne przed przystąpieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}
