#include <string>
#include <vector>
#include <unordered_map>

#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include "BoxType.cpp"

using namespace Gecode;
using namespace std;

class BoxWrapping : public Space
{

private:
    BoolVarArray paper;
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
        cout << paper << endl;
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

    int get_max_length()
    {
        int result = 0;
        for (auto &&box : boxes)
        {
            result += max(box.get_length(), box.get_width());
        }
        return result;
    }

    int get_total_area_boxes()
    {
        int result = 0;
        for (auto &&box : boxes)
        {
            result += box.get_num_boxes() * box.get_length() * box.get_width();
        }
        return result;
    }

    /**
     * The number of positions occupied in the paper have to be exactly
     * the same as the sum of the sizes of the boxes.
     */
    void enforce_first_contraint()
    {
        rel(*this, sum(paper) == get_total_area_boxes());
    }

    /**
     * It is not possible that two different boxes share a common position in
     * the paper.
     */
    void enforce_second_contraint()
    {
    }

    void enforce_constraints()
    {
        enforce_first_contraint();
        enforce_second_contraint();
    }

    void init_cpp(vector<BoxType> &b, int &w)
    {
        boxes = b;
        paper_width = w;
        num_boxes = compute_num_boxes();
    }

    void init_gecode()
    {
        paper = BoolVarArray(*this, paper_width * get_max_length(), 0, 1);
    }

    void init(vector<BoxType> b, int w)
    {
        init_cpp(b, w);
        init_gecode();
    }

public:
    /**
     * Constructor for the class.
     */
    BoxWrapping(const vector<BoxType> &b, int &w)
    {
        init(b, w);
        enforce_constraints();

        branch(*this, paper, BOOL_VAR_NONE(), BOOL_VAL_MIN());
    }

    /**
     * Copy constructor for the class. Required by Gecode.
     */
    BoxWrapping(BoxWrapping &s) : Space(s)
    {
        boxes = s.boxes;
        paper_width = s.paper_width;
        num_boxes = s.num_boxes;
        paper.update(*this, s.paper);
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