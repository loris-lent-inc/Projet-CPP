#include "float.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <vector>

#include "../include/ImageDouble.hpp"

#define PI 3.14159265358979323846

// constructeurs et destructeur
CImageDouble::CImageDouble()
{

    this->m_iHauteur = 0;
    this->m_iLargeur = 0;
    this->m_sNom     = "vide";
    this->m_vMin     = 0;
    this->m_vMax     = 0;
    this->m_pucPixel = NULL;
}

CImageDouble::CImageDouble(int hauteur, int largeur)
{

    this->m_iHauteur = hauteur;
    this->m_iLargeur = largeur;
    this->m_sNom     = "inconnu";
    this->m_vMin     = 0;
    this->m_vMax     = 0;

    this->m_pucPixel = new double[hauteur * largeur];
    for (int i = 0; i < hauteur * largeur; i++)
        this->m_pucPixel[i] = 0;
}

CImageDouble::CImageDouble(const CImageDouble & im)
{

    this->m_iHauteur = im.lireHauteur();
    this->m_iLargeur = im.lireLargeur();
    this->m_sNom     = im.lireNom();
    this->m_vMax     = im.lireMax();
    this->m_vMin     = im.lireMin();
    this->m_pucPixel = NULL;

    if (im.m_pucPixel != NULL)
    {
        this->m_pucPixel = new double[im.lireNbPixels()];
        memcpy(this->m_pucPixel, im.m_pucPixel, im.lireNbPixels() * sizeof(double));
    }
}

CImageDouble::~CImageDouble()
{

    if (this->m_pucPixel)
    {
        delete[] this->m_pucPixel;
        this->m_pucPixel = NULL;
    }
}

CImageDouble::CImageDouble(const CImageNdg & im, const std::string & methode)
{

    this->m_iHauteur = im.lireHauteur();
    this->m_iLargeur = im.lireLargeur();

    this->m_pucPixel = new double[im.lireNbPixels()];
    if (methode.compare("normalise") == 0)
    {
        this->m_sNom = im.lireNom() + "DN";
        this->m_vMin = 0;
        this->m_vMax = 1;
        for (int i = 0; i < this->lireNbPixels(); i++)
            this->operator()(i) = (double)im(i) / 255;
    }
    else if (methode.compare("cast") == 0)
    {
        this->m_sNom = im.lireNom() + "DC";
        this->m_vMin = 255;
        this->m_vMax = 0;
        for (int i = 0; i < this->lireNbPixels(); i++)
        {
            this->operator()(i) = (double)im(i);
            if (this->operator()(i) > this->m_vMax)
                this->m_vMax = this->operator()(i);
            if (this->operator()(i) < this->m_vMin)
                this->m_vMin = this->operator()(i);
        }
    }
    else if (methode.compare("integrale1") == 0)
    {
        this->m_sNom = im.lireNom() + "DI1";

        CImageDouble s(im.lireHauteur(), im.lireLargeur());

        // premi�re ligne
        s(0)                = (double)im(0, 0);
        this->operator()(0) = (double)im(0, 0);
        this->ecrireMin((double)im(0, 0));

        for (int j = 1; j < im.lireLargeur(); j++)
        {
            s(j)                = s(j - 1) + (double)im(0, j);
            this->operator()(j) = this->operator()(j - 1) + (double)im(0, j);
        }

        // le reste
        for (int i = 1; i < im.lireHauteur(); i++)
        {
            s(i, 0)                = (double)im(i, 0);
            this->operator()(i, 0) = this->operator()(i - 1, 0) + (double)im(i, 0);
            for (int j = 1; j < im.lireLargeur(); j++)
            {
                s(i, j)                = s(i, j - 1) + (double)im(i, j);
                this->operator()(i, j) = this->operator()(i - 1, j) + s(i, j);
            }
        }

        this->ecrireMax(this->operator()(this->lireNbPixels() - 1));
    }
    else if (methode.compare("integrale2") == 0)
    {
        this->m_sNom = im.lireNom() + "DI2";
        CImageDouble s(im.lireHauteur(), im.lireLargeur());

        // premi�re ligne
        s(0)                = (double)im(0, 0) * (double)im(0, 0);
        this->operator()(0) = (double)im(0, 0) * (double)im(0, 0);
        this->ecrireMin(this->operator()(0));

        for (int j = 1; j < im.lireLargeur(); j++)
        {
            s(j)                = s(j - 1) + (double)im(0, j) * (double)im(0, j);
            this->operator()(j) = this->operator()(j - 1) + (double)im(0, j) * (double)im(0, j);
        }

        // le reste
        for (int i = 1; i < im.lireHauteur(); i++)
        {
            s(i, 0)                = (double)im(i, 0) * (double)im(i, 0);
            this->operator()(i, 0) = this->operator()(i - 1, 0) + (double)im(i, 0) * (double)im(i, 0);
            for (int j = 1; j < im.lireLargeur(); j++)
            {
                s(i, j)                = s(i, j - 1) + (double)im(i, j) * (double)im(i, j);
                this->operator()(i, j) = this->operator()(i - 1, j) + s(i, j);
            }
        }

        this->ecrireMax(this->operator()(this->lireNbPixels() - 1));
    }
}

