#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "logger.h"



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
    Vector2D(T x, T y) : x(x), y(y) {}
    Vector2D(const Vector2D & old) {x=old.x;y=old.y;}
    ~Vector2D() {}

    Vector2D & operator+=(const Vector2D & rhs) { x+=rhs.x; y+=rhs.y; return *this; }
    Vector2D & operator+(const Vector2D & other) { return Vector2D(*this) += other; }
    Vector2D & operator*=(const T & scalar) { x*=scalar; y*=scalar; return *this; }
    Vector2D & operator*(const T & scalar) { return Vector2D(*this) *= scalar; }

    void print() {printf("%d, %d\n",x,y);} // used for testing and should be deleted

    T x;
    T y;
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
