#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "logger.h"
#include <vector>

enum DirectionUDLR {
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_RIGHT,
  DIRECTION_NEUTRAL,
  DIRECTION_NUM_STATES // Count-- must be last
};

enum PaletteColor {
  PALETTE_BLACK,
  PALETTE_WHITE,
  NUM_PALETTE_COLORS // Count-- must be last
};


SDL_Texture	 *  loadImage(const char * filename, SDL_Renderer * renderer, Logger * log = nullptr);
 // load a single image, returning nullptr on fail
 // if logger is passed (optional), then more specific SDL_Image errors will be logged.


bool isArrowKey(SDL_Scancode);



// 2D vector class supporting vector arithmetic
template <class T> class Vector2D {
  public:
    Vector2D() : x(0), y(0) {}
    Vector2D(T x, T y) : x(x), y(y) {}
    Vector2D(const Vector2D & old) {x=old.x;y=old.y;}
    ~Vector2D() {}

    friend Vector2D operator+(const Vector2D & v1, const Vector2D & v2) { return Vector2D(v1.x+v2.x,v1.y+v2.y); }
    friend Vector2D operator-(const Vector2D & v1, const Vector2D & v2) { return Vector2D(v1.x-v2.x,v1.y-v2.y); }
    friend Vector2D operator*(const Vector2D & v1, const T & s) { return Vector2D(v1.x*s,v1.y*s); }
    friend Vector2D operator*(const T & s, const Vector2D & v1) { return v1*s; }
    friend T dot(const Vector2D & v1, const Vector2D & v2) {return (v1.x*v2.x)+(v1.y*v2.y);}

    void print() {printf("%d, %d\n",x,y);} // used for testing and should be deleted

    T x;
    T y;
};

typedef Vector2D<int>    vecI;
typedef Vector2D<double> vecD;



class Hyperplane {
public:
  Hyperplane() : pt(), normal() {}
  Hyperplane(vecI pt, vecI normal) : pt(pt), normal(normal) {}
  ~Hyperplane() {}

  bool pos_side(vecI point) {return dot(point-pt,normal)>=0;}

  vecI pt;
  vecI normal;
};

typedef std::vector<Hyperplane> Polytope;

bool pointInPolytope(vecI,Polytope);




// This will have one global object to hold useful tables and stuff
class Globals {
  public:
    Globals();
    vecI directionToUnitVector [DIRECTION_NUM_STATES];
};






// --- A circular buffer container ---

// Forward declaration
template <class T> class CircleBufferIter;

template <class T> class CircleBuffer{

  friend class CircleBufferIter<T>;

  typedef CircleBufferIter<T> iterator;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef T value_type;
  typedef T * pointer;
  typedef T & reference;

  public:
    CircleBuffer(int length, T initial_val) : length(length) {arr = new T [length]; for (int i=0; i<length; i++) arr[i]=initial_val;}
    ~CircleBuffer() {delete[] arr;}
    void push(T item) {arr[head]=item; head = (head+1)%length;}

    iterator begin() {return iterator(this, 0);}
    iterator end() {return iterator(this, length);}

  private:
    int length;
    int head = 0;
    T * arr = nullptr;
};

template <class T> class CircleBufferIter{
  public:
    CircleBufferIter(CircleBuffer<T> * circleBuffer, int index) : theCircleBuffer(circleBuffer), index(index) {}
    CircleBufferIter(const CircleBufferIter<T> & other) {index = other.index; theCircleBuffer=other.theCircleBuffer;}
    bool operator!=(const CircleBufferIter<T> & other) const {return index != other.index;}
    T & operator*() {int i = theCircleBuffer->head - index-1; i = i>=0 ? i : i+theCircleBuffer->length ; return theCircleBuffer->arr[i];}
    CircleBufferIter<T> & operator++() {index++; return *this;}
    CircleBufferIter<T> & operator++(int dummy) {CircleBufferIter<T> copy(*this); ++(*this); return copy;}

  private:
    CircleBuffer<T> * theCircleBuffer;
    int index;

};

// --- end of circular buffer container ---
