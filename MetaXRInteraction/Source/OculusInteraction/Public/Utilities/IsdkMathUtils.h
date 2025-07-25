/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "CoreMinimal.h"
#include "IsdkMathUtils.generated.h"

struct FKalmanFilterParameters
{
  FVector X; // Representing the estimated state of the system.
  FVector V; // Velocity vector, used to predict the state in the next time step.
  FVector P; // Error covariance, representing the estimated accuracy of the state prediction.
  FVector K; // Kalman gain, used to update the state vector and error covariance.
  FVector Q; // Process noise covariance, representing the estimated accuracy of the model.
  FVector
      R; // Observation noise covariance, representing the estimated accuracy of the measurements.
  FKalmanFilterParameters()
      : X(FVector::ZeroVector),
        V(FVector::ZeroVector),
        P(FVector::ZeroVector),
        K(FVector::ZeroVector),
        Q(FVector::ZeroVector),
        R(FVector::ZeroVector)
  {
  }
};

class OCULUSINTERACTION_API FIsdkMathUtils
{
 public:
  static float ChangeValueAtRate(float From, float To, float DeltaPerSecond, float Dt);
  static FVector Ransac(
      const TArray<TPair<FVector, float>>& Positions,
      int32 RansacIterations,
      float RansacScoreThreshold);
  static FVector LeastSquares(const TArray<TPair<FVector, float>>& Positions);
  static FVector GetMean(const TArray<FVector>& Positions);
  static FVector GetStandardDeviation(const TArray<FVector>& Positions, const FVector& Mean);
  static FVector
  VelocityFromSamples(FVector Position1, float Time1, FVector Position2, float Time2);
  static void UpdateKalmanFilter(
      FKalmanFilterParameters& Params,
      const FVector& MeasuredPosition,
      double DeltaTime);
};

UCLASS(Blueprintable)
class OCULUSINTERACTION_API UIsdkMathUtilsWrapper : public UObject
{
  GENERATED_BODY()
 public:
  UFUNCTION(BlueprintPure, Category = InteractionSDK)
  static float ChangeValueAtRate(float From, float To, float DeltaPerSecond, float Dt);
};
