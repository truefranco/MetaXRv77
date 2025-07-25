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
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkMathUtilsTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.FIsdkMathUtilsTest.All",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FIsdkMathUtilsTest::RunTest(const FString& Parameters)
{
  // Test CalculateMean
  {
    TArray<FVector> Positions = {FVector(1, 2, 3), FVector(4, 5, 6), FVector(7, 8, 9)};
    FVector Mean = FIsdkMathUtils::GetMean(Positions);
    TestEqual(TEXT("CalculateMean"), Mean, FVector(4, 5, 6));
  }
  // Test CalculateStandardDeviation
  {
    TArray<FVector> Positions = {FVector(1, 2, 3), FVector(4, 5, 6), FVector(7, 8, 9)};
    FVector Mean = FIsdkMathUtils::GetMean(Positions);
    FVector StdDev = FIsdkMathUtils::GetStandardDeviation(Positions, Mean);
    // Expected values calculated manually
    TestEqual(TEXT("CalculateStandardDeviation"), StdDev, FVector(3.000, 3.000, 3.000));
  }
  // Test CalculateVelocityFromSamples
  {
    FVector Position1 = FVector(1, 2, 3);
    float Time1 = 1.0f;
    FVector Position2 = FVector(4, 5, 6);
    float Time2 = 2.0f;
    FVector Velocity = FIsdkMathUtils::VelocityFromSamples(Position1, Time1, Position2, Time2);
    TestEqual(TEXT("CalculateVelocityFromSamples"), Velocity, FVector(3, 3, 3));
  }
  // Test LeastSquares
  {
    TArray<TPair<FVector, float>> Positions = {
        TPair<FVector, float>(FVector(1, 2, 3), 1.0f),
        TPair<FVector, float>(FVector(4, 5, 6), 2.0f),
        TPair<FVector, float>(FVector(7, 8, 9), 3.0f)};
    FVector Velocity = FIsdkMathUtils::LeastSquares(Positions);
    // Expected value calculated manually
    TestEqual(TEXT("LeastSquares"), Velocity, FVector(3, 3, 3));
  }
  // Test LeastSquares with same timestamps
  {
    TArray<TPair<FVector, float>> Positions = {
        TPair<FVector, float>(FVector(1, 2, 3), 1.0),
        TPair<FVector, float>(FVector(4, 5, 6), 1.0),
        TPair<FVector, float>(FVector(7, 8, 9), 1.0)};
    FVector Velocity = FIsdkMathUtils::LeastSquares(Positions);
    // Expected value is zero vector because all timestamps are the same
    TestEqual(TEXT("LeastSquares with same timestamps"), Velocity, FVector::ZeroVector);
  }
  // Test LeastSquares with same positions
  {
    TArray<TPair<FVector, float>> Positions = {
        TPair<FVector, double>(FVector(1, 2, 3), 1.0),
        TPair<FVector, double>(FVector(1, 2, 3), 2.0),
        TPair<FVector, double>(FVector(1, 2, 3), 3.0)};
    FVector Velocity = FIsdkMathUtils::LeastSquares(Positions);
    // Expected value is zero vector because all positions are the same
    TestEqual(TEXT("LeastSquares with same positions"), Velocity, FVector::ZeroVector);
  }
  // Test UpdateKalmanFilter
  {
    FKalmanFilterParameters Params;
    Params.X = FVector(0, 0, 0); // initial position
    Params.V = FVector(1, 1, 1); // initial velocity
    Params.P = FVector(1, 1, 1); // initial error covariance
    Params.Q = FVector(0.1, 0.1, 0.1); // process noise covariance
    Params.R = FVector(0.01, 0.01, 0.01); // measurement noise covariance
    FVector MeasuredPosition = FVector(1, 1, 1); // measured position
    double DeltaTime = 1.0; // time step
    FIsdkMathUtils::UpdateKalmanFilter(Params, MeasuredPosition, DeltaTime);

    FVector ExpectedX = FVector(1.0, 1.0, 1.0); // expected position
    FVector ExpectedV = FVector(1.0, 1.0, 1.0); // expected velocity
    FVector ExpectedP = FVector(0.01, 0.01, 0.01); // expected error covariance
    FVector ExpectedK = FVector(0.991, 0.991, 0.991); // expected Kalman gain
    TestEqual(TEXT("UpdateKalmanFilter.X"), Params.X, ExpectedX);
    TestEqual(TEXT("UpdateKalmanFilter.V"), Params.V, ExpectedV);
    TestEqual(TEXT("UpdateKalmanFilter.P"), Params.P, ExpectedP);
    TestEqual(TEXT("UpdateKalmanFilter.K"), Params.K, ExpectedK);
  }
  // Test Ransac
  {
    TArray<TPair<FVector, float>> Positions = {
        TPair<FVector, float>(FVector(1, 2, 3), 1.0f),
        TPair<FVector, float>(FVector(4, 5, 6), 2.0f),
        TPair<FVector, float>(FVector(7, 8, 9), 3.0f)};
    int32 RansacIterations = 100;
    float RansacScoreThreshold = 0.1f;
    FVector BestSample = FIsdkMathUtils::Ransac(Positions, RansacIterations, RansacScoreThreshold);

    TestEqual(TEXT("Ransac"), BestSample, FVector(3, 3, 3));
  }
  // Test ChangeValueAtRate
  {
    float From = 0.0f;
    float To = 10.0f;
    float DeltaPerSecond = 5.0f;
    float Dt = 1.0f;
    float ExpectedResult = 5.0f;
    float Result = FIsdkMathUtils::ChangeValueAtRate(From, To, DeltaPerSecond, Dt);
    TestEqual(TEXT("ChangeValueAtRate with normal increment"), Result, ExpectedResult);
    From = 10.0f;
    To = 0.0f;
    DeltaPerSecond = 5.0f;
    Dt = 1.0f;
    ExpectedResult = 5.0f;
    Result = FIsdkMathUtils::ChangeValueAtRate(From, To, DeltaPerSecond, Dt);
    TestEqual(TEXT("ChangeValueAtRate with normal decrement"), Result, ExpectedResult);
    From = 0.0f;
    To = 10.0f;
    DeltaPerSecond = 20.0f;
    Dt = 1.0f;
    ExpectedResult = 10.0f;
    Result = FIsdkMathUtils::ChangeValueAtRate(From, To, DeltaPerSecond, Dt);
    TestEqual(TEXT("ChangeValueAtRate with overshoot"), Result, ExpectedResult);
  }
  return true;
}
