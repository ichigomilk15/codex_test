#pragma once
#include "Polygon.h"

class Triangle : public Polygon {
public:
    const char* GetName() const override { return "Triangle"; }
protected:
    void CreateVertices() override;
};
