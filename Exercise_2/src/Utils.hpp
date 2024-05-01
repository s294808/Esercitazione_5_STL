#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"
#include <cmath>


using namespace std;

///insieme di funzioni per l'importazione di diverse proprietà di una mesh poligonale da file CSV

namespace PolygonalLibrary{

// Import the polygonal mesh and test if the mesh is correct
bool ImportMesh(const string &filepath,
                PolygonalMesh& mesh);

// Import the Cell0D properties from Cell0Ds.csv file
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh);

// Import the Cell1D properties from Cell1Ds.csv file
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh);

// Import the Cell2D properties from Cell2Ds.csv file
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh);

bool TestLenghtEdges(vector<Vector2d>& coordinates0D,
                     const unsigned int& numId0D);

bool TestPolygonArea(const unsigned int& numId2D,
                     const vector<vector<unsigned int>>& vertices2D,
                     const vector<Vector2d>& coordinates0D);

}


