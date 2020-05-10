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
    BoolVarArray x_relative, y_relative, rotation;
    IntVarArray coordinates;
    vector<BoxType> boxes;
    vector<int> box_types;
    int paper_width, max_length;

    /*********** NO GECODE ***********/

    bool are_equal(BoxType a, BoxType b)
    {
        return (a.get_length() == b.get_length() &&
                a.get_width() == b.get_width());
    }

    void print_input()
    {
        cout << paper_width << " " << boxes.size() << endl;
        print_boxes();
    }

    void print_output()
    {
        cout << get_paper_length(coordinates) << endl;
        for (int i = 0; i < boxes.size(); i++)
        {
            cout << get_x_coordinate(i) << " " << get_y_coordinate(i) << endl;
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
                cout << box_types[index] << "  ";
                current.print();
                cout << endl;
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

    int get_total_area_boxes()
    {
        int result = 0;
        for (auto &&box : boxes)
        {
            result += boxes.size() * box.get_length() * box.get_width();
        }
        return result;
    }

    void init_cpp(vector<BoxType> &b, int &w)
    {
        boxes = b;
        paper_width = w;
        box_types = compute_box_types();
        max_length = max(paper_width, get_max_length());
    }

    void print_stats()
    {
        cout << "Paper width: " << paper_width << endl
             << "Max length: " << max_length << endl
             << "Number of boxes: " << boxes.size() << endl
             << "Types of boxes: " << endl
             << endl;
        print_boxes();
    }

    /*********** GECODE ***********/

    IntVar get_coordinate(int i, int j)
    {
        return coordinates[2 * i + j];
    }

    IntVar get_x_coordinate(int i)
    {
        return get_coordinate(i, 0);
    }

    IntVar get_y_coordinate(int i)
    {
        return get_coordinate(i, 1);
    }

    BoolVar get_relative_coordinate(BoolVarArray &relative, int i, int j)
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
            IntVar x_1 = get_x_coordinate(i);
            IntVar y_1 = get_y_coordinate(i);
            BoolVar z_i = rotation[i];
            int w_i = boxes[i].get_width();
            int l_i = boxes[i].get_length();

            for (int j = i + 1; j < boxes.size(); j++)
            {
                int w_j = boxes[j].get_width();
                int l_j = boxes[j].get_length();

                IntVar x_2 = get_x_coordinate(j);
                IntVar y_2 = get_y_coordinate(j);
                BoolVar x_rel = get_x_relative(i, j);
                BoolVar y_rel = get_y_relative(i, j);

                BoolVar z_j = rotation[j];

                rel(*this, x_1 + z_i * l_i + (1 - z_i) * w_i <= x_2 + max_length * (x_rel + y_rel));
                rel(*this, x_1 - z_j * l_j - (1 - z_j) * w_j >= x_2 - max_length * (1 - x_rel + y_rel));
                rel(*this, y_1 + z_i * w_i + (1 - z_i) * l_i <= y_2 + max_length * (1 + x_rel - y_rel));
                rel(*this, y_1 - z_j * w_j - (1 - z_j) * l_j >= y_2 - max_length * (2 - x_rel - y_rel));
            }
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

        coordinates = IntVarArray(*this, 2 * boxes.size(), 0, max_length);
        x_relative = BoolVarArray(*this, r_l, 0, 1);
        y_relative = BoolVarArray(*this, r_l, 0, 1);
        rotation = BoolVarArray(*this, boxes.size(), 0, 1);
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
        max_length = s.max_length;
        box_types = s.box_types;

        coordinates.update(*this, s.coordinates);
        x_relative.update(*this, s.x_relative);
        y_relative.update(*this, s.y_relative);
        rotation.update(*this, s.rotation);
    }

    IntVar get_paper_length(IntVarArray coords)
    {
        IntVar greatest = coords[0];
        for (int i = 0; i < boxes.size(); i++)
        {
            IntVar tmp = get_y_coordinate(i);
            if (tmp.assigned() && tmp.val() > greatest.val())
            {
                greatest = tmp;
            }
        }
        return greatest;
    }

    virtual void constrain(const Space &b)
    {
        const BoxWrapping &bw = static_cast<const BoxWrapping &>(b);
        // rel(*this, get_paper_length(coordinates) < get_paper_length(bw.coordinates));
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
        print_input();
        print_output();
    }
};