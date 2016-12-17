#include "iostream"
#include "math.h"
#include "opencv/cv.hpp"
#include "opencv/highgui.h"

#include <vector>
#include <string>

#include "haar.h"
#include "minE.h"
#include "betta.h"

using namespace std;

vector<double> AdaBoost(int NoI, int l, int m){
	vector<double> alfa = vector<double>();

	const int ValOfHaarSimples = 6;
	const int Iter = 1000;

	int q, boolean;

	double *Weights, *eps, *tmpEps;
	Weights = new double[NoI];
	eps = new double[NoI];
	tmpEps = new double[2];

	double sumW, minEps;

	int *Images;
	Images = new int[NoI];
	for(int i = 0; i < l; i++) Images[i] = 1;
	for(int i = 0; i < m; i++) Images[i + l] = 0;	
	
	int **HaarF;
	HaarF = new int*[ValOfHaarSimples];
	for(int j = 0; j < ValOfHaarSimples; j++) HaarF[j] = new int[NoI];
	vector<int> HaarVote = vector<int>();
	for(int i = 0; i < NoI; i++){
		for(int j = 0; j < ValOfHaarSimples; j++){
			HaarVote = haarFunc(i, 112, 92);
			HaarF[j][i] = HaarVote.at(j);
		}
		HaarVote.clear();
	}

	int **HaarFuncStrongest;
	HaarFuncStrongest = new int*[Iter];
	for(int w = 0; w < Iter; w++) HaarFuncStrongest[w] = new int[NoI];

/*
инициализируем веса для каждого изображения (пункт  2)
*/	
	for(int i = 0; i < NoI; i++){
		Weights[i] = 1;
		if (Images[i] == 0)
			Weights[i] = (1.0 / (2 * m));
		else
			Weights[i] = (1.0 / (2 * l));
	}
/*
пошёл обучающий цикл
*/
	for(int i = 0; i < Iter; i++){
		sumW = 0;
/*
нормирование весов
*/
		for(int k = 0; k < NoI; k++) sumW += Weights[k];
		for(int j = 0; j < NoI; j++) Weights[j] /= sumW;
/*
вычисляем ошибки
*/
		for(int j = 0; j < ValOfHaarSimples; j++){
			eps[j] = 0;
			for (int k = 0; k < NoI; k++)
				eps[j] += (Weights[k] * (fabs(HaarF[j][k] - Images[k])));
		}
/*
вычисляем самую маленькую ошибку
*/
		minEps = eps[0];
		q = 0;
		for(int j = 0; j < ValOfHaarSimples - 1; j++){
			tmpEps = minEpsln(minEps, eps[j + 1]);
			minEps = tmpEps[0];
			boolean = (int)tmpEps[1];
			if (boolean == 1)
				q = j + 1;
		}
/*
выбираем классификатор с наименьшей ошибкой
*/
		for(int j = 0; j < NoI; j++)
			HaarFuncStrongest[i][j] = HaarF[q][j];
/*
обновляем веса
*/
		for(int j = 0; j < NoI; j++){
			if (Images[j] == HaarFuncStrongest[i][j])
				Weights[j] *= Weights[j] * betta(minEps);
		}
		alfa.push_back(log(1.0 / betta(minEps))); //итоговые параметры альфа
	}
	return alfa;
}
