#include "src/BoxWrapping.cpp"
#include <fstream>
#include <iostream>

using namespace std;

pair<vector<BoxType>, int> read_file(string filename)
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

    vector<BoxType> boxes;
    int box_number, l, w;

    while (inFile >> box_number >> l >> w)
    {
        for (int i = 0; i < box_number; i++)
        {
            boxes.push_back(BoxType(l, w));
        }
    }
    return make_pair(boxes, width);
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            return 1;
        }
        string filename = argv[1];
        pair<vector<BoxType>, int> boxes = read_file(filename);
        BoxWrapping *bw = new BoxWrapping(boxes.first, boxes.second);
        bw->print();
        DFS<BoxWrapping> e(bw);
        // delete bw;
        bw = e.next();
        while (bw)
        {
            bw->print();
        }
        delete bw;
    }
    catch (Exception e)
    {
        cerr << "Gecode exception: " << e.what() << endl;
        return 1;
    }
    return 0;
}
