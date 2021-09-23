#define UID_WHITE  0xffffff
#define UID_75GREY 0xbfbfbf
#define UID_50GREY 0x7f7f7f
#define UID_25GREY 0x3f3f3f
#define UID_BLACK  0x000000
#define UID_BLUE   0x0000ff
#define UID_CYAN   0x00ffff
#define UID_GREEN  0x00ff00
#define UID_YELLOW 0xffff00
#define UID_ORANGE 0xff7f00
#define UID_RED    0xff0000
#define UID_PURPLE 0xff00ff
#define _25P      +0x3f000000
#define _50P      +0x7f000000
#define _75P      +0xbf000000
#define _100P     +0xff000000

#define PI 3.14159265
#define toDeg *180/PI
#define toRad *PI/180

#include <vector>
#include <math.h>

struct Layer;

struct Object;

typedef void(*basicFunc)(Object*);

typedef struct vector2
{
    int x, y;
};

int clamp(int min, int max, int val);

//end of header file

using namespace std;

//storage classes/structs

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

struct UID_RGB
{
    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int A;

    UID_RGB(unsigned int r, unsigned int g, unsigned int b)
    {
        R = r % 255;
        G = g % 255;
        B = b % 255;
    }
    
    UID_RGB(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
    {
        R = r % 255;
        G = g % 255;
        B = b % 255;
        A = a % 255;
    }
};

unsigned int rgbToInt(UID_RGB c)//converts rgb(a) values to an int
{
    return c.A * 16777216 + c.R * 65536 + c.G * 256 + c.B;
}

UID_RGB intToRgb(unsigned int c)//converts int to rgb(a)
{
    unsigned int a = c / 16777216;
    c -= c % 16777216;
    unsigned int r = c / 65536;
    c -= c % 65536;
    unsigned int g = c / 256;
    c -= c % 256;
    return UID_RGB(r, g, c, a);
}

unsigned int adjValByFac(unsigned int c, float fac)
{
    UID_RGB temp = intToRgb(c);
    temp.A = temp.A * fac;
    temp.R = temp.R * fac;
    temp.G = temp.G * fac;
    temp.B = temp.B * fac;
    return rgbToInt(temp);
}

struct Animation
{
    //in pixels per second
    int speed;
    int endx;
    int endy;
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
    Layer* lptr;
    Palette* palette;
    Animation* anim;
    int main_color; //index to palette
    int sec_color;  //index to palette

    Object* parent;
    vector<Object*> children;

    // Type Specific
    Object_Type type;
    // Used my multiple
    bool filled; //rect, elipse
    float line_weight; //rect, elipse, curve
    int hardness; //rect, elipse, curve

    // Rect
    int radius;

    // Curve
    vector<vector2> points;

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

    //constraints
    int xMin = -9999;
    int xMax = 9999;
    int yMin = -9999;
    int yMax = 9999;

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
private:
    unsigned int lastw;
    unsigned int lasth;
public:
    vector<Object*> object; //Always stores ALL objects on the layer IN ORDER OF IDs and should not be re-ordered (objects removed when deleted)
    vector<Object*> order; //Stores ACTIVE objects in render order (objects removed when set to enabled is false)
    unsigned int* buffer;
    unsigned int w;
    unsigned int h;
    bool custom;
    bool refresh;
    bool postProcess;
    int id;

    Layer()
    {
        w = 0;
        h = 0;
        lastw = 0;
        lasth = 0;
        refresh = true;
        custom = false;
        postProcess = false;
    }
friend class UID;
}Layer0;

class UID
{
    static const int maxX = 1920;
    static const int maxY = 1080;

public:

    vector<Layer*> layer = { &Layer0 };
    vector<Palette*> palette = { &Palette0 };
    vector<Icon*> icon; // for list of icons

    Object* onTop[maxX * maxY];
    Object* cOnTop = &Object0;  //object that is under mouse for the frame (currently onTop)
    Object* pOnTop = &Object0;  //stores object that mouse was on last frame (previously onTop)
    Object* holding = &Object0; //object that is being held

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
        layer.push_back(new Layer());
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
        return onTop[i];
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

        UID_Input.cmouseX = clamp(0, mw, UID_Input.cmouseX);
        UID_Input.cmouseY = clamp(0, mw, UID_Input.cmouseY);

        cOnTop = Get_Obj(UID_Input.cmouseX, UID_Input.cmouseY, (int)mw, (int)mh); //get object that on under mouse

