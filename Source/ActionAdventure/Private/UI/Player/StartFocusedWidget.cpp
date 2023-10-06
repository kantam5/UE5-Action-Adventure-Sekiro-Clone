// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/StartFocusedWidget.h"

#include "Components/TextBlock.h"

void UStartFocusedWidget::SetFocusedText(FText InText)
{
	TextBlock_Focused->SetText(InText);
}
