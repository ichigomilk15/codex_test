#pragma once
#include "Polygon.h"

namespace Shapes {

class Pentagon : public Polygon {
public:
    const char* GetName() const override { return "Pentagon"; }
protected:
    void CreateVertices() override;

};

} // namespace Shapes
