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

#include "Utilities/IsdkMathUtils.h"

float FIsdkMathUtils::ChangeValueAtRate(float From, float To, float DeltaPerSecond, float Dt)
{
  const float Delta = (To - From);
  const float DeltaMagnitude = FMath::Abs(Delta);
  const float Step = DeltaPerSecond * Dt;

  if (DeltaMagnitude <= Step)
  {
    return To;
  }

  return From + FMath::Sign(Delta) * Step;
}

float UIsdkMathUtilsWrapper::ChangeValueAtRate(float From, float To, float DeltaPerSecond, float Dt)
{
  return FIsdkMathUtils::ChangeValueAtRate(From, To, DeltaPerSecond, Dt);
}

FVector FIsdkMathUtils::Ransac(
    const TArray<TPair<FVector, float>>& Positions,
    int32 RansacIterations,
    float RansacScoreThreshold)
{
  FVector BestSample = FVector::ZeroVector;
  float BestScore = FLT_MAX;
  for (int32 i = 0; i < RansacIterations; i++)
  {
    const int32 Idx1 = FMath::RandRange(0, Positions.Num() - 1);
    const int32 Idx2 = FMath::RandRange(0, Positions.Num() - 1);
    // Ensure that the position with the smaller time is used as the start position
    if (Positions[Idx1].Value < Positions[Idx2].Value)
    {
      FVector Sample = VelocityFromSamples(
          Positions[Idx1].Key, Positions[Idx1].Value, Positions[Idx2].Key, Positions[Idx2].Value);
      // Calculate the score
      float Score = 0.0f;
      for (const TPair<FVector, float>& PositionTimestamp : Positions)
      {
        FVector Position = PositionTimestamp.Key;
        Score += FVector::DistSquared(Sample, Position);
      }

      if (Score < BestScore)
      {
        BestSample = Sample;
        BestScore = Score;
        // If the best score is below the threshold, terminate the loop early
        if (BestScore < RansacScoreThreshold)
        {
          break;
        }
      }
    }
  }
  return BestSample;
}

FVector FIsdkMathUtils::LeastSquares(const TArray<TPair<FVector, float>>& Positions)
{
  const int32 NumPositions = Positions.Num();
  double SumTime = 0.0f;
  double SumTimeSquared = 0.0f;
  FVector SumPosition = FVector::ZeroVector;
  FVector SumTimePosition = FVector::ZeroVector;

  for (const TPair<FVector, float>& PositionTimestampPair : Positions)
  {
    const float Time = PositionTimestampPair.Value;
    FVector Position = PositionTimestampPair.Key;

    SumTime += Time;
    SumTimeSquared += Time * Time;
    SumPosition += Position;
    SumTimePosition += Time * Position;
  }

  const double Denominator = NumPositions * (SumTimeSquared - (SumTime * SumTime) / NumPositions);

  if (FMath::IsNearlyZero(Denominator))
  {
    return FVector::ZeroVector;
  }

  const FVector Velocity = (NumPositions * SumTimePosition - SumTime * SumPosition) / Denominator;

  return Velocity;
}

FVector FIsdkMathUtils::GetMean(const TArray<FVector>& Positions)
{
  if (Positions.Num() == 0)
  {
    return FVector::ZeroVector;
  }

  FVector Mean = FVector::ZeroVector;

  for (const FVector& Position : Positions)
  {
    Mean += Position;
  }
  return Mean / Positions.Num();
}

FVector FIsdkMathUtils::GetStandardDeviation(const TArray<FVector>& Positions, const FVector& Mean)
{
  if (Positions.Num() <= 1)
  {
    return FVector::ZeroVector;
  }

  FVector StdDev = FVector::ZeroVector;
  for (const FVector& Position : Positions)
  {
    FVector Diff = Position - Mean;
    StdDev += Diff * Diff;
  }

  const int32 divisor = Positions.Num() - 1;
  StdDev.X = FMath::Sqrt(StdDev.X / divisor);
  StdDev.Y = FMath::Sqrt(StdDev.Y / divisor);
  StdDev.Z = FMath::Sqrt(StdDev.Z / divisor);

  return StdDev;
}

FVector
FIsdkMathUtils::VelocityFromSamples(FVector Position1, float Time1, FVector Position2, float Time2)
{
  const FVector PositionShift = Position2 - Position1;
  const float TimeShift = Time2 - Time1;

  if (FMath::IsNearlyZero(TimeShift))
  {
    return FVector::ZeroVector;
  }

  return PositionShift / TimeShift;
}

void FIsdkMathUtils::UpdateKalmanFilter(
    FKalmanFilterParameters& Params,
    const FVector& MeasuredPosition,
    const double DeltaTime)
{
  FVector& X = Params.X;
  FVector& V = Params.V;
  FVector& P = Params.P;
  FVector& K = Params.K;
  const FVector& Q = Params.Q;
  const FVector& R = Params.R;

  X = X + V * DeltaTime; // predict new position based on velocity
  P = P + Q;
  // Update
  K = P / (P + R);
  const FVector Y = MeasuredPosition - X; // residual
  X = X + K * Y; // position estimate
  V = V + K * Y / DeltaTime; // velocity estimate
  P = (FVector(1, 1, 1) - K) * P;
}
