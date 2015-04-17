



#ifndef transformable_h__
#define transformable_h__

#include <bd/scene/transform.h>
#include <bd/util/bdobj.h>

#include <vector>

namespace bd {


class Transformable : public BDObj
{
public:
    Transformable();
    virtual ~Transformable();

    //////////////////////////////////////////////////////////////////////////
    /// \brief Update this transformable and all of its children.
    //////////////////////////////////////////////////////////////////////////
    void update(Transformable *parent);


    void update();


    //////////////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////////////
    void addChild(Transformable *c);


    //////////////////////////////////////////////////////////////////////////
    /// \brief Get a non-const reference to the Transform of
    ///  this transformable.
    //////////////////////////////////////////////////////////////////////////
    Transform& transform();


    const std::vector<Transformable*>& children() const;

    virtual std::string to_string() const override;

    //////////////////////////////////////////////////////////////////////////
    /// Bake takes ingredients for a loaf of bread and creates a delicious
    /// and wholesome delight.
    ///
    /// TL;DR: the bake() method bakes for several hours.
    /// Interrupting bake() will leave program in a valid, but
    /// delicious state.  YMMV.
    //////////////////////////////////////////////////////////////////////////

    //  template< class Yummy, class Ingredients, class Bread >
    //  Yummy bake(Bread &b, std::vector<Ingredients> pantry);


private:
    void updateChildren();

    Transform m_transform;
    std::vector<Transformable*> m_children;
};

} // namespace bd

#endif
