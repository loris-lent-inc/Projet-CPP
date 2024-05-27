#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <stack>

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
	
	CImageNdg img = this->imgPt->plan();
	CImageNdg ref = refC.plan();




	// TRAITEMENT
	
	//ref.sauvegarde("ref");
	auto D17 = elemStruct::disque(8, 1);
	CImageNdg wth = img.morphologie("WTH", D17);
	CImageNdg bth = img.morphologie("median", elemStruct::disque(5, 1)).morphologie("BTH", D17);
	CImageNdg sso = img.seuillage();

	double scoreWTH = wth.correlation_croisee_normalisee(ref);
	double scoreBTH = bth.correlation_croisee_normalisee(ref);
	double scoreSSO = sso.correlation_croisee_normalisee(ref);

	wth.seuillage().sauvegarde("wth" + std::to_string(scoreWTH));
	bth.seuillage().sauvegarde("bth" + std::to_string(scoreBTH));
	//sso.sauvegarde("ss" + std::to_string(scoreSSO));

	CImageNdg bst = sso;
	double bestScore = scoreSSO;

	if (scoreWTH > bestScore){
		bst = wth.seuillage();
		bestScore = scoreWTH;
	}
	if (scoreBTH > bestScore){
		bst = bth.seuillage();
		bestScore = scoreBTH;
	}

	CImageClasse imgSeuil = CImageClasse(bst, "V4");
	CImageClasse refClass = CImageClasse(ref, "V4");

	imgSeuil = imgSeuil.filtrage("taille", 100, true);

	double scoreIOU = imgSeuil.IOU(refClass);
	double scoreVinet = imgSeuil.Vinet(refClass);
	
	out = CImageCouleur(imgSeuil.toNdg("expansion"));
	out.plan().sauvegarde();
	
	this->dataFromImg[0] = scoreWTH;
	this->dataFromImg[1] = scoreBTH;
	this->dataFromImg[2] = scoreSSO;
	this->dataFromImg[3] = scoreIOU;
	this->dataFromImg[4] = scoreVinet;


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