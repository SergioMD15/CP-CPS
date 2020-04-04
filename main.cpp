#include "src/headers/BoxWrapping.hpp"
#include <fstream>
#include <iostream>

using namespace std;

void print_boxes(vector<pair<int, int>> boxes)
{
    for (auto element : boxes)
    {
        cout << "(L,W): (" << element.first << "," << element.second << ")" << endl;
    }
}

vector<pair<int, int>> read_file(string filename)
{
    ifstream inFile;
    inFile.open(filename);
    if (!inFile)
    {
        cerr << "Unable to open file datafile.txt";
        exit(1);
    }
    int width, total_boxes;
    inFile >> width >> total_boxes;

    vector<pair<int, int>> boxes;
    int box_number, l, w;

    while (inFile >> box_number >> l >> w)
    {
        for (int i = 0; i < box_number; i++)
        {
            boxes.push_back(make_pair(l, w));
        }
    }
    return boxes;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 1;
    }
    string filename = argv[1];
    vector<pair<int, int>> boxes = read_file(filename);
    print_boxes(boxes);
    return 0;
}