CImageDouble & CImageDouble::operator=(const CImageDouble & im)
{

    if (&im == this)
        return *this;

    this->m_iHauteur = im.lireHauteur();
    this->m_iLargeur = im.lireLargeur();
    this->m_sNom     = im.lireNom();
    this->m_vMax     = im.lireMax();
    this->m_vMin     = im.lireMin();

    if (this->m_pucPixel)
        delete[] this->m_pucPixel;
    this->m_pucPixel = new double[im.lireNbPixels()];

    if (im.m_pucPixel != NULL)
        memcpy(this->m_pucPixel, im.m_pucPixel, im.lireNbPixels() * sizeof(double));

    return *this;
}

CImageDouble CImageDouble::seuillage(double seuilBas, double seuilHaut)
{

    // pas de LUT possible, donn�es flottantes
    CImageDouble out(this->lireHauteur(), this->lireLargeur());
    out.m_sNom = this->lireNom() + "S";
    out.m_vMin = 0;
    out.m_vMax = 1;

    for (int i = 0; i < out.lireNbPixels(); i++)
        if ((this->operator()(i) >= seuilBas) && (this->operator()(i) <= seuilHaut))
            out(i) = 1;

    return out;
}

// distance au fond
CImageDouble CImageDouble::distance(std::string eltStructurant, double valBord)
{
    // distance au fond
    // gestion du bord : 0 ou autre valeur valant max des float

    CImageDouble out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom() + "DF");
    out.ecrireMax(DBL_MIN);
    out.ecrireMin(DBL_MAX);

    CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);
    // gestion des bords ajout�s
    if (valBord != 0)
    {
        for (int i = 0; i < agrandie.lireHauteur(); i++)
        {
            agrandie(i, 0)                          = valBord;
            agrandie(i, agrandie.lireLargeur() - 1) = valBord;
        }
        for (int j = 0; j < agrandie.lireLargeur(); j++)
        {
            agrandie(0, j)                          = valBord;
            agrandie(agrandie.lireHauteur() - 1, j) = valBord;
        }
    }

    // coeur image
    for (int i = 0; i < this->lireHauteur(); i++)
        for (int j = 0; j < this->lireLargeur(); j++)
        {
            agrandie(i + 1, j + 1) = (this->operator()(i, j) > 0) ? (double)INT_MAX - 1 : 0;
        }

    if (eltStructurant.compare("V4") == 0)
    {
        // parcours avant
        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                double pixel   = agrandie(i, j);
                pixel          = min(pixel, agrandie(i - 1, j) + 1);
                pixel          = min(pixel, agrandie(i, j - 1) + 1);
                agrandie(i, j) = pixel;
            }
        for (int i = agrandie.lireHauteur() - 2; i >= 1; i--)
            for (int j = agrandie.lireLargeur() - 2; j >= 1; j--)
            {
                double pixel   = agrandie(i, j);
                pixel          = min(pixel, agrandie(i + 1, j) + 1);
                pixel          = min(pixel, agrandie(i, j + 1) + 1);
                agrandie(i, j) = pixel;
            }
        // conservation du centre
        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                out(i - 1, j - 1) = agrandie(i, j);
            }
    }
    else
    {
        // parcours avant
        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                double pixel   = agrandie(i, j);
                pixel          = min(pixel, agrandie(i - 1, j) + 1);
                pixel          = min(pixel, agrandie(i, j - 1) + 1);
                pixel          = min(pixel, agrandie(i - 1, j - 1) + 1);
                pixel          = min(pixel, agrandie(i - 1, j + 1) + 1);
                agrandie(i, j) = pixel;
            }
        for (int i = agrandie.lireHauteur() - 2; i >= 1; i--)
            for (int j = agrandie.lireLargeur() - 2; j >= 1; j--)
            {
                double pixel   = agrandie(i, j);
                pixel          = min(pixel, agrandie(i + 1, j) + 1);
                pixel          = min(pixel, agrandie(i, j + 1) + 1);
                pixel          = min(pixel, agrandie(i + 1, j + 1) + 1);
                pixel          = min(pixel, agrandie(i + 1, j - 1) + 1);
                agrandie(i, j) = pixel;
            }
        // conservation du centre
        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                out(i - 1, j - 1) = agrandie(i, j);
                if (out(i - 1, j - 1) < out.lireMin())
                    out.ecrireMin(out(i - 1, j - 1));
                if (out(i - 1, j - 1) > out.lireMax())
                    out.ecrireMax(out(i - 1, j - 1));
            }
    }

    return out;
}

