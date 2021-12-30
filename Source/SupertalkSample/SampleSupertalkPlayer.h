// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Supertalk/SupertalkPlayer.h"
#include "SampleSupertalkPlayer.generated.h"

UCLASS()
class SUPERTALKSAMPLE_API ASampleSupertalkPlayer : public AActor
{
	GENERATED_BODY()

public:
	ASampleSupertalkPlayer();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Supertalk)
	void PlayScript();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Supertalk)
	USupertalkScript* ScriptToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Supertalk)
	FName InitialSection = NAME_None;

	UPROPERTY(EditAnywhere, Category = Supertalk)
	bool bPlayOnSpawn = false;

private:
	UPROPERTY()
	TObjectPtr<USupertalkPlayer> Player;

	void OnPlayLine(const FSupertalkLine& Line, FSupertalkEventCompletedDelegate Completed);
	void OnPlayChoice(const FSupertalkLine& Line, const TArray<FText>& Choices, FSupertalkChoiceCompletedDelegate Completed);

	// Here's a sample function that a Supertalk script can call
	// Yes, you can call private UFUNCTIONs from a Supertalk script if the object was registered as a function call receiver so be careful!
	UFUNCTION()
	void Wait(float Time);
};