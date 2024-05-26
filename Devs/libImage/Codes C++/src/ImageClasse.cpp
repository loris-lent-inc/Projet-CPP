#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "../include/ImageClasse.hpp"
#include "../include/ImageDouble.hpp"

#define MAGIC_NUMBER_BMP ('B' + ('M' << 8)) // signature bitmap windows

// constructeurs et destructeur
CImageClasse::CImageClasse()
{

    this->m_iHauteur   = 0;
    this->m_iLargeur   = 0;
    this->m_sNom       = "vide";
    this->m_lNbRegions = 0;
    this->m_pucPixel   = NULL;
}

CImageClasse::CImageClasse(int hauteur, int largeur)
{

    this->m_iHauteur   = hauteur;
    this->m_iLargeur   = largeur;
    this->m_sNom       = "inconnu";
    this->m_lNbRegions = 0;
    this->m_pucPixel   = new unsigned long[hauteur * largeur];
    for (int i = 0; i < this->lireNbPixels(); i++)
        this->m_pucPixel[i] = 0;
}

CImageClasse::CImageClasse(const CImageNdg & im, std::string choix)
{

    this->m_iHauteur   = im.lireHauteur();
    this->m_iLargeur   = im.lireLargeur();
    this->m_sNom       = im.lireNom() + "E";
    this->m_lNbRegions = 0;
    this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];

    CImageClasse withBords(im.lireHauteur() + 2, im.lireLargeur() + 2);
    int          i, ii, j, jj;

    for (i = 0, ii = 1; i < im.lireHauteur(); i++, ii++) // pure copie
        for (j = 0, jj = 1; j < im.lireLargeur(); j++, jj++)
            withBords(ii, jj) = (unsigned long)im(i, j);

    int * tableEtiquette = NULL;

    int   k              = 1;
    int   eps;
    int * predNonNul = new int[6];
    int   nbPredNonNuls;

    int nbMaxEtiquettes = im.lireHauteur() * im.lireLargeur();

    tableEtiquette      = new int[nbMaxEtiquettes];
    for (int kk = 0; kk < nbMaxEtiquettes; kk++)
    {
        tableEtiquette[kk] = kk;
    }

    if (choix.compare("V4") == 0)
    {
        for (i = 1; i < withBords.lireHauteur() - 1; i++)
            for (j = 1; j < withBords.lireLargeur() - 1; j++)
            {
                if (withBords(i, j) != 0)
                {
                    // V4
                    int pred1     = withBords(i, j - 1);
                    int pred3     = withBords(i - 1, j);

                    nbPredNonNuls = 0;
                    if (pred1)
                        predNonNul[nbPredNonNuls++] = pred1;
                    if (pred3)
                        predNonNul[nbPredNonNuls++] = pred3;

                    if (nbPredNonNuls == 0)
                    {
                        withBords(i, j) = k;
                        k++;
                    }
                    else
                    {
                        eps         = 0;
                        int minPred = tableEtiquette[predNonNul[0]];
                        int maxPred = tableEtiquette[predNonNul[0]];
                        while (eps < nbPredNonNuls)
                        {
                            if (tableEtiquette[predNonNul[eps]] < minPred)
                                minPred = tableEtiquette[predNonNul[eps]];
                            if (tableEtiquette[predNonNul[eps]] > maxPred)
                                maxPred = tableEtiquette[predNonNul[eps]];
                            eps++;
                        }
                        if (minPred == maxPred)
                            withBords(i, j) = minPred;
                        else
                        {
                            withBords(i, j) = minPred;
                            for (eps = 0; eps < nbPredNonNuls; eps++)
                            {
                                int beta = predNonNul[eps];
                                while (tableEtiquette[beta] != minPred)
                                {
                                    int m                = tableEtiquette[beta];
                                    tableEtiquette[beta] = minPred;
                                    beta                 = m;
                                }
                            }
                        }
                    }
                }
            }
    }
    else
    { // V8
        for (i = 1; i < withBords.lireHauteur() - 1; i++)
            for (j = 1; j < withBords.lireLargeur() - 1; j++)
            {
                if (withBords(i, j) != 0)
                {
                    // V8
                    int pred1     = withBords(i, j - 1);
                    int pred2     = withBords(i - 1, j - 1);
                    int pred3     = withBords(i - 1, j);
                    int pred4     = withBords(i - 1, j + 1);

                    nbPredNonNuls = 0;
                    if (pred1)
                        predNonNul[nbPredNonNuls++] = pred1;
                    if (pred2)
                        predNonNul[nbPredNonNuls++] = pred2;
                    if (pred3)
                        predNonNul[nbPredNonNuls++] = pred3;
                    if (pred4)
                        predNonNul[nbPredNonNuls++] = pred4;

                    if (nbPredNonNuls == 0)
                    {
                        withBords(i, j) = k;
                        k++;
                    }
                    else
                    {
                        eps         = 0;
                        int minPred = tableEtiquette[predNonNul[0]];
                        int maxPred = tableEtiquette[predNonNul[0]];
                        while (eps < nbPredNonNuls)
                        {
                            if (tableEtiquette[predNonNul[eps]] < minPred)
                                minPred = tableEtiquette[predNonNul[eps]];
                            if (tableEtiquette[predNonNul[eps]] > maxPred)
                                maxPred = tableEtiquette[predNonNul[eps]];
                            eps++;
                        }
                        if (minPred == maxPred)
                            withBords(i, j) = minPred;
                        else
                        {
                            withBords(i, j) = minPred;
                            for (eps = 0; eps < nbPredNonNuls; eps++)
                            {
                                int beta = predNonNul[eps];
                                while (tableEtiquette[beta] != minPred)
                                {
                                    int m                = tableEtiquette[beta];
                                    tableEtiquette[beta] = minPred;
                                    beta                 = m;
                                }
                            }
                        }
                    }
                }
            }
    }

    // actualisation de la table d'�quivalence
    for (int kk = 1; kk < k; kk++)
    {
        int m = kk;
        while (tableEtiquette[m] != m)
            m = tableEtiquette[m];
        tableEtiquette[kk] = m;
    }

    // on vire les trous histoire que nbObjets = max de l'image label
    int * etiquettes = new int[k + 1];
    for (int kk = 0; kk < k; kk++)
        etiquettes[kk] = 0;

    // histo pour rep�rer les trous
    for (int kk = 1; kk < k; kk++)
        etiquettes[tableEtiquette[kk]]++;

    // on remet � jour les index (etiquetage d�finitif hors trou)
    etiquettes[0] = 0;
    int compt     = 1;
    for (int kk = 1; kk < k; kk++)
    {
        if (etiquettes[kk])
            etiquettes[kk] = compt++;
    }

    for (i = 0; i < withBords.lireHauteur(); i++)
        for (j = 0; j < withBords.lireLargeur(); j++)
        {
            withBords(i, j) = etiquettes[tableEtiquette[withBords(i, j)]];
        }

    delete[] etiquettes;
    delete[] tableEtiquette;

    // cr�ation image lab�lis�e
    // suppression des 2 lignes et 2 colonnes artificiellement cr��es

    for (i = 0, ii = 1; i < this->lireHauteur(); i++, ii++)
        for (j = 0, jj = 1; j < this->lireLargeur(); j++, jj++)
            this->operator()(i, j) = withBords(ii, jj);

    m_lNbRegions = compt - 1;
    //std::cout << compt - 1 << " objets dans l'image..." << std::endl;
}

