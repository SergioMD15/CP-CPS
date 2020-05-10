using namespace std;
#include <iostream>

class BoxType
{
private:
    int width, length;

    void init(int l, int w)
    {
        width = w;
        length = l;
    }

public:
    void print()
    {
        cout << width << " " << length;
    }

    int get_width()
    {
        return width;
    }

    int get_length()
    {
        return length;
    }

    BoxType(int w, int l)
    {
        init(w, l);
    }

    BoxType()
    {
        init(0, 0);
    }
};