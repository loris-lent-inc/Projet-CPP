// testAppCPP.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>

#include "../libImage/Codes C++/include/elemStruct.h"
#include "../libImage/Codes C++/include/ImageNdg.hpp"
#include "../libImage/Codes C++/include/ImageCouleur.hpp"
#include "../libImage/Codes C++/include/ImageClasse.hpp"
#include "../libImage/Codes C++/include/ImageDouble.hpp"

int main()
{
	CImageNdg img("Source Images - bmp\\In_1.bmp");
	CImageNdg ref("Ground truth - bmp\\In_1.bmp");
	
	
	auto D17 = elemStruct::disque(17, 1);
	CImageNdg wth = img.morphologie("WTH", D17);
	CImageNdg bth = img.morphologie("median", elemStruct::disque(11, 1)).morphologie("BTH", D17);
	CImageNdg sso = img.seuillage();

	double scoreWTH = wth.correlation_croisee_normalisee(ref);
	double scoreBTH = bth.correlation_croisee_normalisee(ref);
	double scoreSSO = sso.correlation_croisee_normalisee(ref);

	CImageNdg bst = sso;
	double bestScore = scoreSSO;
	if (scoreWTH > bestScore)
	{
		bst = wth;
		bestScore = scoreWTH;
	}
	if (scoreBTH > bestScore)
	{
		bst = bth;
		bestScore = scoreBTH;
	}

	CImageClasse imgSeuil = CImageClasse(bst.seuillage(), "V4");

	CImageCouleur out = CImageCouleur(imgSeuil.toNdg("expansion"));
	out.sauvegarde();
}