CImageClasse::CImageClasse(const CImageClasse & in, std::string choix, std::string voisinage)
{

    if (choix.compare("sans") == 0)
    { // simple copie

        this->m_iHauteur   = in.lireHauteur();
        this->m_iLargeur   = in.lireLargeur();
        this->m_sNom       = in.lireNom();
        this->m_lNbRegions = in.lireNbRegions();
        this->m_pucPixel   = new unsigned long[in.lireHauteur() * in.lireLargeur()];
        if (in.m_pucPixel != NULL)
            memcpy(this->m_pucPixel, in.m_pucPixel, in.lireNbPixels() * sizeof(unsigned long));
    }
    else
    {
        // d�tection cas objets/fond ou ND, pr�sence ou non d'une classe � 0

        bool objetsFond = false;
        int  pix        = 0;
        while ((pix < in.lireNbPixels()) && (!objetsFond))
        {
            if (in(pix) == 0)
                objetsFond = true;
            pix++;
        }

        if (objetsFond)
        { // cas pr�sence fond � 0

            CImageNdg im(in.lireHauteur(), in.lireLargeur());
            for (int i = 0; i < in.lireNbPixels(); i++)
                im(i) = (in(i) > 0) ? 1 : 0;
            CImageClasse out(im, voisinage);

            this->m_sNom       = out.lireNom() + "RE";
            this->m_lNbRegions = out.lireNbRegions();
            this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];

            memcpy(this->m_pucPixel, out.m_pucPixel, out.lireNbPixels() * sizeof(unsigned long));
        }
        else
        { // cas nu�es dynamiques donc r�-�tiquetage classe apr�s classe
            this->m_iHauteur   = in.lireHauteur();
            this->m_iLargeur   = in.lireLargeur();
            this->m_sNom       = in.lireNom() + "RE";
            this->m_lNbRegions = 0;

            this->m_pucPixel   = new unsigned long[in.lireHauteur() * in.lireLargeur()];
            for (int pix = 0; pix < this->lireNbPixels(); pix++)
                this->m_pucPixel[pix] = 0;

            for (int classe = 1; classe <= in.lireNbRegions(); classe++)
            {
                CImageNdg im(in.lireHauteur(), in.lireLargeur());

                for (int i = 0; i < in.lireNbPixels(); i++)
                    im(i) = (in(i) == static_cast<unsigned long>(classe)) ? 1 : 0;
                CImageClasse inter(im, voisinage);
                for (int pix = 0; pix < inter.lireNbPixels(); pix++)
                    if (inter(pix))
                        this->operator()(pix) = inter(pix) + this->lireNbRegions();

                this->ecrireNbRegions(this->lireNbRegions() + inter.lireNbRegions());
            }
            std::cout << this->lireNbRegions() << " nouvelles r�gions connexes dans l'image..." << std::endl;
        }
    }
}

CImageClasse::CImageClasse(const CImageNdg & im, int nbClusters, std::string choix)
{
    this->m_iHauteur   = im.lireHauteur();
    this->m_iLargeur   = im.lireLargeur();
    this->m_sNom       = im.lireNom() + "ND";
    this->m_lNbRegions = 0;
    this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];

    // contiendra la somme des elements de la classe
    // pour pouvoir calculer le centre de gravit� !
    std::vector<unsigned long> sommeClasses;

    // contiendra le nombre des elements de la classe
    // pour pouvoir calculer le centre de gravit� !
    std::vector<unsigned long> nbEltsClasses;

    // contiendra le centre de chaque classe
    std::vector<unsigned char> indexClasses;

    // contiendra la classe de chaque niveau de gris
    // imageFinale = lut[imageEntree]
    std::vector<int> lut;

    // allocation de k+1 classes
    // volont� d'affecter l'indice i � la classe i
    nbEltsClasses.resize(nbClusters + 1);
    for (int i = 1; i <= nbClusters; i++)
        nbEltsClasses[i] = 0;
    sommeClasses.resize(nbClusters + 1);
    for (int i = 1; i <= nbClusters; i++)
        sommeClasses[i] = 0L;

    // moyenne image et histo pour initialisation
    std::vector<unsigned long> h;
    h.resize(256, 0);
    for (int i = 0; i < this->lireNbPixels(); i++)
        h[im(i)] += 1L;
    float mean = 0;
    for (int i = 0; i < 256; i++)
        mean += h[i] * i;
    mean /= this->lireNbPixels();

    // nb de niveaux de gris dans image initiale
    // si k sup�rieur, alors on diminue le nb de classes en sortie : nbClasses
    int nbLevels = 0;
    for (int i = 0; i < 256; i++)
        if (h[i])
            nbLevels++;
    int nbClasses;
    nbClasses    = min(nbClusters, nbLevels);
    m_lNbRegions = nbClasses;

    if (strcmp(choix.c_str(), "aleatoire") == 0)
    { // tirage al�atoire des germes initiaux

        indexClasses.resize(nbClasses + 1);

        int num = 1;
        while (num <= nbClasses)
        {
            int  encours  = rand() % 256;
            bool dejaPris = false;
            for (int j = 1; j < num; j++)
                if (indexClasses[j] == encours)
                    dejaPris = true;
            if (dejaPris == false)
            {
                indexClasses[num] = encours;
                num++;
            }
        }
    }
    else if (strcmp(choix.c_str(), "regulier") == 0)
    { // intervalles r�guliers entre germes
        indexClasses.resize(nbClasses + 1);
        // recherche du min et du max
        int min = 0, max = 255;
        while (h[min] == 0)
            min++;
        while (h[max] == 0)
            max--;

        int num     = 1;
        int encours = min;
        int pas     = (max - min) / nbClasses;

        while (num <= nbClasses)
        {
            indexClasses[num] = encours;
            encours += pas;
            num++;
        }
    }

    std::cout << "classification en " << nbClasses << " classes..." << std::endl;

    lut.resize(256);
    int  nbTours = 0;
    bool change  = true;

    while ((!nbTours) || ((change) && (nbTours < MAX_ITER)))
    {
        // Remise � z�ro � chaque tour
        for (int i = 1; i <= nbClasses; i++)
        {
            nbEltsClasses[i] = 0;
            sommeClasses[i]  = 0L;
        }
        change = false;

        // Pour chaque point trouver son plus proche
        for (int i = 0; i < 256; i++)
        {
            unsigned char encours    = i;
            int           plusproche = 1;
            for (int j = 2; j <= nbClasses; j++)
            {
                if (EUCLIDEAN(encours, indexClasses[j]) < EUCLIDEAN(encours, indexClasses[plusproche]))
                    plusproche = j;
            }
            lut[i] = plusproche;
            nbEltsClasses[plusproche] += h[encours];
            sommeClasses[plusproche] += h[encours] * encours;
        }

        // On recalcule le centre de chaque classe
        // a partir des nouveaux germes.
        for (int j = 1; j <= nbClasses; j++)
        {
            if (nbEltsClasses[j])
                sommeClasses[j] /= nbEltsClasses[j];
            else
                sommeClasses[j] = (unsigned long)mean;
            if (EUCLIDEAN(indexClasses[j], sommeClasses[j]) > 0.001)
                change = true;
            indexClasses[j] = (unsigned char)sommeClasses[j];
        }
        nbTours++;
        std::cout << "Iteration : " << nbTours << std::endl;
    }

    // on remplace chaque pixel par sa classe
    for (int i = 0; i < this->lireNbPixels(); i++)
        this->operator()(i) = lut[im(i)];
}

