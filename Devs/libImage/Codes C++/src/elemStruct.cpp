#include "../include/elemStruct.h"

elemStruct::elemStruct(int w, int h, std::string nom)
{
	m_iHauteur = h;
	m_iLargeur = w;
	m_pucPixel = new double[w*h];
	m_sNom = nom;
}

elemStruct::elemStruct(const elemStruct& source)
{
	m_sNom = source.m_sNom + "Copy";
	m_iHauteur = source.m_iHauteur;
	m_iLargeur = source.m_iLargeur;
	m_pucPixel = new double[m_iLargeur*m_iHauteur];
	for (int i = 0; i < m_iLargeur * m_iHauteur; i++)
	{
		m_pucPixel[i] = source.m_pucPixel[i];
	}
}

elemStruct::~elemStruct()
{
	delete[] m_pucPixel;
}

elemStruct elemStruct::V4(double val)
{
	elemStruct temp(3, 3, "V4");
	temp.m_pucPixel[0] = 0;
	temp.m_pucPixel[1] = val;
	temp.m_pucPixel[2] = 0;
	temp.m_pucPixel[3] = val;
	temp.m_pucPixel[4] = 0;
	temp.m_pucPixel[5] = val;
	temp.m_pucPixel[6] = 0;
	temp.m_pucPixel[7] = val;
	temp.m_pucPixel[8] = 0;
	return temp;
}

elemStruct elemStruct::V8(double val)
{
	elemStruct temp(3, 3, "V8");
	temp.m_pucPixel[0] = val;
	temp.m_pucPixel[1] = val;
	temp.m_pucPixel[2] = val;
	temp.m_pucPixel[3] = val;
	temp.m_pucPixel[4] = 0;
	temp.m_pucPixel[5] = val;
	temp.m_pucPixel[6] = val;
	temp.m_pucPixel[7] = val;
	temp.m_pucPixel[8] = val;
	return temp;
}

elemStruct elemStruct::rectangle(int x1, int y1, int x2, int y2, double val)
{
	int w = x2 - x1 + 1;
	int h = y2 - y1 + 1;
	std::string nom = "R" + std::to_string(w) + "x" + std::to_string(h);
	elemStruct temp(w, h, nom);
	for (int i = 0; i < temp.m_iLargeur; i++)
	{
		for (int j = 0; j < temp.m_iHauteur; j++)
		{
			temp.m_pucPixel[i + j * temp.m_iLargeur] = val;
		}
	}
	return temp;
}

elemStruct elemStruct::disque(int r, double val)
{
	std::string nom = "D" + std::to_string(2 * r + 1);
	elemStruct temp(2 * r + 1, 2 * r + 1, nom);
	for (int i = 0; i < temp.m_iLargeur; i++)
	{
		for (int j = 0; j < temp.m_iHauteur; j++)
		{
			if ((i - r) * (i - r) + (j - r) * (j - r) <= r * r)
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = val;
			}
			else
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = 0;
			}
		}
	}
	return temp;
}

elemStruct elemStruct::croix(int r, double val)
{
	std::string nom = "C" + std::to_string(2 * r + 1);
	elemStruct temp(2 * r + 1, 2 * r + 1, nom);
	for (int i = 0; i < temp.m_iLargeur; i++)
	{
		for (int j = 0; j < temp.m_iHauteur; j++)
		{
			if (i == r || j == r)
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = val;
			}
			else
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = 0;
			}
		}
	}
	return temp;
}

elemStruct elemStruct::etoile(int r, double val)
{
	std::string nom = "S" + std::to_string(2 * r + 1);
	elemStruct temp(2 * r + 1, 2 * r + 1, nom);
	for (int i = 0; i < temp.m_iLargeur; i++)
	{
		for (int j = 0; j < temp.m_iHauteur; j++)
		{
			if (abs(i-r)+abs(j-r) < r)
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = val;
			}
			else
			{
				temp.m_pucPixel[i + j * temp.m_iLargeur] = 0;
			}
		}
	}
	return temp;
}

elemStruct elemStruct::gaussien(int r, double val)
{
	std::string nom = "G" + std::to_string(2 * r + 1);
	elemStruct temp(2 * r + 1, 2 * r + 1, nom);
	for (int i = 0; i < temp.m_iLargeur; i++)
	{
		for (int j = 0; j < temp.m_iHauteur; j++)
		{
			temp.m_pucPixel[i + j * temp.m_iLargeur] = val * exp(-((i - r) * (i - r) + (j - r) * (j - r)) / (2 * r * r));
		}
	}
	return temp;
}

int elemStruct::lireHauteur() const
{
	return m_iHauteur;
}

int elemStruct::lireLargeur() const
{
	return m_iLargeur;
}

std::string elemStruct::lireNom() const
{
	return m_sNom;
}

double elemStruct::lirePixel(int x, int y) const
{
	return m_pucPixel[x + y * m_iLargeur];
}

double elemStruct::operator()(int x, int y) const
{
	return m_pucPixel[x + y * m_iLargeur];
}