// conversion entre types
CImageNdg CImageDouble::toNdg(const std::string & methode)
{

    CImageNdg out(this->lireHauteur(), this->lireLargeur());
    out.ecrireBinaire(false);
    out.choixPalette("grise");
    out.ecrireNom(this->lireNom() + "2NDG");

    if (methode.compare("defaut") == 0)
    {
        for (int i = 0; i < this->lireNbPixels(); i++)
            if (this->operator()(i) < 0)
                out(i) = 0;
            else if (this->operator()(i) > 256)
                out(i) = 255;
            else
                out(i) = (unsigned char)this->operator()(i);
    }
    else if (methode.compare("expansion") == 0)
    {
        double a = 255 / (this->lireMax() - this->lireMin());
        double b = -a * this->lireMin();

        for (int i = 0; i < this->lireNbPixels(); i++)
            out(i) = (unsigned char)(a * this->operator()(i) + b);
    }

    return (out);
}

CImageDouble CImageDouble::planHough()
{

    double hough_h = max(this->lireHauteur() / 2, this->lireLargeur() / 2) * sqrt(2.0);

    CImageDouble H((int)(hough_h * 2), 180);
    H.ecrireMin(0);
    H.ecrireMax(0);

    double cx = this->lireLargeur() / 2;
    double cy = this->lireHauteur() / 2;

    // HOUGH transformation

    for (int y = 0; y < this->lireHauteur(); y++)
    {
        for (int x = 0; x < this->lireLargeur(); x++)
        {
            if (this->operator()(y, x) > 0)
                for (int t = 0; t < 180; t++)
                {
                    double r = (((double)x - cx) * cos((double)t * (PI / 180)) +
                                (((double)y - cy) * sin((double)t * (PI / 180))));
                    H(int(hough_h + r), t) += 1;
                }
        }
    }

    for (int p = 0; p < H.lireNbPixels(); p++)
        if (H(p) > H.lireMax())
            H.ecrireMax(H(p));

    return H;
}