CImageClasse::CImageClasse(const CImageCouleur & im, int nbClusters, std::string choix, int plan)
{

    this->m_iHauteur   = im.lireHauteur();
    this->m_iLargeur   = im.lireLargeur();
    this->m_sNom       = im.lireNom() + "ND";
    this->m_lNbRegions = 0;
    this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];

    // contiendra la somme des elements de la classe
    // pour pouvoir calculer le centre de gravit� !
    std::vector<unsigned long> sommeClasses;

    // contiendra le nombre des elements de la classe
    // pour pouvoir calculer le centre de gravit� !
    std::vector<unsigned long> nbEltsClasses;

    // contiendra le centre de chaque classe
    std::vector<unsigned char> indexClasses;

    // contiendra la classe de chaque niveau de gris
    // imageFinale = lut[imageEntree]
    std::vector<int> lut;

    // allocation de k+1 classes
    // volont� d'affecter l'indice i � la classe i
    nbEltsClasses.resize(nbClusters + 1);
    for (int i = 1; i <= nbClusters; i++)
        nbEltsClasses[i] = 0;
    sommeClasses.resize(nbClusters + 1);
    for (int i = 1; i <= nbClusters; i++)
        sommeClasses[i] = 0L;

    // extraction plan H pour optimisation bas�e histogramme

    CImageCouleur hsv = im.conversion("HSV");

    // moyenne sur plan Hue et histo pour initialisation
    std::vector<unsigned long> h;
    h.resize(256, 0);
    for (int i = 0; i < this->lireNbPixels(); i++)
        h[hsv(i)[plan]] += 1L;
    float mean = 0;
    for (int i = 0; i < 256; i++)
        mean += h[i] * i;
    mean /= this->lireNbPixels();

    // nb de niveaux de gris dans image initiale
    // si k sup�rieur, alors on diminue le nb de classes en sortie : nbClasses
    int nbLevels = 0;
    for (int i = 0; i < 256; i++)
        if (h[i])
            nbLevels++;
    int nbClasses;
    nbClasses    = min(nbClusters, nbLevels);
    m_lNbRegions = nbClasses;

    if (strcmp(choix.c_str(), "aleatoire") == 0)
    { // tirage al�atoire des germes initiaux
        srand((unsigned)time(NULL));
        indexClasses.resize(nbClasses + 1);

        int num = 1;
        while (num <= nbClasses)
        {
            int  encours  = rand() % 256;
            bool dejaPris = false;
            for (int j = 1; j < num; j++)
                if (indexClasses[j] == encours)
                    dejaPris = true;
            if (dejaPris == false)
            {
                indexClasses[num] = encours;
                num++;
            }
        }
    }
    else if (strcmp(choix.c_str(), "regulier") == 0)
    { // intervalles r�guliers entre germes
        indexClasses.resize(nbClasses + 1);
        // recherche du min et du max
        int min = 0, max = 255;
        while (h[min] == 0)
            min++;
        while (h[max] == 0)
            max--;

        int num     = 1;
        int encours = min;
        int pas     = (max - min) / nbClasses;

        while (num <= nbClasses)
        {
            indexClasses[num] = encours;
            encours += pas;
            num++;
        }
    }

    std::cout << "classification en " << nbClasses << " classes..." << std::endl;

    lut.resize(256);
    int  nbTours = 0;
    bool change  = true;

    while ((!nbTours) || ((change) && (nbTours < MAX_ITER)))
    {
        // Remise � z�ro � chaque tour
        for (int i = 1; i <= nbClasses; i++)
        {
            nbEltsClasses[i] = 0;
            sommeClasses[i]  = 0L;
        }
        change = false;

        // Pour chaque point trouver son plus proche
        for (int i = 0; i < 256; i++)
        {
            unsigned char encours    = i;
            int           plusproche = 1;
            for (int j = 2; j <= nbClasses; j++)
            {
                if (EUCLIDEAN(encours, indexClasses[j]) < EUCLIDEAN(encours, indexClasses[plusproche]))
                    plusproche = j;
            }
            lut[i] = plusproche;
            nbEltsClasses[plusproche] += h[encours];
            sommeClasses[plusproche] += h[encours] * encours;
        }

        // On recalcule le centre de chaque classe
        // a partir des nouveaux germes.
        for (int j = 1; j <= nbClasses; j++)
        {
            if (nbEltsClasses[j])
                sommeClasses[j] /= nbEltsClasses[j];
            else
                sommeClasses[j] = (unsigned long)mean;
            if (EUCLIDEAN(indexClasses[j], sommeClasses[j]) > 0.001)
                change = true;
            indexClasses[j] = (unsigned char)sommeClasses[j];
        }
        nbTours++;
        std::cout << "Iteration : " << nbTours << std::endl;
    }

    // on remplace chaque "niveau de Hue" par sa classe
    for (int i = 0; i < this->lireNbPixels(); i++)
        this->operator()(i) = lut[hsv(i)[plan]];
}

CImageClasse::CImageClasse(
    const CImageNdg & im, const std::vector<int> & germes,
    double tolerance) // croissance r�gion avec ensemble de germes sous la forme x0,y0,x1,y1,x2,y2 etc
{
    this->m_iHauteur   = im.lireHauteur();
    this->m_iLargeur   = im.lireLargeur();
    this->m_sNom       = im.lireNom() + "CR";
    this->m_lNbRegions = germes.size() / 2 + 1;
    this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];
    for (int i = 0; i < this->lireNbPixels(); i++)
        this->operator()(i) = 0;

    // structure PIXEL utilise pour empilements successifs
    typedef struct
    {
        int abs;
        int ord;
    } PIXEL;

    for (int cr = 0; cr < (int)germes.size(); cr += 2)
    {
        PIXEL germe;
        germe.abs = germes.at(cr) + 1;
        germe.ord = germes.at(cr + 1) + 1;

        CImageClasse growing(im.lireHauteur() + 2, im.lireLargeur() + 2); // suppression des tests pour les bords
        growing.ecrireNbRegions(1);

        CImageNdg img(im.lireHauteur() + 2, im.lireLargeur() + 2, 0);
        for (int i = 0; i < im.lireHauteur(); i++)
            for (int j = 0; j < im.lireLargeur(); j++)
                img(i + 1, j + 1) = im(i, j);

        // gestion des bords -> non nuls pour stopper la croissance
        for (int i = 0; i < growing.lireHauteur(); i++)
        {
            growing(i, 0)                     = ULONG_MAX;
            growing(i, img.lireLargeur() - 1) = ULONG_MAX;
        }
        for (int j = 0; j < growing.lireLargeur(); j++)
        {
            growing(0, j)                     = ULONG_MAX;
            growing(img.lireHauteur() - 1, j) = ULONG_MAX;
        }

        bool encore = true;

        int sInf    = int(im(germes.at(cr), germes.at(cr + 1)) - tolerance);
        int sSup    = int(im(germes.at(cr), germes.at(cr + 1)) + tolerance);

        std::stack<PIXEL> newMembers, voisins;

        newMembers.push(germe);
        int nbTours = 1;

        while (encore)
        {
            encore = false;
            while (!newMembers.empty())
            {
                PIXEL p = newMembers.top();
                newMembers.pop();
                growing(p.abs, p.ord) = growing(p.abs, p.ord) + 1;

                // test des 8 voisins de chaque "germe"
                PIXEL v1;
                v1.abs = p.abs - 1;
                v1.ord = p.ord - 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs - 1;
                v1.ord = p.ord;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                }

                v1.abs = p.abs - 1;
                v1.ord = p.ord + 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs;
                v1.ord = p.ord - 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs;
                v1.ord = p.ord + 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs + 1;
                v1.ord = p.ord - 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs + 1;
                v1.ord = p.ord;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }

                v1.abs = p.abs + 1;
                v1.ord = p.ord + 1;

                if ((growing(v1.abs, v1.ord) == 0) && (img(v1.abs, v1.ord) >= sInf) && (img(v1.abs, v1.ord) <= sSup))
                {
                    growing(v1.abs, v1.ord) = nbTours;
                    encore                  = true;
                    voisins.push(v1);
                }
            }
            nbTours += 1;

            if (encore)
            {
                newMembers.swap(voisins);
            }
        }

        std::cout << "Croissance stable apres " << nbTours << " iterations pour germe " << cr / 2 << " ..."
                  << std::endl;

        // on revient � la taille d'origine
        for (int i = 1; i < growing.lireHauteur() - 1; i++)
            for (int j = 1; j < growing.lireLargeur() - 1; j++)
                if ((growing(i, j) >= 1) && (this->operator()(i - 1, j - 1) == 0))
                    this->operator()(i - 1, j - 1) = cr / 2 + 1;
    }
    // fin de la croissance individuelle de chaque germe image avec 0,1,2 etc
}

