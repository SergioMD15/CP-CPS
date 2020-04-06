#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include <string>
#include <vector>
#include <unordered_map>

#include "BoxType.cpp"

using namespace Gecode;
using namespace std;

class BoxWrapping : public Space
{

private:
    vector<BoxType> boxes;
    int paper_width, num_boxes;

    void print_input()
    {
        for (auto box : boxes)
        {
            box.print();
        }
    }

    void print_output()
    {
    }

    int compute_num_boxes()
    {
        int n_boxes = 0;
        for (auto box : boxes)
        {
            n_boxes += box.get_num_boxes();
        }
        return n_boxes;
    }

public:
    /**
     * Constructor for the class.
     */
    BoxWrapping(vector<BoxType> b, int w)
    {
        boxes = b;
        paper_width = w;
        num_boxes = compute_num_boxes();
    }

    /**
     * Copy constructor for the class. Required by Gecode.
     */
    BoxWrapping(BoxWrapping &s)
    {
        boxes = s.boxes;
    }

    /**
     * Copy constructor for Space parent class. Required by Gecode.
     */
    virtual Space *copy()
    {
        return new BoxWrapping(*this);
    }

    void print()
    {
        cout << num_boxes << " " << paper_width << endl;
        print_input();
        print_output();
    }
};