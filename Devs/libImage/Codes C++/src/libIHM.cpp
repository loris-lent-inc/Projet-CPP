#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <stack>

#include "libIHM.h"

ClibIHM::ClibIHM() {

	this->nbDataImg = 0;
	this->dataFromImg.clear();
	this->imgPt = NULL;
}

ClibIHM::ClibIHM(int nbChamps, byte* data, int stride, int nbLig, int nbCol){
	this->nbDataImg = nbChamps;
	this->dataFromImg.resize(nbChamps);

	this->imgPt = new CImageCouleur(nbLig, nbCol);
	CImageCouleur out(nbLig, nbCol);

	// RECUPERTION DES DONNEES DE L'IMAGE
	byte* pixPtr = (byte*)data;

	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			this->imgPt->operator()(y, x)[0] = pixPtr[3 * x + 2];
			this->imgPt->operator()(y, x)[1] = pixPtr[3 * x + 1];
			this->imgPt->operator()(y, x)[2] = pixPtr[3 * x ];
		}
		pixPtr += stride; // largeur une seule ligne gestion multiple 32 bits
	}
	
	CImageNdg img = this->imgPt->plan();
	// TRAITEMENT
	/*CImageNdg img = this->imgPt->plan(3);
	CImageNdg wth = img.morphologie("WTH");
	CImageNdg bth = img.morphologie("BTH");

	double scoreWTH = wth.correlation_croisee_normalisee();
	double scoreBTH = bth.correlation_croisee_normalisee();
	CImageNdg bst;
	
	if (scoreWTH > scoreBTH)
		bst = wth;
	else
		bst = bth;

	CImageClasse imgSeuil = CImageClasse(bst.seuillage(), "V4");

	double scoreIOU = imgSeuil.IOU();
	double scoreVinet = imgSeuil.Vinet();

	out = CImageCouleur(imgSeuil.toNdg("expansion"));*/



	out = CImageCouleur(img);
	// RECONTRUCTION DU RETOUR IMAGE
	pixPtr = (byte*)data;
	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			pixPtr[3 * x + 2] = out(y, x)[0];
			pixPtr[3 * x + 1] = out(y, x)[1];
			pixPtr[3 * x] = out(y, x)[2];
		}
		pixPtr += stride; // largeur une seule ligne gestion multiple 32 bits
	}
}


ClibIHM::~ClibIHM() {
	
	if (imgPt)
		(*this->imgPt).~CImageCouleur(); 
	this->dataFromImg.clear();
}