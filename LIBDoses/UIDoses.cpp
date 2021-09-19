#include "UIDoses.h"

using namespace std;

typedef void(*basicFunc)(Object*);

static class Input_Information
{
public:
    //pointers
    bool* mouseL;
    bool* mouseR;
    bool* mouseM;
    int* mouseX;
    int* mouseY;

    //store current
    bool cmouseL = false;
    bool cmouseR = false;
    bool cmouseM = false;
    int cmouseX = 0;
    int cmouseY = 0;

    //store previous
    bool pmouseL = false;
    bool pmouseR = false;
    bool pmouseM = false;
    int pmouseX = 0;
    int pmouseY = 0;
}UID_Input;

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
    int main_color; //index to palette
    int sec_color;  //index to palette

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
    vector<basicFunc> ClickOn;
    vector<basicFunc> ClickOff;
    vector<basicFunc> HoverOn;
    vector<basicFunc> HoverOff;
    vector<basicFunc> HoldOn;
    vector<basicFunc> HoldOff;

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
        main_color = 0;
        sec_color = 1;

        //rectangle,elipse
        filled = true;
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

class UID
{
    const int maxX = 1920;
    const int maxY = 1080;

public:

    vector<Layer*> layer = { &Layer0 };
    vector<Palette*> palette = { &Palette0 };
    vector<Icon*> icon; // for list of icons
    Object** onTop = new Object* [maxX * maxY];
    Object* pOnTop = &Object0; //stores object that mouse was on last
    bool firstRender = true;

    unsigned int BGcolor = 0x202020;
    //void* fillfunc;
    UID()
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
        Object* o = onTop[i];
        if (o != &Object0) { return o; }
    }

    Object* Get_Obj(int x, int y, int width, int height)
    {
        int i = (y * maxX) + x;
        if (i < 0 || i > maxX * maxY) { return &Object0; }
        if (x < 0 || x > width) { return &Object0; }
        if (y < 0 || y > height) { return &Object0; }
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
        //HANDLE EVENT SYSTEM

        //write pointers to placeholders
        UID_Input.cmouseL = *UID_Input.mouseL;
        UID_Input.cmouseR = *UID_Input.mouseR;
        UID_Input.cmouseM = *UID_Input.mouseM;
        UID_Input.cmouseX = *UID_Input.mouseX;
        UID_Input.cmouseY = *UID_Input.mouseY;

        Object* cOnTop = Get_Obj(*UID_Input.mouseX, *UID_Input.mouseY, (int)mw, (int)mh); //get object that on under mouse

        if (!firstRender)//handle events if its not the first render
        {
            if (UID_Input.cmouseL && !UID_Input.pmouseL)//--------test click on
            {
                for (int i = 0; i < cOnTop->ClickOn.size(); i++)
                {
                    cOnTop->ClickOn[i](cOnTop);
                }
            }
            else if (cOnTop != pOnTop)//--------------------------test hover on
            {
                for (int i = 0; i < cOnTop->HoverOn.size(); i++)//execute hover on for new
                {
                    cOnTop->HoverOn[i](cOnTop);
                }
                for (int i = 0; i < pOnTop->HoverOff.size(); i++)
                {
                    pOnTop->HoverOff[i](pOnTop);
                }
            }
            else if (UID_Input.cmouseL && UID_Input.pmouseL)//-----test hold on
            {
                for (int i = 0; i < cOnTop->HoldOn.size(); i++)
                {
                    cOnTop->HoldOn[i](cOnTop);
                }
            }
        }
        else firstRender = false;

        //write place holder to previous value placeholders
        UID_Input.pmouseL = UID_Input.cmouseL;
        UID_Input.pmouseR = UID_Input.cmouseR;
        UID_Input.pmouseM = UID_Input.cmouseM;
        UID_Input.pmouseX = UID_Input.cmouseX;
        UID_Input.pmouseY = UID_Input.cmouseY;

        pOnTop = cOnTop;;//write object pointer to last on top

        //HANDLE RELATIONSHIPS

        //parent-child code coming soon

        //HANDLE RENDERING

        unsigned int* memptr = (unsigned int*)mem; // assign memory pointer

        for (int i = 0; i < mh * mw; i++) // writes background color to main buffer
        {
            *memptr++ = BGcolor;
        }

        for (int i = 0; i < maxX * maxY; i++) // clears on top array
        {
            onTop[i] = &Object0;
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
                int mColor = optr->palette->color[optr->main_color];//objects main color

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
                            *pixel++ = mColor;
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
    
    void addClickOn(basicFunc func) { ptr->ClickOn.push_back(func); }
    void addClickOff(basicFunc func) { ptr->ClickOff.push_back(func); }

    void addHoverOn(basicFunc func) { ptr->HoverOn.push_back(func); }
    void addHoverOff(basicFunc func) { ptr->HoverOff.push_back(func); }

    void addHoldOn(basicFunc func) { ptr->HoldOn.push_back(func); }
    void addHoldOff(basicFunc func) { ptr->HoldOff.push_back(func); }
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

//predefined functions

void followMouse(Object* o)
{
    o->posx += UID_Input.cmouseX - UID_Input.pmouseX;
    o->posy += UID_Input.cmouseY - UID_Input.pmouseY;
}

void incPal(Object * o)
{
    if (o->main_color < o->palette->color.size()-1) 
    {
        o->main_color++;
    }
    else
    {
        o->main_color = 0;
    }
}

void decPal(Object* o)
{
    if (o->main_color > 0)
    {
        o->main_color--;
    }
    else
    {
        o->main_color = o->palette->color.size()-1;
    }
}