_EXPORT_ CImageNdg CImageDouble::maxiLocaux() const
{
    CImageDouble NORME;
    CImageDouble NORME2(lireHauteur() + 2, lireLargeur() + 2);
    CImageDouble ANGLE;
    CImageNdg    COLOR(lireHauteur(), lireLargeur());

    NORME = vecteurGradient("norme");
    ANGLE = vecteurGradient("angle");

    // gestion des bords et des coins
    int pix;
    NORME2(0, 0)                        = NORME(0, 0);
    NORME2(0, NORME2.lireLargeur() - 1) = NORME(0, NORME.lireLargeur() - 1);
    for (pix = 1; pix < NORME2.lireLargeur() - 1; pix++)
    {
        NORME2(0, pix)                        = NORME(0, pix - 1);
        NORME2(NORME2.lireHauteur() - 1, pix) = NORME(NORME.lireHauteur() - 1, pix - 1);
    }
    NORME2(NORME2.lireHauteur() - 1, 0) = NORME(NORME.lireHauteur() - 1, 0);
    NORME2(NORME2.lireHauteur() - 1, NORME2.lireLargeur() - 1) =
        NORME(NORME.lireHauteur() - 1, NORME.lireLargeur() - 1);
    for (pix = 1; pix < NORME2.lireHauteur() - 1; pix++)
    {
        NORME2(pix, 0)                        = NORME(pix - 1, 0);
        NORME2(pix, NORME2.lireLargeur() - 1) = NORME(pix - 1, NORME.lireLargeur() - 1);
    }

    // gestion du coeur
    for (int i = 0; i < NORME.lireHauteur(); i++)
        for (int j = 0; j < NORME.lireLargeur(); j++)
        {
            NORME2(i + 1, j + 1) = NORME(i, j);
        }

    // gestion maxi locaux
    for (int i = 0, ii = 1; i < lireHauteur(); i++, ii++)
    {
        for (int j = 0, jj = 1; j < lireLargeur(); j++, jj++)
        {
            double temp  = ANGLE(i, j);
            double norme = NORME2(ii, jj);

            if ((ANGLE(i, j) >= -22.5 && ANGLE(i, j) < 22.5) || (ANGLE(i, j) >= 157.5 && ANGLE(i, j) <= 180) ||
                (ANGLE(i, j) >= -180 && ANGLE(i, j) < -157.5))
            {
                if (NORME2(ii, jj) >= NORME2(ii + 1, jj) && NORME2(ii, jj) >= NORME2(ii - 1, jj) && NORME2(ii, jj) != 0)
                {
                    COLOR(i, j) = 1;
                }
                else
                {
                    COLOR(i, j) = 0;
                }
            }

            if ((ANGLE(i, j) >= 22.5 && ANGLE(i, j) < 67.5) || (ANGLE(i, j) >= -157.5 && ANGLE(i, j) < -112.5))
            {
                if (NORME2(ii, jj) >= NORME2(ii + 1, jj + 1) && NORME2(ii, jj) >= NORME2(ii - 1, jj - 1) &&
                    NORME2(ii, jj) != 0)
                {
                    COLOR(i, j) = 3;
                }
                else
                {
                    COLOR(i, j) = 0;
                }
            }

            if ((ANGLE(i, j) >= 67.5 && ANGLE(i, j) < 112.5) || (ANGLE(i, j) >= -112.5 && ANGLE(i, j) < -67.5))
            {
                if (NORME2(ii, jj) >= NORME2(ii, jj + 1) && NORME2(ii, jj) >= NORME2(ii, jj - 1) && NORME2(ii, jj) != 0)
                {
                    COLOR(i, j) = 2;
                }
                else
                {
                    COLOR(i, j) = 0;
                }
            }

            if ((ANGLE(i, j) >= 112.5 && ANGLE(i, j) < 157.5) || (ANGLE(i, j) >= -67.5 && ANGLE(i, j) < -22.5))
            {
                if (NORME2(ii, jj) >= NORME2(ii - 1, jj + 1) && NORME2(ii, jj) >= NORME2(ii + 1, jj - 1) &&
                    NORME2(ii, jj) != 0)
                {
                    COLOR(i, j) = 5;
                }
                else
                {
                    COLOR(i, j) = 0;
                }
            }
        }
    }

    COLOR.choixPalette("binaire");

    return COLOR;
}

