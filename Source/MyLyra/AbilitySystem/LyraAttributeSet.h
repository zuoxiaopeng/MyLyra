#pragma once

#include "AttributeSet.h"
#include "LyraAttributeSet.generated.h"


#define UE_API MYLYRA_API


struct FGameplayEffectSpec;

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FLyraAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

UCLASS(MinimalAPI)
class ULyraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
};

#undef UE_API 