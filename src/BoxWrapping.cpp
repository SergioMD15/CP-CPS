#include <string>
#include <vector>
#include <unordered_map>

#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

#include "BoxType.cpp"

using namespace Gecode;
using namespace std;

typedef vector<BoxType> VB;
typedef BoolVarArray BVA;
typedef IntVarArray IVA;

class BoxWrapping : public Space
{

private:
    BVA x_relative, y_relative, rotation;
    IVA x_coordinates, y_coordinates;
    VB boxes;
    IntVar paper_length;
    vector<int> number_of_boxes;
    int paper_width, max_length;

    /*********** NO GECODE ***********/

    bool are_equal(BoxType a, BoxType b)
    {
        return (a.get_length() == b.get_length() &&
                a.get_width() == b.get_width());
    }

    void print_input(bool debug)
    {
        if (debug)
        {
            cerr << paper_width << " " << boxes.size() << endl;
            print_debug_boxes();
        }
        else
        {
            cout << paper_width << " " << boxes.size() << endl;
            print_boxes();
        }
    }

    void print_output(bool debug)
    {
        if (debug)
        {
            cerr << paper_length.val() << endl;
        }
        else
        {
            cout << paper_length.val() << endl;
        }
        for (int i = 0; i < boxes.size(); i++)
        {
            int x = x_coordinates[i].val();
            int y = y_coordinates[i].val();
            if (debug)
            {
                cerr << x << " " << y << "  " << x + get_x_bounds_val(i) - 1
                     << " " << y + get_y_bounds_val(i) - 1 << endl;
            }
            else
            {
                cout << x << " " << y << "  " << x + get_x_bounds_val(i) - 1
                     << " " << y + get_y_bounds_val(i) - 1 << endl;
            }
        }
        if (debug)
        {
            cerr << endl;
        }
        else
        {

            cout << endl;
        }
    }

    void print_boxes()
    {
        BoxType current;
        int index = 0;
        for (BoxType box : boxes)
        {
            if (!are_equal(current, box))
            {
                current = box;
                cout << number_of_boxes[index] << "  ";
                current.print();
                cout << endl;
                index++;
            }
            continue;
        }
    }

    void print_debug_boxes()
    {
        BoxType current;
        int index = 0;
        for (BoxType box : boxes)
        {
            if (!are_equal(current, box))
            {
                current = box;
                cerr << number_of_boxes[index] << "  ";
                current.print_debug();
                cerr << endl;
                index++;
            }
            continue;
        }
    }