CImageDouble CImageDouble::extractionLignes(int /*N*/, int /*M*/, double dimLigne, bool affichage)
{

    // extraction des maxi locaux sur voisinage NxM
    CImageDouble mL; // = this->maxiLocaux(N,M);
    double       hough_h = this->lireHauteur() / 2;

    // extraction des lignes avec maxi local + seuil

    CImageDouble lignes(this->lireHauteur(), this->lireLargeur());
    lignes.ecrireNom("lignes");

    int                nbLignes = 1;
    std::stack<int>    angles;
    std::stack<double> rho;

    for (int r = 0; r < this->lireHauteur(); r++)
        for (int a = 0; a < this->lireLargeur(); a++)
            if ((mL(r, a) > 0) && (this->operator()(r, a) >= dimLigne))
            {
                lignes(r, a) = nbLignes++;
                angles.push(a);
                rho.push(r - hough_h);
            }
    if (affichage)
    {
        std::cout << nbLignes - 1 << " lignes avec les infos suivants :" << std::endl;
        while (!angles.empty())
        {
            std::cout << "(" << angles.top() << "," << rho.top() << ")";
            angles.pop();
            rho.pop();

            if (!angles.empty())
                std::cout << " ; ";
        }
        std::cout << std::endl;
    }

    lignes.m_vMin = 0;
    lignes.m_vMax = nbLignes - 1;

    return lignes;
}

CImageNdg CImageDouble::houghInverse(const CImageNdg & img)
{

    CImageNdg HI(img.lireHauteur(), img.lireLargeur(), 0);
    HI.ecrireNom(img.lireNom() + "HI");
    HI.choixPalette(img.lirePalette());

    double hough_h = max(img.lireHauteur() / 2, img.lireLargeur() / 2) * sqrt(2.0);

    double cx      = img.lireLargeur() / 2;
    double cy      = img.lireHauteur() / 2;

    for (int y = 0; y < img.lireHauteur(); y++)
    {
        for (int x = 0; x < img.lireLargeur(); x++)
        {
            if (img(y, x) > 0)
            {
                for (int t = 0; t < 180; t++)
                {
                    double r = (((double)x - cx) * cos((double)t * (PI / 180)) +
                                (((double)y - cy) * sin((double)t * (PI / 180))));
                    if (this->operator()(int(hough_h + r), t) > 0)
                        HI(y, x) = (int)(this->operator()(int(hough_h + r), t)) % 255;
                }
            }
        }
    }

    return (HI);
}