CImageClasse::~CImageClasse()
{

    if (m_pucPixel)
    {
        delete[] m_pucPixel;
        m_pucPixel = NULL;
    }
}

CImageNdg CImageClasse::toNdg(const std::string& methode)
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
        int min = ULONG_MAX;
        int max = 0;

        for (int i = 0; i < this->lireNbPixels(); i++)
        {
            if (this->operator()(i) < min)
                min = this->operator()(i);
            if (this->operator()(i) > max)
                max = this->operator()(i);
        }

        double a = 255.f / (max - min);
        double b = -a * min;

        for (int i = 0; i < this->lireNbPixels(); i++)
            out(i) = (unsigned char)(a * this->operator()(i) + b);
    }

    return (out);
}

void CImageClasse::sauvegarde(const std::string & fixe)
{

    CImageNdg temp(this->lireHauteur(), this->lireLargeur());
    temp.choixPalette("binaire");
    temp.ecrireNom(this->lireNom());
    for (int i = 0; i < temp.lireNbPixels(); i++)
        if (m_pucPixel[i] >= 256) // cast pour rentrer sur un unsigned char
            temp(i) = (unsigned char)(m_pucPixel[i] % 255 + 1);
        else
            temp(i) = (unsigned char)(m_pucPixel[i]);
    temp.sauvegarde(fixe);
}

CImageClasse & CImageClasse::operator=(const CImageClasse & im)
{

    if (&im == this)
        return *this;

    this->m_iHauteur   = im.lireHauteur();
    this->m_iLargeur   = im.lireLargeur();
    this->m_sNom       = im.lireNom();
    this->m_lNbRegions = im.lireNbRegions();
    this->m_pucPixel   = new unsigned long[m_iHauteur * m_iLargeur];

    if (this->m_pucPixel)
        delete[] this->m_pucPixel;
    this->m_pucPixel = new unsigned long[im.lireHauteur() * im.lireLargeur()];

    if (im.m_pucPixel != NULL)
        memcpy(this->m_pucPixel, im.m_pucPixel, im.lireNbPixels() * sizeof(unsigned long));

    return *this;
}

_EXPORT_ CImageClasse & CImageClasse::operator-(const CImageClasse & im)
{
    for (int i = 0; i < this->lireNbPixels(); i++)
        if (im(i) > this->operator()(i))
            this->operator()(i) = 0;
        else
            this->operator()(i) -= im(i);
    return *this;
}

// signatures pour Image_Ndg et Image_Couleur
std::vector<SIGNATURE_Ndg> CImageClasse::signatures(const CImageNdg & img, bool enregistrementCSV)
{

    std::vector<SIGNATURE_Ndg> tab;

    if (this->lireNbRegions() > 0)
    {
        tab.resize(this->lireNbRegions() + 1); // gestion du fond �ventuel, cas des objets, vide si nu�es dynamiques

        for (int k = 0; k < (int)tab.size(); k++)
        {
            tab[k].moyenne = 0;
            tab[k].min     = 255;
            tab[k].max     = 0;
            tab[k].surface = 0;
        }

        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                tab[this->operator()(i, j)].moyenne += (double)img(i, j);
                if (tab[this->operator()(i, j)].min > img(i, j))
                    tab[this->operator()(i, j)].min = img(i, j);
                if (tab[this->operator()(i, j)].max < img(i, j))
                    tab[this->operator()(i, j)].max = img(i, j);
                tab[this->operator()(i, j)].surface += 1;
            }

        for (int k = 0; k < (int)tab.size(); k++)
            if (tab[k].surface > 0)
            {
                tab[k].moyenne /= tab[k].surface;
            }

        if (enregistrementCSV)
        {
            std::string   fichier = "../Res/" + this->lireNom() + "_SNdg.csv";
            std::ofstream f(fichier.c_str());

            if (!f.is_open())
                std::cout << "Impossible d'ouvrir le fichier en ecriture !" << std::endl;
            else
            {
                f << "Objet; Surface; Min; Max; Moyenne_Ndg " << std::endl;
                for (int k = 0; k < (int)tab.size(); k++)
                    f << k << ";" << tab[k].surface << " ; " << tab[k].min << " ; " << tab[k].max << " ; "
                      << tab[k].moyenne << std::endl;
            }
            f.close();
        }
    }

    return tab;
}

std::vector<SIGNATURE_Couleur> CImageClasse::signatures(const CImageCouleur & img, bool enregistrementCSV)
{

    std::vector<SIGNATURE_Couleur> tab;

    if (this->lireNbRegions() > 0)
    {
        tab.resize(this->lireNbRegions() + 1); // gestion de l'"objet" fond

        for (int k = 0; k < (int)tab.size(); k++)
        {
            tab[k].moyenne[0] = 0;
            tab[k].moyenne[1] = 0;
            tab[k].moyenne[2] = 0;
            tab[k].surface    = 0;
        }

        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                tab[this->operator()(i, j)].moyenne[0] += (double)img(i, j)[0];
                tab[this->operator()(i, j)].moyenne[1] += (double)img(i, j)[1];
                tab[this->operator()(i, j)].moyenne[2] += (double)img(i, j)[2];
                tab[this->operator()(i, j)].surface += 1;
            }

        for (int k = 0; k < (int)tab.size(); k++)
            if (tab[k].surface > 0)
            {
                tab[k].moyenne[0] /= tab[k].surface;
                tab[k].moyenne[1] /= tab[k].surface;
                tab[k].moyenne[2] /= tab[k].surface;
            }

        if (enregistrementCSV)
        {
            std::string   fichier = "../Res/" + this->lireNom() + "_SCouleur.csv";
            std::ofstream f(fichier.c_str());

            if (!f.is_open())
                std::cout << "Impossible d'ouvrir le fichier en ecriture !" << std::endl;
            else
            {
                f << "Objet; Surface; Moyenne_0; Moyenne_1; Moyenne_2 " << std::endl;
                for (int k = 0; k < (int)tab.size(); k++)
                    f << k << ";" << tab[k].surface << " ; " << tab[k].moyenne[0] << ";" << tab[k].moyenne[1] << " ; "
                      << tab[k].moyenne[2] << std::endl;
            }
            f.close();
        }
    }

    return tab;
}

// affichage
CImageCouleur CImageClasse::affichage(const std::vector<SIGNATURE_Ndg> & tab, int R, int G, int B)
{

    CImageCouleur out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom() + "_Contours");

    // mettre � jour

    CImageClasse contours(this->lireHauteur() + 2, this->lireLargeur() + 2);
    CImageClasse contoursC(this->lireHauteur() + 2, this->lireLargeur() + 2);
    for (int pix = 0; pix < contours.lireNbPixels(); pix++)
        contoursC(pix) = contours(pix) = ULONG_MAX;

    for (int i = 1; i < contours.lireHauteur() - 1; i++)
        for (int j = 1; j < contours.lireLargeur() - 1; j++)
            contoursC(i, j) = contours(i, j) = this->operator()(i - 1, j - 1);

    for (int i = 1; i < contours.lireHauteur() - 1; i++)
        for (int j = 1; j < contours.lireLargeur() - 1; j++)
        {
            int flag = 0;
            int k    = i - 1;
            while (k <= i + 1)
            {
                int l = j - 1;
                while (l <= j + 1)
                {
                    if (contours(k, l) != 0)
                        flag += 1;
                    l++;
                }
                k++;
            }
            if (flag == 9)
                contoursC(i, j) = 0;
        }

    for (int i = 1; i < contoursC.lireHauteur() - 1; i++)
        for (int j = 1; j < contoursC.lireLargeur() - 1; j++)
            if (contoursC(i, j) != 0)
            {
                out(i - 1, j - 1)[0] = R;
                out(i - 1, j - 1)[1] = G;
                out(i - 1, j - 1)[2] = B;
            }
            else
            {
                out(i - 1, j - 1)[0] = (int)tab[contours(i, j)].moyenne;
                out(i - 1, j - 1)[1] = (int)tab[contours(i, j)].moyenne;
                out(i - 1, j - 1)[2] = (int)tab[contours(i, j)].moyenne;
            }

    return out;
}

