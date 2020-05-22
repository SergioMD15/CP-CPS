#include "BoxWrapping.cpp"
#include <fstream>
#include <iostream>

using namespace std;

pair<vector<BoxType>, int> read_file()
{
    int width, total_boxes;
    cin >> width >> total_boxes;

    vector<BoxType> boxes;
    int box_number, l, w;

    while (cin >> box_number >> w >> l)
    {
        for (int i = 0; i < box_number; i++)
        {
            boxes.push_back(BoxType(w, l));
        }
    }
    return make_pair(boxes, width);
}

int main(int argc, char *argv[])
{
    try
    {
        pair<vector<BoxType>, int> boxes = read_file();
        BoxWrapping *bw = new BoxWrapping(boxes.first, boxes.second);
        BAB<BoxWrapping> e(bw);
        bw = e.next();
        BoxWrapping *last = bw;
        while (bw)
        {
            last = bw;
            bw->print_debug();
            bw = e.next();
        }
        last->print();
        delete bw;
    }
    catch (Exception e)
    {
        cerr << "Gecode exception: " << e.what() << endl;
        return 1;
    }
    return 0;
}