// vecteur gradient
CImageDouble CImageDouble::vecteurGradient(const std::string & axe) const
{
    CImageDouble out(this->lireHauteur(), this->lireLargeur());

    if (axe.compare("norme") == 0)
    {
        out.ecrireNom(this->lireNom() + "NVG");
        double vMin = sqrt(2) * 255;
        double vMax = 0;

        CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);

        // gestion des bords et des coins
        int pix;
        agrandie(0, 0)                          = this->operator()(0, 0);
        agrandie(0, agrandie.lireLargeur() - 1) = this->operator()(0, this->lireLargeur() - 1);
        for (pix = 1; pix < agrandie.lireLargeur() - 1; pix++)
        {
            agrandie(0, pix)                          = this->operator()(0, pix - 1);
            agrandie(agrandie.lireHauteur() - 1, pix) = this->operator()(this->lireHauteur() - 1, pix - 1);
        }
        agrandie(agrandie.lireHauteur() - 1, 0) = this->operator()(this->lireHauteur() - 1, 0);
        agrandie(agrandie.lireHauteur() - 1, agrandie.lireLargeur() - 1) =
            this->operator()(this->lireHauteur() - 1, this->lireLargeur() - 1);
        for (pix = 1; pix < agrandie.lireHauteur() - 1; pix++)
        {
            agrandie(pix, 0)                          = this->operator()(pix - 1, 0);
            agrandie(pix, agrandie.lireLargeur() - 1) = this->operator()(pix - 1, this->lireLargeur() - 1);
        }

        // gestion du coeur
        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                agrandie(i + 1, j + 1) = this->operator()(i, j);
            }

        // passage des noyaux
        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                double vx         = agrandie(i + 1, j) - agrandie(i - 1, j);
                double vy         = agrandie(i, j + 1) - agrandie(i, j - 1);

                out(i - 1, j - 1) = sqrt((vx * vx) + (vy * vy));
                if (out(i - 1, j - 1) < vMin)
                    vMin = out(i - 1, j - 1);
                if (out(i - 1, j - 1) > vMax)
                    vMax = out(i - 1, j - 1);
            }

        out.ecrireMin(vMin);
        out.ecrireMax(vMax);
    }
    else if (axe.compare("angle") == 0)
    {
        out.ecrireNom(this->lireNom() + "AVG");

        CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);

        // gestion des bords et des coins
        int pix;
        agrandie(0, 0)                          = this->operator()(0, 0);
        agrandie(0, agrandie.lireLargeur() - 1) = this->operator()(0, this->lireLargeur() - 1);
        for (pix = 1; pix < agrandie.lireLargeur() - 1; pix++)
        {
            agrandie(0, pix)                          = this->operator()(0, pix - 1);
            agrandie(agrandie.lireHauteur() - 1, pix) = this->operator()(this->lireHauteur() - 1, pix - 1);
        }
        agrandie(agrandie.lireHauteur() - 1, 0) = this->operator()(this->lireHauteur() - 1, 0);
        agrandie(agrandie.lireHauteur() - 1, agrandie.lireLargeur() - 1) =
            this->operator()(this->lireHauteur() - 1, this->lireLargeur() - 1);
        for (pix = 1; pix < agrandie.lireHauteur() - 1; pix++)
        {
            agrandie(pix, 0)                          = this->operator()(pix - 1, 0);
            agrandie(pix, agrandie.lireLargeur() - 1) = this->operator()(pix - 1, this->lireLargeur() - 1);
        }

        // gestion du coeur
        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                agrandie(i + 1, j + 1) = this->operator()(i, j);
            }

        // passage des noyaux

        for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
            for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
            {
                double vx         = agrandie(i + 1, j) - agrandie(i - 1, j);
                double vy         = agrandie(i, j + 1) - agrandie(i, j - 1);
                out(i - 1, j - 1) = atan2(vy, vx) * 180 / PI;
            }

        // fixer intervall -PI.. PI soit -180 .. 180 en degres
        out.ecrireMin(-180);
        out.ecrireMax(180);
    }

    return out;
}

