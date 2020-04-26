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
    BoolVarArray x_relative, y_relative;
    IntVarArray coordinates;
    vector<BoxType> boxes;
    int paper_width, num_boxes, max_length;

    /*********** NO GECODE ***********/

    void print_input()
    {
        for (auto box : boxes)
        {
            box.print();
        }
    }

    void print_output()
    {
        cout << coordinates << endl;
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

    void init_cpp(vector<BoxType> &b, int &w)
    {
        boxes = b;
        paper_width = w;
        num_boxes = compute_num_boxes();
        max_length = get_max_length();
    }

    /*********** GECODE ***********/

    IntVar get_coordinate(int i, int j)
    {
        return coordinates[i * num_boxes + j];
    }

    IntVar get_x_coordinate(int j)
    {
        return get_coordinate(0, j);
    }

    IntVar get_y_coordinate(int j)
    {
        return get_coordinate(1, j);
    }

    BoolVar get_relative_coordinate(BoolVarArray &relative, int i, int j)
    {
        int cummulative = 0;
        for (int k = 0; k < i + 1; k++)
        {
            cummulative += 1;
        }
        return relative[i * num_boxes + j - cummulative];
    }

    BoolVar get_x_relative(int i, int j)
    {
        return get_relative_coordinate(x_relative, i, j);
    }

    BoolVar get_y_relative(int i, int j)
    {
        return get_relative_coordinate(y_relative, i, j);
    }

    void enforce_constraints()
    {
        for (int i = 0; i < num_boxes; i++)
        {
            IntVar x_1 = get_x_coordinate(i);
            IntVar y_1 = get_y_coordinate(i);

            rel(*this, x_1 + boxes[i].get_width() <= paper_width);
            rel(*this, y_1 + boxes[i].get_length() <= max_length);

            for (int j = i + 1; j < num_boxes; j++)
            {
                IntVar x_2 = get_x_coordinate(j);
                IntVar y_2 = get_y_coordinate(j);
                BoolVar x_rel = get_x_relative(i, j);
                BoolVar y_rel = get_y_relative(i, j);

                rel(*this, x_1 + boxes[i].get_width() <= x_2 + paper_width * (x_rel + y_rel));
                rel(*this, x_1 - boxes[j].get_width() >= x_2 - paper_width * (1 - x_rel + y_rel));
                rel(*this, y_1 + boxes[i].get_length() <= y_2 - max_length * (1 + x_rel - y_rel));
                rel(*this, y_1 - boxes[j].get_length() >= y_2 - max_length * (2 - x_rel - y_rel));
            }
        }
    }

    int get_relative_length()
    {
        int result = 0;
        for (int i = 1; i < num_boxes; i++)
        {
            result += i;
        }
        return result;
    }

    void init_gecode()
    {
        coordinates = IntVarArray(*this, num_boxes, 0, paper_width);
        x_relative = BoolVarArray(*this, get_relative_length(), 0, 1);
        y_relative = BoolVarArray(*this, get_relative_length(), 0, 1);
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
        branch(*this, coordinates, INT_VAR_NONE(), INT_VAL_MIN());
    }

    /**
     * Copy constructor for the class. Required by Gecode.
     */
    BoxWrapping(BoxWrapping &s) : Space(s)
    {
        boxes = s.boxes;
        paper_width = s.paper_width;
        num_boxes = s.num_boxes;
        max_length = s.max_length;

        coordinates.update(*this, s.coordinates);
        x_relative.update(*this, s.x_relative);
        y_relative.update(*this, s.y_relative);
    }

    virtual void constrain(const Space &b)
    {
        const BoxWrapping &bw = static_cast<const BoxWrapping &>(b);
        // rel(*this, get_paper_length() < bw.get_paper_length());
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