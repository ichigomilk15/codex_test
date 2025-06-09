#pragma once
#include "Polygon.h"

class Pentagon : public Polygon {
public:
    const char* GetName() const override { return "Pentagon"; }
protected:
    void CreateVertices() override;
};
