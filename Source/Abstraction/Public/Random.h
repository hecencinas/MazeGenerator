// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static class ABSTRACTION_API Random
{
public:
    static int NextInt(int high, int low)
    {
        if (!Seeded)
        {
            srand(time(NULL));
            Seeded = true;
        }
        return (rand() % (high - low)) + low;
    }
    static bool Seeded;
};

bool Random::Seeded = false;

