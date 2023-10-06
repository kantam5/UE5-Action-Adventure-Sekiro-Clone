// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/ScreenOverlayWidget.h"
#include "Animation/WidgetAnimation.h"

void UScreenOverlayWidget::PlayExecuteAnimation()
{
	SetRenderOpacity(1.f);
	PlayAnimation(ExecuteAnim);
}
