#pragma once
#include "Polygon.h"

namespace Shapes {

class Quad : public Polygon {
public:
    const char* GetName() const override { return "Quad"; }
protected:
    void CreateVertices() override;

};

} // namespace Shapes
