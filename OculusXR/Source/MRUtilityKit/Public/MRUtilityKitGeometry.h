// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Containers/Array.h"
#include "Math/Vector2D.h"

MRUTILITYKIT_API void MRUKTriangulatePolygon(const TArray<TArray<FVector2f>>& Polygons, TArray<FVector2D>& Vertices, TArray<int32>& Indices);
