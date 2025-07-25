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

#include "Tests/IsdkCommonTestCommands.h"

bool FIsdkTestCheckInteractionRelationships::Update()
{
  TScriptInterface<IIsdkIInteractorState> TestInteractor;
  TScriptInterface<IIsdkIInteractableState> TestInteractable;
  GetTestInstances(TestInteractor, TestInteractable);

  // Test Interactable
  {
    const FIsdkInteractionRelationshipCounts CurrentCounts =
        TestInteractable->GetInteractableStateRelationshipCounts();

    const FString TestDescriptionHover = FString::Printf(
        TEXT("%s: Ray Interactable Relationship Counts - Hover (actual: %d, expected: %d)"),
        *TestStepName,
        CurrentCounts.NumHover,
        ExpectedCounts.NumHover);
    Test->TestEqual(*TestDescriptionHover, CurrentCounts.NumHover, ExpectedCounts.NumHover);

    const FString TestDescriptionSelect = FString::Printf(
        TEXT("%s: Ray Interactable Relationship Counts - Select (actual: %d, expected: %d)"),
        *TestStepName,
        CurrentCounts.NumSelect,
        ExpectedCounts.NumSelect);
    Test->TestEqual(*TestDescriptionSelect, CurrentCounts.NumSelect, ExpectedCounts.NumSelect);

    TArray<TScriptInterface<IIsdkIInteractorState>> RelationshipInteractors;
    TestInteractable->GetInteractableStateRelationships(
        EIsdkInteractableState::Hover, RelationshipInteractors);
    Test->TestEqual(
        TEXT("Ray Interactable Relationship Interactors (Hover)"),
        RelationshipInteractors.Num(),
        ExpectedCounts.NumHover);
    if (ExpectedCounts.NumHover == 1 && RelationshipInteractors.Num() > 0)
    {
      Test->TestEqual(
          TEXT("Ray Interactable Relationship Hover Instance"),
          RelationshipInteractors[0].GetObject(),
          TestInteractor.GetObject());
    }

    RelationshipInteractors.Empty();
    TestInteractable->GetInteractableStateRelationships(
        EIsdkInteractableState::Select, RelationshipInteractors);
    Test->TestEqual(
        TEXT("Ray Interactable Relationship Interactors (Select)"),
        RelationshipInteractors.Num(),
        ExpectedCounts.NumSelect);
    if (ExpectedCounts.NumSelect == 1 && RelationshipInteractors.Num() > 0)
    {
      Test->TestEqual(
          TEXT("Ray Interactable Relationship Select Instance"),
          RelationshipInteractors[0].GetObject(),
          TestInteractor.GetObject());
    }
  }

  // Test Interactor
  {
    const FIsdkInteractionRelationshipCounts CurrentCounts =
        TestInteractor->GetInteractorStateRelationshipCounts();

    const FString TestDescriptionHover = FString::Printf(
        TEXT("%s: Ray Interactor Relationship Counts - Hover (actual: %d, expected: %d)"),
        *TestStepName,
        CurrentCounts.NumHover,
        ExpectedCounts.NumHover);
    Test->TestEqual(*TestDescriptionHover, CurrentCounts.NumHover, ExpectedCounts.NumHover);

    const FString TestDescriptionSelect = FString::Printf(
        TEXT("%s: Ray Interactor Relationship Counts - Select (actual: %d, expected: %d)"),
        *TestStepName,
        CurrentCounts.NumSelect,
        ExpectedCounts.NumSelect);
    Test->TestEqual(*TestDescriptionSelect, CurrentCounts.NumSelect, ExpectedCounts.NumSelect);

    TArray<TScriptInterface<IIsdkIInteractableState>> RelationshipInteractables;
    TestInteractor->GetInteractorStateRelationships(
        EIsdkInteractableState::Hover, RelationshipInteractables);
    Test->TestEqual(
        TEXT("Ray Interactor Relationship Interactors (Hover)"),
        RelationshipInteractables.Num(),
        ExpectedCounts.NumHover);
    if (ExpectedCounts.NumHover == 1 && RelationshipInteractables.Num() > 0)
    {
      Test->TestEqual(
          TEXT("Ray Interactor Relationship Hover Instance"),
          RelationshipInteractables[0].GetObject(),
          TestInteractable.GetObject());
    }

    RelationshipInteractables.Empty();
    TestInteractor->GetInteractorStateRelationships(
        EIsdkInteractableState::Select, RelationshipInteractables);
    Test->TestEqual(
        TEXT("Ray Interactor Relationship Interactors (Select)"),
        RelationshipInteractables.Num(),
        ExpectedCounts.NumSelect);
    if (ExpectedCounts.NumSelect == 1 && RelationshipInteractables.Num() > 0)
    {
      Test->TestEqual(
          TEXT("Ray Interactor Relationship Select Instance"),
          RelationshipInteractables[0].GetObject(),
          TestInteractable.GetObject());
    }
  }

  return true;
}
