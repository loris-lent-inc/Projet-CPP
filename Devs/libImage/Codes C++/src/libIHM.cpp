#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <stack>
#include <thread>


#include "../include/libIHM.h"




ClibIHM::ClibIHM() {

	this->nbDataImg = 0;
	this->dataFromImg.clear();
	this->imgPt = NULL;
}

ClibIHM::ClibIHM(int nbChamps, byte* data, byte* refIm, int stride, int nbLig, int nbCol){
	this->nbDataImg = nbChamps;
	this->dataFromImg.resize(nbChamps);

	this->imgPt = new CImageCouleur(nbLig, nbCol);
	CImageCouleur refC(nbLig, nbCol);
	CImageCouleur out(nbLig, nbCol);

	// RECUPERTION DES DONNEES DE L'IMAGE
	byte* pixPtr = (byte*)data;
	byte* refPtr = (byte*)refIm;

	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			this->imgPt->operator()(y, x)[0] = pixPtr[3 * x + 2];
			refC(y, x)[0] = refPtr[3 * x + 2];
			this->imgPt->operator()(y, x)[1] = pixPtr[3 * x + 1];
			refC(y, x)[1] = refPtr[3 * x + 1];
			this->imgPt->operator()(y, x)[2] = pixPtr[3 * x ];
			refC(y, x)[2] = refPtr[3 * x];
		}
		pixPtr += stride; // largeur une seule ligne gestion multiple 32 bits
		refPtr += stride;
	}
	
	CImageNdg img = this->imgPt->plan().morphologie("median", elemStruct::croix(5));
	CImageNdg ref = refC.plan();

	// TRAITEMENT
	
	auto D17 = elemStruct::disque(8);
	
	CImageNdg wth, bth;
	double scoreWTH, scoreBTH, scoreIOU, scoreVinet;
	
	std::thread tw([&img, &ref, &D17, &wth, &scoreWTH]() {
		wth = img.morphologie("WTH", D17);
		scoreWTH = wth.correlation_croisee_normalisee(ref);
		});

	std::thread tb([&img, &ref, &D17, &bth, &scoreBTH]() {
		bth = img.morphologie("BTH", D17);
		scoreBTH = bth.correlation_croisee_normalisee(ref);
		});


	tw.join(); tb.join();

	CImageNdg bst = wth.seuillage();
	double bestScore = scoreWTH;

	if (scoreBTH > bestScore){
		bst = bth.seuillage();
		bestScore = scoreBTH;
	}

	CImageClasse imgSeuil = CImageClasse(bst, "V4");
	CImageClasse refClass = CImageClasse(ref, "V4");
	imgSeuil = imgSeuil.filtrage("taille", 100, true);

	out = CImageCouleur(imgSeuil.toNdg("expansion"));

	scoreIOU = imgSeuil.IOU(refClass);
	scoreVinet = imgSeuil.Vinet(refClass);	
	
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
	
	this->dataFromImg[0] = scoreVinet;
	this->dataFromImg[1] = scoreIOU;
}


ClibIHM::~ClibIHM() {
	
	if (imgPt)
		(*this->imgPt).~CImageCouleur(); 
	this->dataFromImg.clear();
}