        if (!firstRender)//handle events if its not the first render
        {
            if (holding == &Object0)
            {
                if (UID_Input.cmouseL && !UID_Input.pmouseL)//--------test click on
                {
                    for (int i = 0; i < cOnTop->ClickOn.size(); i++)
                    {
                        cOnTop->ClickOn[i](cOnTop);
                    }
                }
                else if (cOnTop != pOnTop)//--------------------------test hover
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
                if (UID_Input.cmouseL && UID_Input.pmouseL)//-----test hold on
                {
                    holding = pOnTop;
                    for (int i = 0; i < pOnTop->HoldOn.size(); i++)
                    {
                        pOnTop->HoldOn[i](pOnTop);
                    }
                }
            }
            else if (holding != &Object0)
            {
                if (UID_Input.cmouseL && UID_Input.pmouseL)//-----test hold on
                {
                    for (int i = 0; i < holding->HoldOn.size(); i++)
                    {
                        holding->HoldOn[i](holding);
                    }
                }
                else
                {
                    holding = &Object0;
                }
            }
        }

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

        unsigned int* memptr = (unsigned int*)mem; // memptr is only for cycling through main buffer
        unsigned int* pixel = (unsigned int*)mem;                       // pixel is for cycling through a layers buffer if it has one 

        for (int i = 0; i < mh * mw; i++) // writes background color to main buffer
        {
            *memptr++ = BGcolor;
        }
        memptr = (unsigned int*)mem;

        for (int i = 0; i < maxX * maxY; i++) // clears on top array
        {
            onTop[i] = &Object0;
        }

