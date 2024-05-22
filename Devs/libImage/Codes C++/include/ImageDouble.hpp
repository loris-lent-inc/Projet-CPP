#ifndef _IMAGE_DOUBLE_
#define _IMAGE_DOUBLE_

#include <cmath> // For log2, exp, etc.
#include <complex>
#include <string>
#include <valarray>
#include <vector>

#include "../include/ImageNdg.hpp"

typedef std::complex<double>   Complex;
typedef std::valarray<Complex> CArray;

class CImageDouble
{
private:
    int         m_iHauteur;
    int         m_iLargeur;
    std::string m_sNom;
    double      m_vMin; // valeur plus petite possible
    double      m_vMax; // valeur plus grande possible
    double *    m_pucPixel;

public:
    // constructeurs
    _EXPORT_ CImageDouble();                         // par d�faut
    _EXPORT_ CImageDouble(int hauteur, int largeur); // initialisation � 0
    _EXPORT_ CImageDouble(const CImageNdg &   im,
                          const std::string & methode = "cast"); // choix "normalise"/"cast"/"integrale1"/"integrale2"
    // image Ndg en entr�e
    _EXPORT_ CImageDouble(const CImageDouble & im); // image en entr�e

    // destructeur
    _EXPORT_ ~CImageDouble();

    // pouvoir acceder a un pixel par image(i)
    _EXPORT_ double & operator()(int i) const
    {
        return m_pucPixel[i];
    }

    // pouvoir acceder a un pixel par image(i,j)
    _EXPORT_ double & operator()(int i, int j) const
    {
        return m_pucPixel[i * m_iLargeur + j];
    }

    // operateur copie image par imOut = imIn
    _EXPORT_ CImageDouble & operator=(const CImageDouble & im);

    // lireters et ecrireters
    _EXPORT_ int lireHauteur() const
    {
        return m_iHauteur;
    }

    _EXPORT_ int lireLargeur() const
    {
        return m_iLargeur;
    }

    _EXPORT_ double lireMin() const
    {
        return m_vMin;
    }

    _EXPORT_ double lireMax() const
    {
        return m_vMax;
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

    _EXPORT_ void ecrireMin(double min)
    {
        m_vMin = min;
    }

    _EXPORT_ void ecrireMax(double max)
    {
        m_vMax = max;
    }

    _EXPORT_ void ecrireNom(std::string nom)
    {
        m_sNom = nom;
    }

    _EXPORT_ int lireNbPixels() const
    {
        return m_iHauteur * m_iLargeur;
    }

    _EXPORT_ CImageDouble seuillage(double seuilBas = 0, double seuilHaut = 1);

    // distance au fond
    _EXPORT_ CImageDouble distance(std::string eltStructurant = "V8", double valBord = 0);

    // conversions
    _EXPORT_ CImageNdg toNdg(const std::string & methode = "defaut"); // choix "defaut"/"expansion"

    _EXPORT_ CImageDouble planHough();
    _EXPORT_ CImageNdg    maxiLocaux() const;
    _EXPORT_ CImageDouble extractionLignes(int N = 9, int M = 9, double dimLigne = 50, bool affichage = "true");
    _EXPORT_ CImageNdg    houghInverse(const CImageNdg & img);

    // Vecteur gradient
    _EXPORT_ CImageDouble vecteurGradient(const std::string & axe = "norme") const; // choix "norme"/"angle"

    // Filtrage : moyen/gaussien
    _EXPORT_ CImageDouble filtrage(const std::string & methode = "moyen", int N = 3, double sigma = 0.5);

    // pyramides
    // _EXPORT_ std::vector<CImageDouble> pyramide(int hauteur = 5, int /*tailleFiltre = 5*/, double /*sigma = 1*/);
};

#endif