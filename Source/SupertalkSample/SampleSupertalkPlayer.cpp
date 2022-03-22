// Fill out your copyright notice in the Description page of Project Settings.


#include "SampleSupertalkPlayer.h"
#include "Supertalk/SupertalkValue.h"

ASampleSupertalkPlayer::ASampleSupertalkPlayer()
{
	// Create the supertalk player, which is what runs scripts.
	Player = CreateDefaultSubobject<USupertalkPlayer>(TEXT("SupertalkPlayer"));
}

void ASampleSupertalkPlayer::BeginPlay()
{
	// Bind events to handle dialogue
	Player->OnPlayLineEvent.BindUObject(this, &ThisClass::OnPlayLine);
	Player->OnPlayChoiceEvent.BindUObject(this, &ThisClass::OnPlayChoice);

	// Register this object as being able to receive commands from Supertalk scripts
	Player->AddFunctionCallReceiver(this);

	if (bPlayOnSpawn)
	{
		PlayScript();
	}
}

void ASampleSupertalkPlayer::PlayScript()
{
	if (!IsValid(ScriptToPlay))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot play invalid script on %s"), *GetName());
		return;
	}

	if (Player->IsRunningScript())
	{
		Player->Stop();
	}

	// Resetting state isn't necessary if you want multiple scripts to share state. Adding an option for this isn't a bad idea, but in most cases
	// sharing state is probably a bad idea.
	Player->ClearVariables();

	// Run the script
	Player->RunScript(ScriptToPlay, InitialSection);
}

void ASampleSupertalkPlayer::OnPlayLine(const FSupertalkLine& Line, FSupertalkEventCompletedDelegate Completed)
{
	// This is where you'd pass the line to your UI. For simplicity's sake, this is just being added as a debug message.

	// The "speaker" of a line might be a string, or it might be a custom object (maybe one that contains the name of the speaker and a profile image). Depends on your game.
	// For this sample, we'll just get a display string.
	FString SpeakerName;
	if (IsValid(Line.Speaker))
	{
		// A USupertalkValue may represent a constant value or a variable - the "Resolved" family of functions automatically resolves variables to their actual values and as such is generally
		// what you want. ToResolvedDisplayText will get a string representation of whatever the value is, but other values are also possible.
		// For example, a custom object can be retrieved by first resolving the value and then casting it to USupertalkObjectValue:
		// const USupertalkObjectValue* Value = Cast<USupertalkObjectValue>(Line.Speaker->GetResolvedValue(Player);
		// if (Value)
		// {
		//	ObjValue->Object->DoSomething();
		// }

		SpeakerName = Line.Speaker->ToResolvedDisplayText(Player).ToString();
	}

	// Supertalk lines can have format specifiers contained within them, letting you concat variables and other values within the line's text.
	// FormatText is a utility function that formats a line appropriately and resolves any variables.
	FString LineText = Line.FormatText(Player).ToString();
	
	// There are other fields in FSupertalkLine that may be useful - for example, attributes. For simplicity's sake, those are not included here but may be useful when implementing things yourself.

	FString LineStr = FString::Printf(TEXT("%s says: %s"), *SpeakerName, *LineText);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LineStr);

	// To progress the script you must call the completed delegate. This doesn't have to be called immediately or within the same function - it can be called as part of a separate event (for example,
	// pressing a button to skip the current line) or on a delay (for example, after an audio file has played). For the purposes of this sample, we're just going to call it here immediately.
	// Just make sure that you do execute it at some point - otherwise the active script will be stuck.
	// If you need to expose this to blueprint, you can wrap the delegate with a struct or object and implement a function that lets blueprint execute it.
	Completed.ExecuteIfBound();
}

void ASampleSupertalkPlayer::OnPlayChoice(const FSupertalkLine& Line, const TArray<FText>& Choices, FSupertalkChoiceCompletedDelegate Completed)
{
	// This is the same as OnPlayLine, but we always select the first choice regardless of what it is.
	// In an actual game you'd want to wait for the user to make a selection from the Choices array.
	FString SpeakerName;
	if (IsValid(Line.Speaker))
	{
		SpeakerName = Line.Speaker->ToResolvedDisplayText(Player).ToString();
	}

	FString LineText = Line.FormatText(Player).ToString();

	FString LineStr = FString::Printf(TEXT("%s asks you to choose: %s"), *SpeakerName, *LineText);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LineStr);

	for (const FText& Choice : Choices)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("  - %s"), *Choice.ToString()));
	}

	// You'd normally pass the index of the choice in here, but we'll hardcode it at 0 for this sample.
	Completed.ExecuteIfBound(0);
}

void ASampleSupertalkPlayer::Wait(float Time)
{
	// This is a simple "latent" function. If a function Supertalk executes creates a finalizer (via MakeLatentFunction) then the function
	// will be assumed to be latent and will not complete until the finalizer is completed (which can be at any point in the future).
	// This works similarly to the line/choice completed delegates in OnPlayLine/OnPlayChoice.
	// If you do not call MakeLatentFunction then it is assumed that the function is *not* latent and execution will continue immediately upon the function's return.
	if (Time > 0.f)
	{
		// We're a latent function now.
		FSupertalkLatentFunctionFinalizer Finalizer = Player->MakeLatentFunction();

		// Set a timer and call the finalizer when it completes.
		FTimerHandle Handle;
		FTimerDelegate Delegate;
		Delegate.BindWeakLambda(this, [Finalizer]() mutable { Finalizer.Complete(); });
		GetWorldTimerManager().SetTimer(Handle, Delegate, Time, false);
	}

	// If Time was <= 0 then we never call MakeLatentFunction and therefore this function completes immediately, not blocking further execution of the script.
}
