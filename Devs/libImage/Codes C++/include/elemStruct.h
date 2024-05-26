#pragma once

#ifndef _ELEM_STRUCT_
#define _ELEM_STRUCT_

#include <string>
#include <vector>

class elemStruct
{
	///////////////////////////////////////
private:
	///////////////////////////////////////

	int              m_iHauteur;
	int              m_iLargeur;
	std::string      m_sNom;

	double*			 m_pucPixel;

	///////////////////////////////////////
public:
	///////////////////////////////////////

		// constructeurs
	_declspec(dllexport) elemStruct(int w, int h, std::string nom = "elem");
	_declspec(dllexport) elemStruct(const elemStruct& source);

	// destructeur
	_declspec(dllexport) ~elemStruct();

	// Formes standards
	_declspec(dllexport) static elemStruct V4(double val = 1);
	_declspec(dllexport) static elemStruct V8(double val = 1);
	_declspec(dllexport) static elemStruct rectangle(int x1, int y1, int x2, int y2, double val = 1);
	_declspec(dllexport) static elemStruct disque(int r, double val = 1);
	_declspec(dllexport) static elemStruct croix(int r, double val = 1);
	_declspec(dllexport) static elemStruct etoile(int r, double val = 1);
	_declspec(dllexport) static elemStruct gaussien(int r, double val = 1);


	// Accesseurs
	_declspec(dllexport) int lireHauteur() const;
	_declspec(dllexport) int lireLargeur() const;
	_declspec(dllexport) std::string lireNom() const;
	_declspec(dllexport) double lirePixel(int x, int y) const;
	_declspec(dllexport) double operator()(int x, int y) const;

};




#endif // _ELEM_STRUCT_