CImageCouleur CImageClasse::affichage(const std::vector<SIGNATURE_Couleur> & tab, const std::string & methode)
{

    CImageCouleur out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom());

    if (methode.compare("moyenne") == 0)
    {
        out.ecrireNom(this->lireNom() + "_Moyenne");

        for (int i = 0; i < this->lireNbPixels(); i++)
        {
            out(i)[0] = (int)tab[this->operator()(i)].moyenne[0];
            out(i)[1] = (int)tab[this->operator()(i)].moyenne[1];
            out(i)[2] = (int)tab[this->operator()(i)].moyenne[2];
        }
    }

    return out;
}

// r�gion par r�gion
CImageClasse CImageClasse::selection(const std::string & methode, int classe)
{

    CImageClasse out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom());
    out.ecrireNbRegions(this->lireNbRegions());

    if (methode.compare("selection") == 0)
    {
        std::stringstream convert;
        convert << classe;
        out.ecrireNom(this->lireNom() + "_Sel_" + convert.str());

        for (int i = 0; i < this->lireNbPixels(); i++)
            out(i) = (this->operator()(i) == static_cast<unsigned long>(classe)) ? classe : 0;
    }

    return out;
}

// filtrage selon crit�res comme taille
CImageClasse CImageClasse::filtrage(const std::string & methode, int taille, bool miseAJour)
{

    CImageClasse out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNbRegions(this->lireNbRegions());

    if (methode.compare("taille") == 0)
    {
        std::stringstream convert;
        convert << taille;
        out.ecrireNom(this->lireNom() + "_Surf_" + convert.str());

        std::vector<unsigned long> surface;
        surface.resize(this->lireNbRegions() + 1, 0);

        for (int i = 0; i < this->lireNbPixels(); i++)
            surface[this->operator()(i)] += 1;

        std::vector<unsigned long> lut;
        lut.resize(this->lireNbRegions() + 1);

        for (int k = 0; k < (int)lut.size(); k++)
            lut[k] = (surface[k] > (unsigned long)taille) ? k : 0;

        if (!miseAJour)
        {
            for (int i = 0; i < this->lireNbPixels(); i++)
                out(i) = lut[this->operator()(i)];
        }
        else
        {
            std::vector<unsigned long> renumerotation;
            renumerotation.resize(out.lireNbRegions() + 1, 0);

            int valEnCours = 1;
            for (int k = 0; k < (int)lut.size(); k++)
                if (lut[k])
                    renumerotation[k] = valEnCours++;
            out.ecrireNbRegions(valEnCours - 1);

            for (int i = 0; i < out.lireNbPixels(); i++)
                out(i) = renumerotation[this->operator()(i)];
        }
    }
    if (methode.compare("taille_inf") == 0)
    {
        std::stringstream convert;
        convert << taille;
        out.ecrireNom(this->lireNom() + "_Surf_" + convert.str());

        std::vector<unsigned long> surface;
        surface.resize(this->lireNbRegions() + 1, 0);

        for (int i = 0; i < this->lireNbPixels(); i++)
            surface[this->operator()(i)] += 1;

        std::vector<unsigned long> lut;
        lut.resize(this->lireNbRegions() + 1);

        for (int k = 0; k < (int)lut.size(); k++)
            lut[k] = (surface[k] < (unsigned long)taille) ? k : 0;

        if (!miseAJour)
        {
            for (int i = 0; i < this->lireNbPixels(); i++)
                out(i) = lut[this->operator()(i)];
        }
        else
        {
            std::vector<unsigned long> renumerotation;
            renumerotation.resize(out.lireNbRegions() + 1, 0);

            int valEnCours = 1;
            for (int k = 0; k < (int)lut.size(); k++)
                if (lut[k])
                    renumerotation[k] = valEnCours++;
            out.ecrireNbRegions(valEnCours - 1);

            for (int i = 0; i < out.lireNbPixels(); i++)
                out(i) = renumerotation[this->operator()(i)];
        }
    }
    else if (methode.compare("bords") == 0)
    {
        std::stringstream convert;
        convert << taille;
        out.ecrireNom(this->lireNom() + "_Bord_" + convert.str());

        // Calcul des bords, on assigne a un numero de composante connexe la valeur 1 si elle est un bord
        std::vector<unsigned long> bord;
        bord.resize(this->lireNbRegions() + 1, 0);

        for (int j = 0; j < this->lireLargeur(); j++)
        {
            bord[this->operator()(0, j)]                       = 1; // Haut
            bord[this->operator()(this->lireHauteur() - 1, j)] = 1; // Bas
        }
        for (int i = 0; i < this->lireHauteur(); i++)
        {
            bord[this->operator()(i, 0)]                       = 1; // Gauche
            bord[this->operator()(i, this->lireLargeur() - 1)] = 1; // Droite
        }

        std::vector<unsigned long> lut;
        lut.resize(this->lireNbRegions() + 1);
        for (int k = 0; k < (int)lut.size(); k++)
            // On parcourt la liste des bords, si une composante est à 1, elle est un bord et donc on met a jour la
            // valeur de la classe dans la lut
            lut[k] = (bord[k] != 1) ? k : 0;

        if (!miseAJour)
        {
            for (int i = 0; i < this->lireNbPixels(); i++)
                out(i) = lut[this->operator()(i)];
        }
        else
        {
            std::vector<unsigned long> renumerotation;
            renumerotation.resize(out.lireNbRegions() + 1, 0);

            int valEnCours = 1;
            for (int k = 0; k < (int)lut.size(); k++)
                if (lut[k])
                    renumerotation[k] = valEnCours++;
            out.ecrireNbRegions(valEnCours - 1);

            for (int i = 0; i < out.lireNbPixels(); i++)
                out(i) = renumerotation[this->operator()(i)];
        }
    }
    return out;
}

