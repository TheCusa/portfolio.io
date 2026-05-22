// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils.h"

Utils::Utils()
{
}

Utils::~Utils()
{
}
TArray<int8> Utils::GenerateCode(int8 ArraySize)
{
	TArray<int8> Temp;
	for (int i = 0; i < ArraySize; ++i)
	{
		Temp.Add(FMath::RandRange(0, 9));
	}
	return Temp;
}

void Utils::DebugShowCode(TArray<int8>& Array, FString message)
{
#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		for (int i = 0; i < Array.Num(); ++i)
		{
			message += FString::Printf(TEXT("%d "), Array[i]);
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, message);
	}
#endif
}