    vector<int> compute_box_types()
    {
        vector<int> types;
        int n_boxes = 0;
        BoxType b;
        int n = 0;
        for (auto box : boxes)
        {
            if (!are_equal(box, b))
            {
                types.push_back(n);
                b = box;
                n = 0;
            }
            n++;
        }
        types.push_back(n);
        types.erase(types.begin());
        return types;
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

    void init_cpp(vector<BoxType> &b, int &w)
    {
        boxes = b;
        paper_width = w;
        number_of_boxes = compute_box_types();
        max_length = max(paper_width, get_max_length());
    }

    /*********** GECODE ***********/

    BoolVar get_relative_coordinate(BVA &relative, int i, int j)
    {
        int cummulative = 0;
        for (int k = 1; k <= i + 1; k++)
        {
            cummulative += k;
        }
        return relative[i * boxes.size() + j - cummulative];
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
        for (int i = 0; i < boxes.size(); i++)
        {
            IntVar x_1 = x_coordinates[i];
            IntVar y_1 = y_coordinates[i];
            BoolVar z_1 = rotation[i];
            int w_1 = boxes[i].get_width();
            int l_1 = boxes[i].get_length();

            for (int j = i + 1; j < boxes.size(); j++)
            {
                int w_2 = boxes[j].get_width();
                int l_2 = boxes[j].get_length();
                BoolVar z_2 = rotation[j];

                IntVar x_2 = x_coordinates[j];
                IntVar y_2 = y_coordinates[j];
                BoolVar x_rel = get_x_relative(i, j);
                BoolVar y_rel = get_y_relative(i, j);

                BoolExpr e1 = x_1 + (z_1 * l_1) + ((1 - z_1) * w_1) <= x_2 + max_length * (x_rel + y_rel);
                BoolExpr e2 = x_1 - (z_2 * l_2) - ((1 - z_2) * w_2) >= x_2 - max_length * (1 - x_rel + y_rel);
                BoolExpr e3 = y_1 + (z_1 * w_1) + ((1 - z_1) * l_1) <= y_2 + max_length * (1 + x_rel - y_rel);
                BoolExpr e4 = y_1 - (z_2 * w_2) - ((1 - z_2) * l_2) >= y_2 - max_length * (2 - x_rel - y_rel);

                rel(*this, e1 && e2 && e3 && e4);
                rel(*this, (x_1 == x_2) >> (y_1 != y_2));
                rel(*this, (y_1 == y_2) >> (x_1 != x_2));
            }
            rel(*this, paper_length >= y_1 + get_y_bounds(i));
            rel(*this, paper_width >= x_1 + get_x_bounds(i));
        }
    }

    int get_relative_length()
    {
        int result = 0;
        for (int i = 1; i < boxes.size(); i++)
        {
            result += i;
        }
        return result;
    }

    void init_gecode()
    {
        int r_l = get_relative_length();

        x_coordinates = IVA(*this, boxes.size(), 0, paper_width);
        y_coordinates = IVA(*this, boxes.size(), 0, max_length);
        x_relative = BVA(*this, r_l, 0, 1);
        y_relative = BVA(*this, r_l, 0, 1);
        rotation = BVA(*this, boxes.size(), 0, 1);
        paper_length = IntVar(*this, 0, max_length);
    }

    void init(VB b, int w)
    {
        init_cpp(b, w);
        init_gecode();
    }

public:
    /**
     * Constructor for the class.
     */
    BoxWrapping(const VB &b, int &w)
    {
        init(b, w);
        enforce_constraints();
        branch(*this, x_coordinates, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
        branch(*this, y_coordinates, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
        branch(*this, rotation, BOOL_VAR_NONE(), BOOL_VAL_MIN());
        branch(*this, x_relative, BOOL_VAR_NONE(), BOOL_VAL_MIN());
        branch(*this, y_relative, BOOL_VAR_NONE(), BOOL_VAL_MIN());
        branch(*this, paper_length, INT_VAL_MIN());
    }

    LinIntExpr get_x_bounds(int i)
    {
        return rotation[i] * boxes[i].get_length() + ((1 - rotation[i]) * boxes[i].get_width());
    }

    LinIntExpr get_y_bounds(int i)
    {
        return rotation[i] * boxes[i].get_width() + ((1 - rotation[i]) * boxes[i].get_length());
    }

    int get_x_bounds_val(int i)
    {
        return rotation[i].val() * boxes[i].get_length() + ((1 - rotation[i].val()) * boxes[i].get_width());
    }

    int get_y_bounds_val(int i)
    {
        return rotation[i].val() * boxes[i].get_width() + ((1 - rotation[i].val()) * boxes[i].get_length());
    }

    /**
     * Copy constructor for the class. Required by Gecode.
     */
    BoxWrapping(BoxWrapping &s) : Space(s)
    {
        boxes = s.boxes;
        paper_width = s.paper_width;
        max_length = s.max_length;
        number_of_boxes = s.number_of_boxes;

        x_coordinates.update(*this, s.x_coordinates);
        y_coordinates.update(*this, s.y_coordinates);
        x_relative.update(*this, s.x_relative);
        y_relative.update(*this, s.y_relative);
        rotation.update(*this, s.rotation);
        paper_length.update(*this, s.paper_length);
    }

    IntVar get_paper_length()
    {
        return paper_length;
    }

    virtual void constrain(const Space &b)
    {
        const BoxWrapping &bw = static_cast<const BoxWrapping &>(b);
        rel(*this, paper_length < bw.paper_length);
    }

    /**
     * Copy constructor for Space parent class. Required by Gecode.
     */
    virtual Space *copy()
    {
        return new BoxWrapping(*this);
    }

    void print_debug()
    {
        print_input(true);
        print_output(true);
    }

    void print()
    {
        print_input(false);
        print_output(false);
    }
};
