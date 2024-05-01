#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>


namespace PolygonalLibrary{

bool ImportMesh(const string &filepath,
                PolygonalMesh& mesh)
{
    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv", mesh))
    {
        return false;
    }else{

        //STAMPA MARKER DELLE CELLE0D
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:"; //restituisce la chiave dell'elemento corrente della mappa, che rappresenta il marker della cella

            for(const unsigned int id: it -> second)
            {
                cout << "\t" << id;
            }

            cout << endl;
        }

    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv", mesh))
    {
        return false;
    }else{

        //STAMPA MARKER DELLE CELLE1D
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }

        return true;
    }

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv", mesh))
    {
        return false;
    }else{

        // test che verifica se la mesh è corretta controllando che gli edge della cella 2D siano correttamente definiti nei vertici della mesh
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> edges = mesh.Cell2DEdges[c];

            for(unsigned int e = 0; e < mesh.Cell2DEdges[c].size(); e++)
            {
                const unsigned int origin = mesh.Cell1DVertices[edges[e]][0];
                const unsigned int end = mesh.Cell1DVertices[edges[e]][1];

                auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);
                if(findOrigin == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 2;
                }

                auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
                if(findEnd == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 3;
                }

            }
        }

    }

    if(!TestLenghtEdges(mesh.Cell0DCoordinates, mesh.NumberCell0D))
    {
        return false;
    }else{
        cout << "Le lunghezze dei lati sono diverse da 0"<<endl;
    }

    if(!TestPolygonArea(mesh.NumberCell2D,
                         mesh.Cell2DVertices,
                         mesh.Cell0DCoordinates))
    {
        return false;
    }else{
        cout << "Le lunghezze dei lati sono diverse da 0"<<endl;
    }

    return true;
}

bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2d coord;

        char separator;

        converter >> id >> separator >> marker >> separator >> coord(0) >> separator >> coord(1);

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);

        if( marker != 0)
        {
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)-> second.push_back(id);
        }

    }
    file.close();
    return true;
}

bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line); //aggiunge tutte le linee del file alla lista listLines

    listLines.pop_front(); //elimina la prima riga non contenente dati utili

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    //alloca memoria per l'array Cell1DId in modo che possa contenere almeno mesh.NumberCell0D elementi senza dover ridimensionare l'array durante l'aggiunta degli elementi successivi
    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;

        char separator;

        converter >> id >> separator >> marker >> separator >> vertices(0) >> separator >> vertices(1);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            //inserisce un valore nella mappa Cell1DMarkers della mesh
            //Se il valore marker non esiste già come chiave nella mappa, viene inserito insieme all'id della cella corrente
            //Se il valore marker esiste già come chiave nella mappa, l'id della cella corrente viene aggiunto alla lista di valori associati a quella chiave
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    file.close();

    return true;

}

bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;

    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front(); //elimina la prima riga dove non ci sono dati

    mesh.NumberCell2D = listLines.size(); //otteniamo il numero di celle 2D dal numero di righe presenti nel file

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int numVertices;
        unsigned int numEdges;
        unsigned int marker;
        char separator;

        converter >> id >> separator >> marker >> separator >> numVertices >> separator ;

        vector<unsigned int> vertices(numVertices);

        for(unsigned int i = 0; i < numVertices; i++)
        {
            converter >> vertices[i] >> separator;
        }

        converter >> numEdges >> separator;

        vector<unsigned int> edges(numEdges);

        for(unsigned int i = 0; i < numEdges; i++)
        {
            converter >> edges[i] >> separator;
        }

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);

    }

    file.close();

    return true;

}

bool TestLenghtEdges(vector<Vector2d>& coordinates0D,
                     const unsigned int& numId0D){
    cout << "Test Length Edges:" << endl;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = 0;

    for (i = 0; i < numId0D + 1; i++)
    {
        const double x1 = coordinates0D[i][0];
        const double y1 = coordinates0D[i][1];

        for (k = i + 1; k < numId0D; k++)
        {
            const double x2 = coordinates0D[k][0];
            const double y2 = coordinates0D[k][1];

            double lX = pow(x1 - x2, 2);
            double lY = pow(y1 - y2, 2);
            double tot = sqrt(lX + lY);

            if (tot == 0)
            {
                j += 1;
                cout << "tra i vertici " << i << " e " << k << "la lunghezza è zero" << endl;
            }
        }
    }
    if (j == numId0D){
        return false;
    } else {
        return true;
    }
}

bool TestPolygonArea(const unsigned int& numId2D,
                     const vector<vector<unsigned int>>& vertices2D,
                     const vector<Vector2d>& coordinates0D)
{
    vector<unsigned int> numVertices2D;
    for (const auto& cellVertices : vertices2D) {
        numVertices2D.push_back(cellVertices.size());
    }

    double Area = 0.0;
    int Count = 0;

    for (int j = 0; j < numId2D; ++j)
    {
        for (int i = 0; i < numVertices2D[j]; ++i)
        {
            int v1 = vertices2D[j][i];
            int nextIndex = i + 1;
            if (nextIndex == numVertices2D[j])
                nextIndex = 0;
            int v2 = vertices2D[j][nextIndex];
            Area += (coordinates0D[v1][0] * coordinates0D[v2][1] - coordinates0D[v2][0] * coordinates0D[v1][1]) /2.0;
        }

        if (abs(Area) == 0)
        {
            Count += 1;
        }

        Area = 0.0;
    }

    if (Count == 0){
        return true;
    }else{
        return false;
    }

}

}

