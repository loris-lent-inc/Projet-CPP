#ifndef _IMAGE_CLASSE_
#define _IMAGE_CLASSE_

#include <cmath> // For log2, exp, etc.
#include <algorithm>
#include <string>
#include <vector>

#include "ImageCouleur.hpp"
#include "ImageNdg.hpp"

#define EUCLIDEAN(a, b) ((a - b) * (a - b))
#define MAX_ITER 100

typedef struct
{
    double moyenne;
    int    min;
    int    max;
    int    surface;
} SIGNATURE_Ndg;

typedef struct
{
    double moyenne[3];
    int    surface;
} SIGNATURE_Couleur;

typedef struct
{
    float       centreGravite_i;
    float       centreGravite_j;
    int         surface;
    std::string codeFreeman; // au sens V8
    int         premierPt_i; // premier point rencontre sens de parcours avant
    int         premierPt_j;
    int         rectEnglob_Hi; // coins superieur gauche et inferieur droit
    int         rectEnglob_Hj;
    int         rectEnglob_Bi;
    int         rectEnglob_Bj;
    float       perimetre; // au sens V8
    float       compacite;

    double lambda1; // + grande valeur propre
    double lambda2; // + petite valeur propre
    double V1i;     // vecteur propre principal
    double V1j;
    double V2i; // vecteur propre secondaire
    double V2j;
} SIGNATURE_Forme;

typedef struct
{
    int    surface;
    double emergence;
    double occupation;
} SIGNATURE_Cellule;

// Define a structure for covariance matrix
typedef struct
{
    double ** data;
    int       rows;
    int       cols;
} MATRIX;

// definition classe Image Classe --> images etiquetees pour analyse objets, nuees dynamiques pour analyse regions

class CImageClasse
{
private:
    int         m_iHauteur;
    int         m_iLargeur;
    std::string m_sNom;
    long        m_lNbRegions;

    unsigned long * m_pucPixel;

public:
    // constructeurs
    _EXPORT_ CImageClasse();
    _EXPORT_ CImageClasse(int hauteur, int largeur);
    _EXPORT_ CImageClasse(const CImageNdg & im, std::string choix = "V8"); // objets
    _EXPORT_ CImageClasse(const CImageClasse & in, std::string choix = "sans",
                          std::string voisinage = "V8"); // re-etiquetage eventuel
    _EXPORT_ CImageClasse(const CImageNdg & im, int nbClusters = 2, std::string choix = "aleatoire"); // clustering
    _EXPORT_ CImageClasse(const CImageCouleur & im, int nbClusters = 2, std::string choix = "aleatoire",
                          int plan = 0); // plan dans HSV, non dans espace 3D
    _EXPORT_
    CImageClasse(const CImageNdg & im, const std::vector<int> & germes,
                 double tolerance = 5); // croissange region avec ensemble de germes sous la forme x0,y0,x1,y1,x2,y2 etc
    _EXPORT_ ~CImageClasse();           // destructeur

    // sauvegarde au format bmp
    // attention : cast des informations car pertes potentielles
    _EXPORT_ void sauvegarde(const std::string & fixe = ""); // sauvegarde data au format BMP avec cast des long en char

    // convertion en image Ndg
    _EXPORT_ CImageNdg toNdg(const std::string& methode = "defaut");

    // pouvoir acceder e un pixel par image(i)
    _EXPORT_ unsigned long & operator()(int i) const
    {
        return m_pucPixel[i];
    }

    // pouvoir acceder e un pixel par image(i,j)
    _EXPORT_ unsigned long & operator()(int i, int j) const
    {
        return m_pucPixel[i * m_iLargeur + j];
    }

    // operateur copie image par imOut = imIn
    _EXPORT_ CImageClasse & operator=(const CImageClasse & im);

    _EXPORT_ CImageClasse & operator-(const CImageClasse & im);

    // get et set
    _EXPORT_ int lireHauteur() const
    {
        return m_iHauteur;
    }

    _EXPORT_ int lireLargeur() const
    {
        return m_iLargeur;
    }

    _EXPORT_ int lireNbRegions() const
    {
        return m_lNbRegions;
    }

    _EXPORT_ std::string lireNom() const
    {
        return m_sNom;
    }

    _EXPORT_ int lireNbPixels() const
    {
        return m_iHauteur * m_iLargeur;
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

    _EXPORT_ void ecrireNbRegions(int nb)
    {
        m_lNbRegions = nb;
    }

    // signatures pour Image_Ndg et Image_Couleur
    _EXPORT_ std::vector<SIGNATURE_Ndg> signatures(const CImageNdg & img, bool enregistrementCSV = false);
    _EXPORT_ std::vector<SIGNATURE_Couleur> signatures(const CImageCouleur & img, bool enregistrementCSV = false);

    // affichage
    _EXPORT_ CImageCouleur affichage(const std::vector<SIGNATURE_Ndg> & tab, int R = 255, int G = 0, int B = 0);
    _EXPORT_ CImageCouleur affichage(const std::vector<SIGNATURE_Couleur> & tab,
                                     const std::string &                    methode = "moyenne");

    // selection
    _EXPORT_ CImageClasse selection(const std::string & methode = "selection", int classe = 0);

    // mutation : conversion depuis Ndg seuillee
    _EXPORT_ CImageClasse mutation(const CImageNdg & img);
    _EXPORT_ CImageNdg    mutation(const std::string & methode = "troncature"); // choix "troncature"

    // morphologie
    // methode options : "erosion", "dilatation"
    // element structurant options : "V4" ou "V8"
    _EXPORT_ CImageClasse morphologie(const std::string methode = "dilatation", const std::string eltStructurant = "V8");

    // filtrage selon criteres taille, bords, etc
    _EXPORT_ CImageClasse filtrage(const std::string & methode = "taille", int taille = 50, bool miseAJour = false);

    // signatures forme pour Image_Ndg et Image_Couleur
    _EXPORT_ std::vector<SIGNATURE_Forme> sigComposantesConnexes(bool enregistrementCSV = false) const;

    // cellules de Voronoi
    _EXPORT_ CImageClasse voronoi();
    _EXPORT_ std::vector<SIGNATURE_Cellule> sigCellules(const CImageNdg & img, bool enregistrementCSV = false);

    // Tracer axes principaux et secondaires
    _EXPORT_ void traceACP(const std::vector<SIGNATURE_Forme> & tab);

    // Tracer cercle dans chaque composante connexe
    _EXPORT_ void cerclesComposantesConnexes(const std::vector<SIGNATURE_Forme>& tab);
    _EXPORT_ void tracerCercle(int cx, int cy, int rayon, int couleur);

    _EXPORT_ double localIOU(const CImageClasse& ref, int x, int y, int w, int h);
    _EXPORT_ double localIOU(const CImageClasse& ref, const SIGNATURE_Forme& sig){return this->localIOU(ref, sig.rectEnglob_Hi, sig.rectEnglob_Hj, sig.rectEnglob_Bi - sig.rectEnglob_Hi, sig.rectEnglob_Bj - sig.rectEnglob_Hj);};
    _EXPORT_ double IOU(const CImageClasse& ref){return this->localIOU(ref, 0, 0, this->lireLargeur(), this->lireHauteur());};

    _EXPORT_ double Vinet(const CImageClasse& ref);

};

#endif