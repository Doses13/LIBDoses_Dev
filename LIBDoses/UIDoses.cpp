#include "UIDoses.h"
#include <vector>

using namespace std;

static class Input_Pointers
{
public:
    bool* mouseL;
    bool* mouseR;
    bool* mouseM;
}UID_Input;

static struct Functions
{

};

struct Palette
{
    vector<unsigned int> color;
    int id;

    Palette()
    {
        color = { 0x303030, 0xdddddd };
        id = 0;
    }

    Palette(vector<unsigned int> cols)
    {
        for (int i = 0; i < cols.size(); i++)
        {
            color.push_back(cols[i]);
            id = 0;
        }
    }
}Palette0;

struct Icon
{

};

enum Object_Type
{
    rectangle, curve, elipse, custom, image, scroll, text
};

struct Object
{
    // General
    int posx, posy;
    int width, height;
    int originx, originy;
    bool enabled, visible;

    // Inheritance
    int id, order, layer;
    Palette* palette;
    int main_color;
    int sec_color;

    Object* parent;
    vector<Object*> children;

    // Type Specific
    Object_Type type;
    // Used my multiple
    bool filled; //rect, elipse
    int line_weight; //rect, elipse, curve
    int hardness; //rect, elipse, curve

    // Rect
    int radius;

    // Elipse
    int foci1x;
    int foci1y;
    int foci2x;
    int foci2y;

    // Custom
    int* custom_draw;

    // Image


    // Pointers
    void* icon;

    //function
    void* onHover;
    void* onMoveAway;
    void* onClick;
    void* onClickOff;

    //constructors
    Object()
    {
        enabled = false;
        visible = false;
    }

    Object(int l, Object_Type t)//takes in layer and type
    {
        type = t;
        posx = 0;
        posy = 0;
        width = 0;
        height = 0;
        originx = 0;
        originy = 0;
        enabled = true;
        visible = true;
        palette = 0;
        main_color;
        sec_color;

        //rectangle,elipse
        filled = true;
        main_color = Palette0.color[0];
        sec_color = Palette0.color[1];
        line_weight = 1;
        hardness = 100;
    }
}Object0;

struct Layer
{
    vector<Object*> object; //Always stores ALL objects on the layer IN ORDER OF IDs and should not be re-ordered (objects removed when deleted)
    vector<Object*> order; //Stores ACTIVE objects in render order (objects removed when set to enabled is false)
    unsigned int* buffer;
    unsigned int w;
    unsigned int h;
    bool custom;
    bool refresh;
    int id;

    Layer()
    {
        w = 0;
        h = 0;
        custom = false;
        refresh = true;
    }
}Layer0;

static class UI
{
private:
    const int maxX = 1920;
    const int maxY = 1080;

    void Input_Calls()
    {

    }

public:
    vector<Layer*> layer = { &Layer0 };
    vector<Palette*> palette = { &Palette0 };
    vector<Icon*> icon; // for list of icons
    Object** onTop = new Object * [maxX * maxY];

    unsigned int BGcolor = 0x202020;
    //void* fillfunc;
    UI()
    {
        for (int i = 0; i < maxX * maxY; i++)
        {
            onTop[i] = &Object0;
        }
    }

    //defaults?

    int Add_Object(int l, Object_Type type)
    {
        Check_layer(l);
        Layer* lay = layer[l];
        lay->object.push_back(new Object(l, type));// create object (already sets layer and type)
        lay->order.push_back(lay->object.back());// push to order vector
        int id = lay->object.size() - 1;
        lay->object.back()->id = id; // give new object its object id
        return id;// returns id of new object
    }

    void Add_Layer()
    {
        layer.push_back(new Layer);
        layer.back()->id = layer.size();
    }

    void Check_layer(int l)
    {
        if (layer.size() <= l)
        {
            for (int i = 0; i <= l; i++)
            {
                Add_Layer();
            }
        }
    }

    Palette* Add_Palette(vector<unsigned int> cols)
    {
        palette.push_back(new Palette(cols));
        return palette.back();
    }

    Object* Get_Obj(int x, int y)
    {
        int i = (y * maxX) + x;
        if (i < 0 || i > maxX * maxY) { return &Object0; }
        //if (x < 0 || x > width) { return &Object0; }
        //if (y < 0 || y > height) { return &Object0; }
        Object* o = onTop[i];
        if (o != &Object0) { return o; }
    }

    bool Is_Pos(int x, int y, int width, int height)
    {
        int i = (y * maxX) + x;
        Object* o = onTop[i];
        if (x < 0 || x > width) { return false; }
        if (y < 0 || y > height) { return false; }
        if (o == &Object0) { return false; }
        else return true;
    }

