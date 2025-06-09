#pragma once
#include "Polygon.h"

class Quad : public Polygon {
public:
    const char* GetName() const override { return "Quad"; }
protected:
    void CreateVertices() override;
};
