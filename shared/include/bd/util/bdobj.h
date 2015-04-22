//
// Created by jim on 4/14/15.
//

#ifndef bdobj_h__
#define bdobj_h__

#include <string>
namespace bd {

class BDObj
{
private:
    static unsigned int counter;

public:
    BDObj();
    BDObj(const std::string&);
    virtual ~BDObj();
    void id(const std::string &);
    virtual std::string to_string() const;

private:
    std::string ID;

};

} // namespace bd

#endif // !bdobj_h__