// signatures forme pour Image_Ndg et Image_Couleur
std::vector<SIGNATURE_Forme> CImageClasse::sigComposantesConnexes(bool enregistrementCSV) const
{

    std::vector<SIGNATURE_Forme> tab;

    if (this->lireNbRegions() > 0)
    {
        tab.resize(this->lireNbRegions() + 1); // gestion de l'"objet" fond

        for (int k = 0; k < (int)tab.size(); k++)
        {
            tab[k].centreGravite_i = 0;
            tab[k].centreGravite_j = 0;
            tab[k].surface         = 0;
            tab[k].premierPt_i     = -1;
            tab[k].premierPt_j     = -1;
            tab[k].codeFreeman     = "";
            tab[k].perimetre       = 0;
            tab[k].rectEnglob_Bi   = 0;
            tab[k].rectEnglob_Bj   = 0;
            tab[k].rectEnglob_Hi   = this->lireHauteur() - 1;
            tab[k].rectEnglob_Hj   = this->lireLargeur() - 1;
        }

        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                int pix = this->operator()(i, j);
                tab[pix].centreGravite_i += i;
                tab[pix].centreGravite_j += j;
                tab[pix].surface += 1;
                if (tab[pix].premierPt_i == -1)
                    tab[pix].premierPt_i = i;
                if (tab[pix].premierPt_j == -1)
                    tab[pix].premierPt_j = j;
                tab[pix].rectEnglob_Bi = max(tab[pix].rectEnglob_Bi, i);
                tab[pix].rectEnglob_Bj = max(tab[pix].rectEnglob_Bj, j);
                tab[pix].rectEnglob_Hi = min(tab[pix].rectEnglob_Hi, i);
                tab[pix].rectEnglob_Hj = min(tab[pix].rectEnglob_Hj, j);
            }

        for (int k = 0; k < (int)tab.size(); k++)
            if (tab[k].surface > 0)
            {
                tab[k].centreGravite_i /= tab[k].surface;
                tab[k].centreGravite_j /= tab[k].surface;
            }

        CImageClasse agrandie(this->lireHauteur() + 2,
                              this->lireLargeur() + 2); // bords a 0 pour bonne gestion des contours des objets
        agrandie.ecrireNbRegions(this->lireNbRegions());
        CImageClasse copie(this->lireHauteur(), this->lireLargeur());
        copie.ecrireNbRegions(this->lireNbRegions());

        // presence objets/bord ou ND
        std::vector<unsigned long> surface;
        surface.resize(this->lireNbRegions() + 1, 0);

        for (int i = 0; i < this->lireNbPixels(); i++)
            surface[this->operator()(i)] += 1;

        // gestion du coeur
        for (int i = 0; i < this->lireHauteur(); i++)
            for (int j = 0; j < this->lireLargeur(); j++)
            {
                agrandie(i + 1, j + 1) = this->operator()(i, j);
                copie(i, j)            = this->operator()(i, j);
            }

        if (surface[0] > 0)
        { // cas objets/fond
            for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
                for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
                    if (this->operator()(i - 1, j - 1) != 0)
                    {
                        unsigned long minH  = min(agrandie(i, j - 1), agrandie(i, j + 1));
                        unsigned long minV  = min(agrandie(i - 1, j), agrandie(i + 1, j));
                        unsigned long minV4 = min(minH, minV);
                        copie(i - 1, j - 1) = minV4;
                    }

            for (int pix = 0; pix < lireNbPixels(); pix++)
                copie(pix) = this->operator()(pix) - copie(pix);
        }
        else
        { // cas ND
            for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
                for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
                {
                    unsigned long minH  = min(agrandie(i, j - 1), agrandie(i, j + 1));
                    unsigned long maxH  = max(agrandie(i, j - 1), agrandie(i, j + 1));
                    unsigned long minV  = min(agrandie(i - 1, j), agrandie(i + 1, j));
                    unsigned long maxV  = max(agrandie(i - 1, j), agrandie(i + 1, j));
                    unsigned long minV4 = min(minH, minV);
                    unsigned long maxV4 = max(maxH, maxV);
                    if (!((agrandie(i, j) == minV4) && (agrandie(i, j) == maxV4))) // pixel diff�rent de ses voisins
                        copie(i - 1, j - 1) = 0;
                }

            for (int pix = 0; pix < lireNbPixels(); pix++)
                copie(pix) = this->operator()(pix) - copie(pix);
        }
        // 	copie -> image des bords int�rieurs des objets

        // parcours dans sens jusqu'� rejoindre point initial ou "stabilisation" au m�me point
        // vigilance -> adapt� sur contours normaux, sans "barbules" par exemple

        for (int num = 1; num <= this->lireNbRegions(); num++)
        {
            int pIniti  = tab[num].premierPt_i;
            int pInitj  = tab[num].premierPt_j;

            int pi      = pIniti;
            int pj      = pInitj;

            bool retour = false;

            while (!retour)
            {

                int pi_encours = pi;
                int pj_encours = pj;

                if ((pj != copie.lireLargeur() - 1) && (copie(pi, pj + 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "0";
                    copie(pi, pj + 1) = 0;
                    pj                = pj + 1;
                }
                else if ((pi != copie.lireHauteur() - 1) && (pj != copie.lireLargeur() - 1) &&
                         (copie(pi + 1, pj + 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "7";
                    copie(pi + 1, pj + 1) = 0;
                    pi                    = pi + 1;
                    pj                    = pj + 1;
                }
                else if ((pi != copie.lireHauteur() - 1) && (copie(pi + 1, pj) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "6";
                    copie(pi + 1, pj) = 0;
                    pi                = pi + 1;
                }
                else if ((pi != copie.lireHauteur() - 1) && (pj != 0) &&
                         (copie(pi + 1, pj - 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "5";
                    copie(pi + 1, pj - 1) = 0;
                    pi                    = pi + 1;
                    pj                    = pj - 1;
                }
                else if ((pj != 0) && (copie(pi, pj - 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "4";
                    copie(pi, pj - 1) = 0;
                    pj                = pj - 1;
                }
                else if ((pi != 0) && (pj != 0) && (copie(pi - 1, pj - 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "3";
                    copie(pi - 1, pj - 1) = 0;
                    pi                    = pi - 1;
                    pj                    = pj - 1;
                }
                else if ((pi != 0) && (copie(pi - 1, pj) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "2";
                    copie(pi - 1, pj) = 0;
                    pi                = pi - 1;
                }
                else if ((pi != 0) && (pj != copie.lireLargeur() - 1) &&
                         (copie(pi - 1, pj + 1) == static_cast<unsigned long>(num)))
                {
                    tab[num].codeFreeman += "1";
                    copie(pi - 1, pj + 1) = 0;
                    pi                    = pi - 1;
                    pj                    = pj + 1;
                }

                if ((((pi == pIniti) && (pj == pInitj))) || ((pi == pi_encours) && (pj == pj_encours)))
                    retour = true;
            }
        }


        // calcul du p�rim�tre
        for (int num = 1; num <= this->lireNbRegions(); num++)
        {
            for (int code = 0; code < (int)tab[num].codeFreeman.size(); code++)
            {
                if ((tab[num].codeFreeman[code] == '0') || (tab[num].codeFreeman[code] == '2') ||
                    (tab[num].codeFreeman[code] == '4') || (tab[num].codeFreeman[code] == '6'))
                    tab[num].perimetre += 1;
                else
                    tab[num].perimetre += (float)sqrt(2);
            }
            // Avec perimetre et surface :
            // compacité = perimetre^2 / 4 * pi * surface
            tab[num].compacite = (tab[num].perimetre * tab[num].perimetre) / (4 * PI * tab[num].surface);
        }

         


        // ACP
        for (int num = 1; num <= this->lireNbRegions(); num++)
        { // optimisable sans souci avec un seul parcours plut�t que num
            double somme_x, somme_y, somme_x2, somme_y2, somme_xy, n;

            double * x  = (double *)calloc(tab[num].surface, sizeof(double));
            double * y  = (double *)calloc(tab[num].surface, sizeof(double));
            int      nb = 0;

            somme_x     = 0.;
            somme_y     = 0.;
            somme_x2    = 0.;
            somme_y2    = 0.;
            somme_xy    = 0.;
            n           = 0;

            for (int i = tab[num].rectEnglob_Hi; i <= tab[num].rectEnglob_Bi; i++)
                for (int j = tab[num].rectEnglob_Hj; j <= tab[num].rectEnglob_Bj; j++)
                    if (this->operator()(i, j) == static_cast<unsigned long>(num))
                    {
                        x[nb] = i;
                        y[nb] = j;
                        somme_x += x[nb];
                        somme_y += y[nb];
                        somme_x2 += x[nb] * x[nb];
                        somme_y2 += y[nb] * y[nb];
                        somme_xy += x[nb] * y[nb];
                        nb += 1;
                        n += 1;
                    }

            free(x); /* vigilance en C */
            free(y);

            somme_x /= n;
            somme_y /= n;
            somme_x2 /= n;
            somme_y2 /= n;
            somme_xy /= n;

            double COVxx = somme_x2 - somme_x * somme_x;
            double COVyy = somme_y2 - somme_y * somme_y;
            double COVxy = somme_xy - somme_x * somme_y;

            // matrice de covariance
            // | a b |   | COVxx COVxy |
            // | c d | = | COVxy COVyy |
            double a = COVxx, b = COVxy, c = COVxy, d = COVyy;

            double T     = a + d;
            double Det   = a * d - b * c;

            // valeurs propres L1,L2 = racines du polyn�me caract�ristique
            double sqrtt = std::sqrt(T * T - 4 * Det);
            double L1    = 0.5 * (T + sqrtt);
            double L2    = 0.5 * (T - sqrtt);

            // vecteurs propres => A.v = L.v, soit:
            // (a-L).vx +     b.vy = 0
            //     c.vx + (d-L).vy = 0
            double V1x, V1y, V2x, V2y, EPSILON = 1E-8;
            if (std::abs(b) < EPSILON && std::abs(c) < EPSILON)
            {
                V1x = 1.0;
                V1y = 0.0;
                V2x = 0.0;
                V2y = 1.0;
            }
            else
            {
                V1x = b;
                V1y = L1 - a;
                V2x = b;
                V2y = L2 - a;
            }

            // signatures ACP
            tab[num].V1i     = V1x;
            tab[num].V1j     = V1y;
            tab[num].V2i     = V2x;
            tab[num].V2j     = V2y;
            tab[num].lambda1 = L1;
            tab[num].lambda2 = L2;
        }

        // enregistrement
        if (enregistrementCSV)
        {
            std::string   fichier = "res/" + this->lireNom() + "_SForme.csv";
            std::ofstream f(fichier.c_str());

            if (!f.is_open())
                std::cout << "Impossible d'ouvrir le fichier en ecriture !" << std::endl;
            else
            {
                f << "Objet; CG_i; CG_j; Surface; codeFreeman ; RE_Hi ; RE_Hj ; RE_Bi ; RE_Bj ; perimetre ; compacite ; lambda1 ; "
                     "lambda2 ; V1i ; V1j ; V2i ; V2j ;"
                  << std::endl;
                for (int k = 1; k < (int)tab.size(); k++)
                    f << k << ";" << tab[k].centreGravite_i << ";" << tab[k].centreGravite_j << ";" << tab[k].surface
                      << ";" << tab[k].codeFreeman << " ; " << tab[k].rectEnglob_Hi << " ; " << tab[k].rectEnglob_Hj
                      << " ; " << tab[k].rectEnglob_Bi << " ; " << tab[k].rectEnglob_Bj << " ; " << tab[k].perimetre
                      << " ; " << tab[k].compacite << " ; " << tab[k].lambda1 << " ; " << tab[k].lambda2 
                      << " ; " << tab[k].V1i << " ; "<< tab[k].V1j << " ; " << tab[k].V2i << " ; " << tab[k].V2j << " ; " << std::endl;
            }
            f.close();
        }
    }

    return tab;
}

CImageClasse CImageClasse::mutation(const CImageNdg & img)
{

    CImageClasse out(img.lireHauteur(), img.lireLargeur());
    out.ecrireNom(img.lireNom() + "M");
    out.ecrireNbRegions(1);

    for (int i = 0; i < img.lireNbPixels(); i++)
        out(i) = (img(i) > 0) ? 1UL : 0UL;

    return out;
}

CImageNdg CImageClasse::mutation(const std::string & methode) // ou "troncature" pour truncature au-dessus de 255
{

    CImageNdg out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom() + "T");
    out.choixPalette("binaire");

    if (methode.compare("troncature") == 0)
    {
        for (int i = 0; i < this->lireNbPixels(); i++)
            if (this->operator()(i) == 0UL)
                out(i) = 0;
            else if (this->operator()(i) < 256)
                out(i) = (unsigned char)this->operator()(i);
            else
                out(i) = (unsigned char)255;
    }

    return out;
}

// cellules de Voronoi
CImageClasse CImageClasse::voronoi()
{
    CImageClasse out(this->lireHauteur(), this->lireLargeur());
    out.ecrireNom(this->lireNom() + "CV");
    out.ecrireNbRegions(this->lireNbRegions());

    // structure PIXEL utilisee pour empilements successifs
    typedef struct s_pixel
    {
        int  abs;
        int  ord;
        long value;
    } t_pixel;
    std::stack<t_pixel> newMembers, voisins;

    CImageClasse regionGrowing(this->lireHauteur() + 2,
                               this->lireLargeur() + 2); // pour ajouter des bords non-nuls

    for (int i = 0; i < this->lireHauteur(); i++)
        for (int j = 0; j < this->lireLargeur(); j++)
            regionGrowing(i + 1, j + 1) = this->operator()(i, j);

    // gestion des bords -> non nuls pour stopper la croissance
    for (int i = 0; i < regionGrowing.lireHauteur(); i++)
    {
        regionGrowing(i, 0)                               = ULONG_MAX;
        regionGrowing(i, regionGrowing.lireLargeur() - 1) = ULONG_MAX;
    }
    for (int j = 0; j < regionGrowing.lireLargeur(); j++)
    {
        regionGrowing(0, j)                               = ULONG_MAX;
        regionGrowing(regionGrowing.lireHauteur() - 1, j) = ULONG_MAX;
    }

    // Germes des régions : tous les pixels non nuls, toutes les composantes connexes
    for (int i = 1; i < this->lireHauteur() - 1; i++)
        for (int j = 1; j < this->lireLargeur() - 1; j++)
            if (regionGrowing(i, j) != 0)
            {
                t_pixel pxl;
                pxl.abs   = i;
                pxl.ord   = j;
                pxl.value = regionGrowing(i, j);

                newMembers.push(pxl);
            }

    bool encore = true;
    while (encore)
    {
        encore = false;
        while (!newMembers.empty())
        {
            t_pixel p = newMembers.top();

            newMembers.pop();

            // test des 8 voisins de chaque "germe"
            t_pixel v1;
            v1.abs   = p.abs - 1;
            v1.ord   = p.ord - 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs - 1;
            v1.ord   = p.ord;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
				voisins.push(v1);
            }

            v1.abs   = p.abs - 1;
            v1.ord   = p.ord + 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs;
            v1.ord   = p.ord - 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs;
            v1.ord   = p.ord + 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs + 1;
            v1.ord   = p.ord - 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs + 1;
            v1.ord   = p.ord;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }

            v1.abs   = p.abs + 1;
            v1.ord   = p.ord + 1;
            v1.value = p.value;

            if (regionGrowing(v1.abs, v1.ord) == 0)
            {
                regionGrowing(v1.abs, v1.ord) = v1.value;
                encore                        = true;
                voisins.push(v1);
            }
        }

        if (encore)
        {
			// On ne prend plus que les nouveaux voisins, bords qu'on vient de créer
            newMembers.swap(voisins);
        }
    }

    // on revient a la taille d'origine
    for (int i = 1; i < regionGrowing.lireHauteur() - 1; i++)
        for (int j = 1; j < regionGrowing.lireLargeur() - 1; j++)
            out(i - 1, j - 1) = regionGrowing(i, j);

    // Afficher les composantes connexes sur l'image de retour
    CImageClasse resFin = out - *this;
    return resFin;
}

std::vector<SIGNATURE_Cellule> CImageClasse::sigCellules(const CImageNdg & img, bool enregistrementCSV)
{
    CImageClasse CV = this->voronoi();
    std::vector<SIGNATURE_Cellule> tab;

    if (this->lireNbRegions() > 0)
    {
        tab.resize(this->lireNbRegions() + 1); // gestion de l'"objet" fond

        auto sObj = new unsigned long long[this->lireNbRegions() + 1], sCell = new unsigned long long[this->lireNbRegions() + 1], ndgObj = new unsigned long long[this->lireNbRegions() + 1], ndgFond = new unsigned long long[this->lireNbRegions() + 1];

        for (int k = 0; k < (int)tab.size(); k++)
        {
           sObj[k] = 0;
           sCell[k] = 0;
           ndgObj[k] = 0;
           ndgFond[k] = 0;
        }

        for (int i = 0; i < this->lireNbPixels(); i++)
        {
            int pixSeg = this->operator()(i);
            int pixVor = CV(i);
            if (pixVor == 0) {
                sObj[pixSeg]++;
                sCell[pixSeg]++;
                ndgObj[pixSeg] += img(i);
            }
            else {
				sCell[pixVor]++;
				ndgFond[pixVor] += img(i);
			}
        }

        for (int k = 0; k < (int)tab.size(); k++)
        {
            ndgObj[k] = (sObj[k] > 0) ? ndgObj[k] / sObj[k] : 0;
            ndgFond[k] = (sCell[k] > 0) ? ndgFond[k] / sCell[k] : 0;

            tab[k].emergence = ndgObj[k] - ndgFond[k];
            tab[k].occupation = (sCell[k] > 0) ? (double)sObj[k] / (sCell[k]) : 0;
            tab[k].surface = sObj[k];
        }

        if (enregistrementCSV)
        {
            std::string   fichier = "res/" + this->lireNom() + "_SVoronoi.csv";
            std::ofstream f(fichier.c_str());

            if (!f.is_open())
                std::cout << "Impossible d'ouvrir le fichier en ecriture !" << std::endl;
            else
            {
                f << "Objet; Surface; emergence ; occupation" << std::endl;
                for (int k = 0; k < (int)tab.size(); k++)
                    f << k << ";" << tab[k].surface << ";" << tab[k].emergence << ";" << tab[k].occupation << std::endl;
            }
            f.close();
        }
    }

    return tab;
}

void CImageClasse::traceACP(const std::vector<SIGNATURE_Forme> & tab)
{
    // Parcourir toutes les composantes
    for (int num = 1; num <= this->lireNbRegions(); num++)
    {
        double coefDir1    = tab[num].V1j / tab[num].V1i;
        double coefDir2    = tab[num].V2j / tab[num].V2i;

        double ordOrigine1 = tab[num].centreGravite_j - coefDir1 * tab[num].centreGravite_i;
        double ordOrigine2 = tab[num].centreGravite_j - coefDir2 * tab[num].centreGravite_i;

        for (int i = tab[num].rectEnglob_Hi; i <= tab[num].rectEnglob_Bi; i++)
        {
            for (int j = tab[num].rectEnglob_Hj; j <= tab[num].rectEnglob_Bj; j++)
            {
                if (this->operator()(i, j) == static_cast<unsigned long>(num))
                {
                    int y1 = coefDir1 * i + ordOrigine1;
                    int y2 = coefDir2 * i + ordOrigine2;
                    if ((y1 == j) || (y2 == j))
                    {
                        this->operator()(i, j) = 0;
                    }
                }
            }
        }
        for (int i = tab[num].rectEnglob_Hi; i <= tab[num].rectEnglob_Bi; i++)
        {
            for (int j = tab[num].rectEnglob_Hj; j <= tab[num].rectEnglob_Bj; j++)
            {
                if (this->operator()(i, j) == static_cast<unsigned long>(num) && this->operator()(i, j) != 0)
                {
                    int x1 = (j - ordOrigine1) / coefDir1;
                    int x2 = (j - ordOrigine2) / coefDir2;
                    if ((x1 == i) || (x2 == i))
                    {
                        this->operator()(i, j) = 0;
                    }
                }
            }
        }
    }
}

void CImageClasse::cerclesComposantesConnexes(const std::vector<SIGNATURE_Forme> & tab)
{
    /// L'idee est de parcourir les contours des composantes connexes
    /// et de trouver les point le plus proche et le plus loin du centre de gravite
    /// et d'utiliser ces distances et le centre de gravité pour tracer les cercles

    CImageNdg temp = this->toNdg();
    //CImageNdg erod = temp.morphologie("erosion", "V8");
    //CImageNdg contours_interne = temp - erod;
    CImageNdg dilat = temp.morphologie("dilatation", elemStruct::V8());
    CImageNdg contours_externe = dilat - temp; // le coutour externe donne un rendu plus précis

    //contours_externe.sauvegarde("contours_externe");

    //Parcourir toutes les composantes
    for(int num = 1; num <= this->lireNbRegions(); num++)
    {
        int centerX = static_cast<int>(tab[num].centreGravite_i);
        int centerY = static_cast<int>(tab[num].centreGravite_j);

        double rayonMax_Sq = 0;
        double rayonMin_Sq = DBL_MAX;

        // Parcours le long du contour
        for (int i = tab[num].rectEnglob_Hi; i <= tab[num].rectEnglob_Bi; i++)
        {
            for (int j = tab[num].rectEnglob_Hj; j <= tab[num].rectEnglob_Bj; j++)
            {
                // Recherche des points les plus proches et les plus loins du centre de gravité
                if (contours_externe(i, j) != 0)
                {
                    double distance_Sq = pow(i - centerX, 2) + pow(j - centerY, 2);
                    rayonMax_Sq = max(rayonMax_Sq, distance_Sq);
                    rayonMin_Sq = min(rayonMin_Sq, distance_Sq);
                }
            }
        }

        // Tracer les cercles
        tracerCercle(centerX, centerY, static_cast<int>(sqrt(rayonMin_Sq)), this->lireNbRegions() + 1);
        tracerCercle(centerX, centerY, static_cast<int>(sqrt(rayonMax_Sq)), this->lireNbRegions() + 1);
    }
}

void CImageClasse::tracerCercle(int cx, int cy, int rayon, int couleur)
{
    const double pas_angle = PI / 180;

    for (double a = 0; a < 2 * PI; a += pas_angle)
    {
        int x = cx + rayon * cos(a);
        int y = cy + rayon * sin(a);

        if (x >= 0 && x < this->m_iHauteur && y >= 0 && y < this->m_iLargeur)
            this->operator()(x, y) = couleur;
    }
}

_EXPORT_ double CImageClasse::localIOU(const CImageClasse& ref, int x, int y, int w, int h) {
    double intersection = 0;
    double uni = 0;

    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (i < 0 || i >= this->lireHauteur() || j < 0 || j >= this->lireLargeur()) {
                continue;
            }
            if (this->operator()(i, j) && ref(i, j)) {
                intersection++;
            }
            if (this->operator()(i, j) || ref(i, j)) {
                uni++;
            }
        }
    }

    return (uni != 0) ? intersection / uni : 0;
}


_EXPORT_ double CImageClasse::Vinet(const CImageClasse& ref) {
    double surface = 0;
    double cumul = 0;

    auto loc_sig = this->sigComposantesConnexes(false);
    auto ref_sig = ref.sigComposantesConnexes(false);

    int min_nb = min(loc_sig.size(), ref_sig.size());

    for (int i = 1; i < min_nb; i++) {
        int left = min(loc_sig[i].rectEnglob_Hj, ref_sig[i].rectEnglob_Hj);
        int right = max(loc_sig[i].rectEnglob_Bj, ref_sig[i].rectEnglob_Bj);
        int top = min(loc_sig[i].rectEnglob_Hi, ref_sig[i].rectEnglob_Hi);
        int bottom = max(loc_sig[i].rectEnglob_Bi, ref_sig[i].rectEnglob_Bi);
        int h = bottom - top;
        int w = right - left;
		double iou = this->localIOU(ref, left, top, right - left, bottom - top);
        surface += (w) * (h);
        cumul += iou * w * h;
	}

    // On ajoute les surfaces des composantes connexes surdetectées
    for (int i = min_nb; i < loc_sig.size(); i++) {
		surface += loc_sig[i].surface;
	}

    // On ajoute les surfaces des composantes connexes manquées
    for (int i = min_nb; i < ref_sig.size(); i++) {
        surface += ref_sig[i].surface;
    }

    return cumul / surface;
}

