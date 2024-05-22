#ifndef _IMAGE_COULEUR_
#define _IMAGE_COULEUR_

#include <algorithm>
#include <string>
#include <vector>

#include "ImageNdg.hpp"

#define MIN3(x, y, z) ((y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)))
#define MAX3(x, y, z) ((y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)))

class CImageCouleur
{
private:
    int              m_iHauteur;
    int              m_iLargeur;
    std::string      m_sNom;
    unsigned char *  m_pucData;
    unsigned char ** m_ppucPixel;

public:
    // constructors
    _EXPORT_ CImageCouleur();
    _EXPORT_ CImageCouleur(int hauteur, int largeur, int valR = -1, int valV = -1,
                           int valB = -1); // si -1 alors non pixels non initialis�s
    _EXPORT_ CImageCouleur(const std::string nom);
    _EXPORT_ CImageCouleur(const CImageCouleur & im);
    _EXPORT_ CImageCouleur(const CImageNdg & im);
    _EXPORT_ CImageCouleur(const CImageNdg & pR, const CImageNdg & pG, const CImageNdg & pB);

    _EXPORT_ CImageCouleur(const CImageNdg & im, const CImageNdg & binaire, int R = 255, int G = 0,
                           int B = 0); // img binaire de pr�f�rence contours
    _EXPORT_ CImageCouleur(const CImageCouleur & im, const CImageNdg & binaire, int R = 255, int G = 0,
                           int B = 0); // img binaire de pr�f�rence contours

    _EXPORT_ ~CImageCouleur(); // destructor

    // Save under BMP format
    _EXPORT_ void sauvegarde(const std::string file = "");

    // Access to a pixel by image(i,j)[plan]
    _EXPORT_ unsigned char *& operator()(int i, int j) const
    {
        return m_ppucPixel[i * m_iLargeur + j];
    }

    // Access to a pixel by image(i)[plan]
    _EXPORT_ unsigned char *& operator()(int i) const
    {
        return m_ppucPixel[i];
    }
    // Copy image by imOut = imIn
    _EXPORT_ CImageCouleur & operator=(const CImageCouleur & im);

    // Get and set
    _EXPORT_ int lireHauteur() const
    {
        return m_iHauteur;
    }

    _EXPORT_ int lireLargeur() const
    {
        return m_iLargeur;
    }

    _EXPORT_ std::string lireNom() const
    {
        return m_sNom;
    }

    _EXPORT_ void ecrireHauteur(int hauteur)
    {
        m_iHauteur = hauteur;
    }

    _EXPORT_ void ecrireLargeur(int largeur)
    {
        m_iLargeur = largeur;
    }

    _EXPORT_ void ecrireNom(std::string nom)
    {
        m_sNom = nom;
    }

    _EXPORT_ int lireNbPixels() const
    {
        return m_iHauteur * m_iLargeur;
    }

    // Histogram
    _EXPORT_ std::vector<unsigned long> histogramme(bool enregistrementCSV = false);

    // Plans
    // options : "0" -> R or H, "1" -> V or S, "2" -> B or V et
    // "3" -> luminance hence weights fct of illuminant
    _EXPORT_ CImageNdg       plan(int choix = 3, double poidsRouge = 1. / 3., double poidsVert = 1. / 3.,
                                  double poidsBleu = 1. / 3.);
    _EXPORT_ CImageCouleur & miseAJourPlan(int choix, const CImageNdg & plan);

    // Color space changes : HSV
    // option "HSV"
    _EXPORT_ CImageCouleur conversion(const std::string methode = "HSV") const; // accessible par les objets constants

    // transformation
    // options : "expansion" on [0,255], "egalisation"
    _EXPORT_ CImageCouleur
    transformation(const std::string methode = "expansion"); // version avec sortie sur [0,255] pour expansion notamment sur chaque plan

    //Crop
    _EXPORT_ CImageCouleur crop(int x, int y, int w, int h) const;

    // signatures
    _EXPORT_ std::vector<MOMENTS> signatures();
};

#endif // _IMAGE_COULEUR_