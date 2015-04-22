
#ifndef drawable_h__
#define drawable_h__


namespace bd {


class IDrawable
{
public:
    virtual ~IDrawable() {}
    virtual void draw() = 0;
};

} // namespace bd

#endif // !drawable_h__