    void Render(static void* mem, static unsigned int mh, static unsigned int mw) //takes in pointer to main buffer, its width and height
    {
        unsigned int* memptr = (unsigned int*)mem; // assign memory pointer

        for (int i = 0; i < mh * mw; i++) // writes background color to main buffer
        {
            *memptr++ = BGcolor;
        }

        for (int i = 0; i < layer.size(); i++)// for each layer ----------------------------------
        {
            // Does checks on layer buffer

            Layer* lptr = layer[i];
            unsigned int* pixel = lptr->buffer;

            bool ref = lptr->refresh;
            bool cus = lptr->custom;
            bool buf = lptr->buffer;
            unsigned int w = lptr->w;
            unsigned int h = lptr->h;

            if (buf) // has buffer
            {
                if (ref || w != mw || h != mh) // need to refresh (default case) (also goes if window was resized)
                {
                    free(lptr->buffer);
                    if (cus) // sets layers buffer to its custom size
                    {
                        lptr->buffer = (unsigned int*)malloc(w * h * sizeof(unsigned int));
                    }
                    else // sets layers buffer it size of main buffer
                    {
                        lptr->buffer = (unsigned int*)malloc(mw * mh * sizeof(unsigned int));
                        lptr->w = mw;
                        lptr->h = mh;
                        w = mw;
                        h = mh;
                    }
                }
            }
            else //does not have buffer
            {
                if (cus)//should have h and w already set
                {
                    lptr->buffer = (unsigned int*)malloc(w * h * sizeof(unsigned int));
                }
                else // if not custom it gives it the resolution of the main memory buffer
                {
                    lptr->buffer = (unsigned int*)malloc(mw * mh * sizeof(unsigned int));
                    lptr->w = mw;
                    lptr->h = mh;
                    w = mw;
                    h = mh;
                }
            }

            pixel = lptr->buffer;
            for (int j = 0; j < h * w; j++) // writes background color to main buffer
            {
                *pixel++ = 0xff000000;
            }

            // starts rendering of and object

            for (int j = 0; j < lptr->order.size(); j++)// for each object------------------------
            {
                pixel = lptr->buffer; // resets pointer position

                Object* optr = lptr->order[j];
                int oy = optr->posy;             //object y
                int ox = optr->posx;             //object x
                int oh = optr->height;           //object height
                int ow = optr->width;            //object width
                int c0 = optr->palette->color[0];//objects palette color 0

                // how far object goes over border
                int no = 0;
                int so = 0;
                int wo = 0;
                int eo = 0;
                no = (int)(oy < 0) * (-oy);
                so = (int)((oy + oh) > (int)h) * (oy + oh - (int)h);
                wo = (int)(ox < 0) * (-ox);
                eo = (int)((ox + ow) > (int)w) * (ox + ow - (int)w);

                switch (optr->type)
                {
                case rectangle:
                    pixel += ((int)w * (oy + no)) + ox + wo;//set pixel to start

                    for (int y = oy + no; y < oy + oh - so; y++)
                    {
                        for (int x = ox + wo; x < ow + ox - eo; x++)
                        {
                            *pixel++ = c0;
                            onTop[y * 1920 + x] = optr;
                        }
                        pixel += (int)w - ow + eo + wo;
                    }
                    break;
                case curve:
                    break;
                case elipse:
                    break;
                case custom:
                    break;
                case image:
                    break;
                case scroll:
                    break;
                case text:
                    break;
                default:
                    break;
                }
            }

            // post process

            // write to main memory buffer

            memptr = (unsigned int*)mem; // resets position of main memory pointer
            pixel = lptr->buffer; // resets position of layer memory pointer

            for (unsigned int j = 0; j < mw * mh; j++)
            {
                *memptr = (int)(*pixel != 0xff000000) * (*pixel) + (int)(*pixel == 0xff000000) * (*memptr);
                memptr++;
                pixel++;
            }

        }
    }

}UID;

class UID_Rect
{

public:
    int id;
    int layer;
    Object* ptr;
    Layer* lptr;

    // Functions
    void move(int x, int y)
    {
        ptr->posx += x;
        ptr->posy += y;
    }

    void setPos(int x, int y)
    {
        ptr->posx = x;
        ptr->posy = y;
    }

    void setColor(unsigned int color)
    {
        ptr->palette->color[0] = color;
    }
    void setColor(unsigned int color, int n)
    {
        ptr->palette->color[n] = color;
    }

    void setOnClick(void** func)
    {

    }
    // Constructors

    UID_Rect()//sets default everything
    {
        id = UID.Add_Object(0, rectangle);
        layer = 0;
        ptr = UID.layer[layer]->object[id];
    }

    UID_Rect(int l)//specify layer
    {
        id = UID.Add_Object(l, rectangle);
        layer = l;
        ptr = UID.layer[layer]->object[id];
    }

    UID_Rect(int l, int posx, int posy, int width, int height, Palette* palette, Object* parent)
    {
        id = UID.Add_Object(l, rectangle);
        layer = l;
        ptr = UID.layer[layer]->object[id];
        ptr->posx = posx;
        ptr->posy = posy;
        ptr->width = width;
        ptr->height = height;
        ptr->palette = palette;
        ptr->parent = parent;
    }

    UID_Rect(int l, int posx, int posy, int width, int height, bool filled, int line_weight, int hardness, int radius, Palette* palette, Object* parent)
    {
        id = UID.Add_Object(l, rectangle);
        layer = l;
        ptr = UID.layer[layer]->object[id];
        ptr->posx = posx;
        ptr->posy = posy;
        ptr->width = width;
        ptr->height = height;
        ptr->parent = parent;
        ptr->filled = filled;
        ptr->line_weight = line_weight;
        ptr->hardness = hardness;
        ptr->palette = palette;
        ptr->parent = parent;
    }
};

//preconfigured functions

static struct functionInfo
{

};

void FollowMouse()
{

}