// filtrage : moyen ou gaussien (approches par vecteur et transpos�e plus rapide)
CImageDouble CImageDouble::filtrage(const std::string & methode, int N, double sigma)
{

    CImageDouble out(this->lireHauteur(), this->lireLargeur());
    out.m_vMax = DBL_MIN;
    out.m_vMin = DBL_MAX;

    out.m_sNom = this->lireNom() + "FETy";

    for (int i = 0; i < this->lireHauteur(); i++)
    {
        for (int j = 0; j < this->lireLargeur(); j++)
        {
            double somme     = 0;
            double moy       = 0;
            double ecartType = 0;
            int    nbPixels  = 0;
            for (int k = -N / 2; k <= N / 2; k++)
            {
                for (int l = -N / 2; l <= N / 2; l++)
                {
                    if (i + k >= 0 && i + k < this->lireHauteur() && j + l >= 0 && j + l < this->lireLargeur())
                    {
                        moy += (double)this->operator()(i + k, j + l);
                        nbPixels++;
                    }
                }
            }
            moy /= nbPixels;
            for (int k = -N / 2; k <= N / 2; k++)
            {
                for (int l = -N / 2; l <= N / 2; l++)
                {
                    if (i + k >= 0 && i + k < this->lireHauteur() && j + l >= 0 && j + l < this->lireLargeur())
                    {
                        somme += ((double)this->operator()(i + k, j + l) - moy) *
                                 ((double)this->operator()(i + k, j + l) - moy);
                    }
                }
            }
            ecartType = sqrt(somme / nbPixels);
            out(i, j) = ecartType;
            if (out(i, j) < out.lireMin())
                out.ecrireMin(out(i, j));
            if (out(i, j) > out.lireMax())
                out.ecrireMax(out(i, j));
        }
    }

    if (methode.compare("moyen") == 0)
    {
        out.m_sNom  = this->lireNom() + "FMo";
        int nbBords = N / 2;

        CImageDouble agrandie(this->lireHauteur() + nbBords * 2, this->lireLargeur() + nbBords * 2);

        // gestion du coeur
        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                agrandie(i + nbBords, j + nbBords) = this->operator()(i, j);
            }

        // gestion des bords
        for (int pix = 0; pix < agrandie.lireLargeur(); pix++)
        {
            for (int t = nbBords - 1; t >= 0; t--)
                agrandie(t, pix) = agrandie(nbBords, pix);
            for (int t = agrandie.lireHauteur() - 1; t >= agrandie.lireHauteur() - 1 - nbBords; t--)
                agrandie(t, pix) = agrandie(agrandie.lireHauteur() - 1 - nbBords, pix);
        }
        for (int pix = 0; pix < agrandie.lireHauteur(); pix++)
        {
            for (int t = nbBords - 1; t >= 0; t--)
                agrandie(pix, t) = agrandie(pix, nbBords);
            for (int t = agrandie.lireLargeur() - 1; t >= agrandie.lireLargeur() - 1 - nbBords; t--)
                agrandie(pix, t) = agrandie(pix, agrandie.lireLargeur() - 1 - nbBords);
        }

        CImageDouble agrandie2 = agrandie;

        // colonnes
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                double somme = 0;
                double moy   = 0;

                for (int k = -nbBords; k <= nbBords; k++)
                {
                    moy += (double)agrandie(i - k, j);
                    somme += (double)1;
                }
                agrandie2(i, j) = moy / somme;
            }
        // lignes
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                double somme = 0;
                double moy   = 0;

                for (int l = -nbBords; l <= nbBords; l++)
                {
                    moy += (double)agrandie2(i, j - l);
                    somme += (double)1;
                }
                agrandie(i, j) = (moy / somme);
            }
        // image out
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                out(i - nbBords, j - nbBords) = agrandie(i, j);
                if (out(i - nbBords, j - nbBords) < out.lireMin())
                    out.ecrireMin(out(i - nbBords, j - nbBords));
                if (out(i - nbBords, j - nbBords) > out.lireMax())
                    out.ecrireMax(out(i - nbBords, j - nbBords));
            }
    }
    else if (methode.compare("gaussien") == 0)
    {
        out.m_sNom = this->lireNom() + "FGa";
        // d�finition du noyau
        double noyau[50]; // taille maxi pour optimisation

        double somme = 0; // normalisation
        somme        = somme + 1 - 1;
        for (int i = 0; i < N; i++)
        {
            noyau[i] = exp(-((i - N / 2) * (i - N / 2)) / (2 * sigma * sigma));
            somme += noyau[i];
        }

        // filtrage
        int nbBords = N / 2;

        CImageDouble agrandie(this->lireHauteur() + nbBords * 2, this->lireLargeur() + nbBords * 2);

        // gestion du coeur
        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                agrandie(i + nbBords, j + nbBords) = this->operator()(i, j);
            }

        // gestion des bords
        for (int pix = 0; pix < agrandie.lireLargeur(); pix++)
        {
            for (int t = nbBords - 1; t >= 0; t--)
                agrandie(t, pix) = agrandie(nbBords, pix);
            for (int t = agrandie.lireHauteur() - 1; t >= agrandie.lireHauteur() - 1 - nbBords; t--)
                agrandie(t, pix) = agrandie(agrandie.lireHauteur() - 1 - nbBords, pix);
        }
        for (int pix = 0; pix < agrandie.lireHauteur(); pix++)
        {
            for (int t = nbBords - 1; t >= 0; t--)
                agrandie(pix, t) = agrandie(pix, nbBords);
            for (int t = agrandie.lireLargeur() - 1; t >= agrandie.lireLargeur() - 1 - nbBords; t--)
                agrandie(pix, t) = agrandie(pix, agrandie.lireLargeur() - 1 - nbBords);
        }

        CImageDouble agrandie2 = agrandie;

        // colonnes
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                double somme = 0;
                double moy   = 0;

                for (int k = -nbBords; k <= nbBords; k++)
                {
                    moy += (double)agrandie(i - k, j) * noyau[k + nbBords];
                    somme += noyau[k + nbBords];
                }
                agrandie2(i, j) = moy / somme;
            }
        // lignes
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                double somme = 0;
                double moy   = 0;

                for (int l = -nbBords; l <= nbBords; l++)
                {
                    moy += (double)agrandie2(i, j - l) * noyau[l + nbBords];
                    somme += noyau[l + nbBords];
                }
                agrandie(i, j) = (moy / somme);
            }
        // image out
        for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
            for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
            {
                out(i - nbBords, j - nbBords) = agrandie(i, j);
                if (out(i - nbBords, j - nbBords) < out.lireMin())
                    out.ecrireMin(out(i - nbBords, j - nbBords));
                if (out(i - nbBords, j - nbBords) > out.lireMax())
                    out.ecrireMax(out(i - nbBords, j - nbBords));
            }
    }

    return out;
}