        for (int i = 0; i < layer.size(); i++)// for each layer ----------------------------------
        {
            Layer* lptr = layer[i];
            
            bool ref = lptr->refresh;
            bool cus = lptr->custom;
            bool ppr = lptr->postProcess;
            bool buf = lptr->buffer;
            unsigned int lw = lptr->w;
            unsigned int lh = lptr->h;
            unsigned int llw = lptr->lastw;
            unsigned int llh = lptr->lasth;

            if (ref && !cus && !ppr) 
            { 
                lptr->buffer = (unsigned int*)mem; 
                lptr->w = mw;
                lptr->h = mh;
            }
            else if ((lw != llw || lh != llh) && lptr->custom) //if custom resize
            {
                if (ref) //if needs to refresh
                {
                    if (lptr->buffer) { free(lptr->buffer); };
                    lptr->buffer = (unsigned int*)malloc(lw * lh * sizeof(unsigned int));
                    pixel = lptr->buffer;
                    for (int j = 0; j < lh * lw; j++) // writes background color to layer buffer
                    {
                        *pixel++ = 0xff000000;
                    }
                    pixel = lptr->buffer;
                    lptr->lastw = lw;
                    lptr->lasth = lh;
                }
                else
                {
                    //code to transfer old buffer to new resized buffer
                }
            }
            else if ((lptr->w != mw || lptr->h != mh) && !cus) //if window resize
            {
                if (ref)
                {
                    if (lptr->buffer) { free(lptr->buffer); };
                    lptr->buffer = (unsigned int*)malloc(mw * mh * sizeof(unsigned int));
                    pixel = lptr->buffer;
                    for (int j = 0; j < lh * lw; j++) // writes background color to layer buffer
                    {
                        *pixel++ = 0xff000000;
                    }
                    pixel = lptr->buffer;
                    lptr->lastw = lw;
                    lptr->lasth = lh;
                }
                else
                {
                    //code to transfer old buffer to new resized buffer
                }
            }

            // starts rendering of and object

            for (int j = 0; j < lptr->order.size(); j++)// for each object------------------------
            {
                //pixel = lptr->buffer;

                Object* optr = lptr->order[j];

                //clamp objects coordinates
                optr->posx = clamp(optr->xMin, optr->xMax, optr->posx);
                optr->posy = clamp(optr->yMin, optr->yMax, optr->posy);

                int oy = optr->posy;             //object y
                int ox = optr->posx;             //object x
                int oh = optr->height;           //object height
                int ow = optr->width;            //object width
                unsigned int mColor = optr->palette->color[optr->main_color];//objects main color
                

                //clamp objects rendering box so it doesnt go off screen
                int cxs = clamp(0, lptr->w, ox);     //clamped x start
                int cys = clamp(0, lptr->h, oy);     //clamped y start
                int cxe = clamp(0, lptr->w, ox + ow);//clamped x end
                int cye = clamp(0, lptr->h, oy + oh);//clamped y end

                switch (optr->type)
                {
                case rectangle:
                    for (int y = cys; y < cye; y++)
                    {
                        pixel = lptr->buffer + (y * lptr->w + cxs);
                        for (int x = cxs; x < cxe; x++)
                        {
                            *pixel++ = mColor;
                            onTop[y * 1920 + x] = optr;
                        }
                    }
                    break;
                case curve:
                {
                    /*
                    float slope = ((float)optr->points[1].y - (float)optr->points[0].y) / ((float)optr->points[1].x - (float)optr->points[0].x);
                    int orx = optr->posx;
                    int ory = optr->posy;
                    if (slope < 0)
                    {
                        ory += optr->height;
                    }
                    float lnw = optr->line_weight/2;
                    
                    pixel += ((int)lw * (oy + no)) + ox + wo;//set pixel to start
                    for (int y = oy + no; y < oy + oh - so; y++)
                    {
                        for (int x = ox + wo; x < ow + ox - eo; x++)
                        {
                            float nx = x - orx;
                            float ny = y - ory;
                            float vert = abs(slope * nx - ny);
                            float hori = abs(ny / slope - nx);
                            hori = (int)(hori != 0) * hori + (int)(hori == 0) * 0.0000001;//make sure hori IS NOT 0
                            float len = vert * cos(atan(vert / hori));
                            //*pixel++ = (int)(len <= lw) * mColor + (int)(len > lw) * *pixel;//no aa
                            *pixel++ = (int)(len <= lnw) * adjValByFac(mColor, abs(len-1)) + (int)(len > lnw) * *pixel;// with aa
                            onTop[y * 1920 + x] = optr;
                        }
                        pixel += (int)lw - ow + eo + wo;
                    }*/
                }
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

            // write to main memory buffer if it needs to
            if (lptr->postProcess || !lptr->refresh)
            {
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
        firstRender = false;
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

class UID_Curve
{
    vector2 calcTopLeft(int x1, int y1, int x2, int y2)
    {
        if (x2 - x1 >= 0 && y2 - y1 >= 0) { return { x1, y1 }; } // pointing SE
        if (x2 - x1 >= 0 && y2 - y1 < 0) { return { x1, y2 }; }// pointing NE
        if (x2 - x1 < 0 && y2 - y1 < 0) { return { x2, y2 }; }// pointing NW
        if (x2 - x1 < 0 && y2 - y1 >= 0) { return { x2, y1 }; }// pointing SE
    }

public:
    int id;
    int layer;
    Object* ptr;
    Layer* lptr;

    void movePoint(int i, int x, int y)
    {
        ptr->points[i].x += x;
        ptr->points[i].y += y;
    }

    void setPoint(int i, int x, int y)
    {
        ptr->points[i].x = x;
        ptr->points[i].y = y;
        ptr->posx = calcTopLeft(ptr->points[0].x, ptr->points[0].y, x, y).x;
        ptr->posy = calcTopLeft(ptr->points[0].x, ptr->points[0].y, x, y).y;
        ptr->width = abs(ptr->points[i].x - ptr->points[0].x);
        ptr->height = abs(ptr->points[i].y - ptr->points[0].y);
    }

    UID_Curve(int l, int x1, int y1, int x2, int y2, Palette* palette, Object* parent)
    {
        id = UID.Add_Object(l, curve);
        layer = l;
        ptr = UID.layer[layer]->object[id];
        ptr->points.push_back({ x1,y1 });
        ptr->points.push_back({ x2,y2 });
        ptr->posx = calcTopLeft(x1, y1, x2, y2).x;
        ptr->posy = calcTopLeft(x1, y1, x2, y2).y;
        ptr->width = abs(x2 - x1) + 1;
        ptr->height = abs(y2 - y1) + 1;
        ptr->palette = palette;
        ptr->parent = parent;
    }
};

//predefined event functions

void followMouse(Object* o) {
    o->posx += UID_Input.cmouseX - UID_Input.pmouseX;
    o->posy += UID_Input.cmouseY - UID_Input.pmouseY;
}

void incPal(Object * o) {
    if (o->main_color < o->palette->color.size()-1) { o->main_color++; }
    else { o->main_color = 0; }
}

void decPal(Object* o) {
    if (o->main_color > 0) { o->main_color--; }
    else { o->main_color = o->palette->color.size()-1; } 
}

void scale(Object* o) {

}
//general functions

int clamp(int min, int max, int val)
{
    if (val > max) val = max;
    else if (val < min) val = min;
    return val;
}

int mapToByte(int min, int max, int val)
{
    val = clamp(min, max, val);
    if (val == 0) { return 0; }
    return 256 * ((max - min) / val) - 1;
}

/*thinking



*/