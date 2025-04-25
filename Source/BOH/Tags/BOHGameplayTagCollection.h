// 

#pragma once

#include "GameplayTagContainer.h"

#include "BOHGameplayTagCollection.generated.h"

// TODO: Check new lyra implementation using macros and namespaces instead of using this collection.

/**
 * @class UBOHGameplayTagCollection
 * @brief Inheriting this class allows registering native tags. Override AddAllTags in the child class and call AddTag
 * to register tags. The tags will be registered when the BOHAssetManager is initialized.
 */
UCLASS(Abstract, NotBlueprintable)
class BOH_API UBOHGameplayTagCollection : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initialize all children of UBOHGameplayTagCollection.
	 */
	static FSimpleMulticastDelegate* GetOnGameplayTagCollectionCreatedDelegate();

	/**
	 * @brief Add a tag from the collection to the TagManager.
	 */
	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

	/**
	 * @brief Initialize all children of UBOHGameplayTagCollection.
	 */
	UBOHGameplayTagCollection(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	
	FORCEINLINE bool IsInitialized() const { return m_isInitialized; }

protected:
	/**
	 * @brief Override this function to add all tags to the TagManager.
	 */
	virtual void AddAllTags();

	/**
	 * @brief Initialize all children of UBOHGameplayTagCollection.
	 */
	virtual void PostCDOContruct() override;

private:
	bool m_isInitialized = false;

	inline static FSimpleMulticastDelegate m_onGameplayTagCollectionCreatedDelegate = FSimpleMulticastDelegate();
};