/*
std::vector<CImageDouble> CImageDouble::pyramide(int hauteur, int tailleFiltre, double sigma)
{
    std::vector<CImageDouble> burt;

    // propagation des min,max du niveau initial aux autres niveaux, sinon va modifier la dynamique des niveaux
    burt.resize(hauteur);
    burt.at(0) = *this;
    burt.at(0).ecrireNom(this->lireNom() + std::to_string(0));

    for (int niv = 1; niv < hauteur; niv++)
    {
        // filtrage gaussien qui am�liore les r�sultats avant sous-�chantillonage
        CImageDouble niveau = burt.at(niv - 1).filtrage("gaussien", 5, 1);
        CImageDouble inter(niveau.lireHauteur() / 2, niveau.lireLargeur() / 2);

        inter.ecrireMax(this->lireMax()); // pour ne pas changer la dynamique entre niveaux
        inter.ecrireMin(this->lireMin());
        inter.ecrireNom(this->lireNom() + std::to_string(niv));

        for (int i = 0; i < inter.lireHauteur(); i++)
            for (int j = 0; j < inter.lireLargeur(); j++)
                inter(i, j) = (niveau(2 * i, 2 * j) + niveau(2 * i + 1, 2 * j) + niveau(2 * i, 2 * j + 1) + niveau(2 * i
+ 1, 2 * j + 1)) / 4;

        burt.at(niv) = inter;
    }

    return burt;
}
*/