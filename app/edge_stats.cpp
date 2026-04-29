#include "SurfRemesh.h"
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

int main(int argc, char **argv)
{
    if( argc == 1) {
        cout << "Usage: " << argv[0] << " <inmeshfile> [bins]" << endl;
        return 1;
    }

    string infile = argv[1];
    int numBins = 20;
    if( argc > 2) {
        numBins = atoi(argv[2]);
    }

    SurfRemesh remesh;
    remesh.setMesh(infile);
    remesh.refine();

    auto lengths = remesh.getEdgeLengths();
    cout << "Edge length statistics:" << endl;
    cout << "  Min:    " << lengths[0] << endl;
    cout << "  Mean:   " << lengths[1] << endl;
    cout << "  Max:    " << lengths[2] << endl;

    auto nodes = remesh.getNodes();
    auto triangles = remesh.getTriangles();

    vector<double> edgeLengths;
    for( const auto& tri : triangles ) {
        Array3D p0 = nodes[tri[0]];
        Array3D p1 = nodes[tri[1]];
        Array3D p2 = nodes[tri[2]];
        edgeLengths.push_back(JMath::length(p0, p1));
        edgeLengths.push_back(JMath::length(p1, p2));
        edgeLengths.push_back(JMath::length(p2, p0));
    }

    sort(edgeLengths.begin(), edgeLengths.end());

    double minLen = edgeLengths.front();
    double maxLen = edgeLengths.back();
    double binWidth = (maxLen - minLen) / numBins;

    map<int, int> histogram;
    for( double len : edgeLengths ) {
        int bin = min((int)((len - minLen) / binWidth), numBins - 1);
        histogram[bin]++;
    }

    cout << "\nEdge length distribution (" << numBins << " bins):" << endl;
    cout << fixed << setprecision(4);
    for( int i = 0; i < numBins; i++ ) {
        double binStart = minLen + i * binWidth;
        double binEnd = binStart + binWidth;
        int count = histogram[i];
        double pct = 100.0 * count / edgeLengths.size();
        cout << "  [" << setw(8) << binStart << ", " << setw(8) << binEnd << "): "
             << setw(5) << count << " (" << setw(6) << pct << "%)" << endl;
        if( i == numBins - 1) break;
    }

    return 0;
}