#ifndef MAT_H
#define MAT_H
#include <stdlib.h>
#include <string.h>



class Mat
{
public:

    Mat();


    Mat(int w, int h, int c);
    Mat(int w, int h, float* data);
    Mat(int w, int h, int c, float* data);
    // release
    ~Mat();
    Mat& operator=( Mat& m);
    void create(int w, int h, int c);
    void release();

    bool empty() const;
    size_t total() const;

    // data reference
    Mat channel(int c);
    const Mat channel(int c) const;
    float* row(int c);

    float* data;

    int w;
    int h;
    int c;

    size_t cstep;
};




inline Mat::Mat()
    : data(0), w(0), h(0), c(0), cstep(0)
{
}

inline Mat::~Mat()
{
    data = 0;
    w = 0;
    h = 0;
	c = 0;
	cstep = 0;

}

inline Mat::Mat(int _w, int _h, int _c)
    : data(0)
{
    create(_w, _h, _c);
}


inline Mat::Mat(int _w, int _h, float* _data)
    : data(_data)
{
    w = _w;
    h = _h;
    c = 1;

    cstep = w * h;
}

inline Mat::Mat(int _w, int _h, int _c, float* _data)
    : data(_data)
{
    w = _w;
    h = _h;
    c = _c;

    cstep = w * h;
}





inline void Mat::create(int _w, int _h, int _c)
{
    if ( w == _w && h == _h && c == _c )
        return;


    w = _w;
    h = _h;
    c = _c;

    cstep = w * h;

    size_t totalsize = total()*sizeof(float);
    data = (float*)malloc(totalsize);

}




inline bool Mat::empty() const
{
    return data == 0 || total() == 0;
}

inline size_t Mat::total() const
{
    return cstep * c;
}

inline Mat Mat::channel(int c)
{
    return Mat(w, h, data + cstep * c );
}

inline const Mat Mat::channel(int c) const
{
    return Mat(w, h, data + cstep * c);
}

inline float* Mat::row(int c)
{
    return (float*)data + w * c;
}
inline Mat& Mat::operator=(Mat& m)
{
    if (this == &m)
        return *this;

    data = m.data;

    w = m.w;
    h = m.h;
    c = m.c;

    cstep = m.cstep;

    return *this;
}
#endif