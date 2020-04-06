using namespace std;
#include <iostream>

class BoxType
{
private:
    int num_boxes, width, length;

    void init(int n, int l, int w)
    {
        num_boxes = n;
        width = w;
        length = l;
    }

public:
    void print()
    {
        cout << num_boxes << "   " << length << " " << width << endl;
    }

    int get_num_boxes()
    {
        return num_boxes;
    }

    int get_width()
    {
        return width;
    }

    int get_length()
    {
        return length;
    }

    BoxType(int n, int w, int l)
    {
        init(n, w, l);
    }
};