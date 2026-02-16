


//TODO: have all the structs update when needed
USTRUCT(BlueprintType, Blueprintable)
struct FRewindLifeTime
{
	GENERATED_BODY()

	//Snapshot of the game state that gets updated everytime all units have gone

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TurnCount = 0;


	//Information character had at the start of their turn, turn is indicated by the indexed number, make sure theirs a check

	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Health;
	//MP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> MP;
	// Augment
	TArray<TMap<EDamageType, int>> AugmentStage;
	// Damage Points
	TArray<int> DamageStage;
	// Defense Points
	TArray<int> NegationStage;

};

struct FLifeTimeEachAction
{

	//any information about the unit that needs to be reset when their turn ends


	//todo: heal amount, and maybe drain/blood loss
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DrainAmount;

	
	//Damage Taken
	TMap<EDamageType, float> DamageTaken
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};

	TMap<EDamageType, int> DamageTakenElementCount
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};

	
};

struct FLifeTimeTurnStart
{

	// damage dealt this turn
	// elements used this turn
	// any healing done this turn
	// any drain done this turn
	// specific abilties used this turn


	//todo: heal amount, and maybe drain/blood loss
	TMap<EHealTypes, float> HealAmountType;
	float HealAmount;
	TMap<EDrainTypes, float> DrainAmountType;
	float DrainAmount;


	// Damage dealt, should get reset at the first turn start
	TMap<EDamageType, float> DamageDealt
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},

	};

	TMap<EDamageType, int> DamageDealtElementCount
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},

	};
};

struct FLifeTimeBattleInfo
{

	//information that lasts until the battle is over

	TMap<EHealTypes, float> TotalHealAmount;

	// Total damage taken
	TMap<EDamageType, float> TotalDamageTaken
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},

	};


	TMap<EDamageType, int> TotalDamageTakenElementCount
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};

	TMap<EDamageType, float> TotalDamageDealt
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};


	TMap<EDamageType, int> TotalDamageDealtElementCount
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};


	
	float TotalDrainAmount;

	//TODO: have this update
	//everytime an ability is used, increment this count
	TMap<FName, int> AbilityNameUsedCount;
	
};



/*TODO: this whole thing needs to be updated properly which it is currently not*/
UCLASS(ClassGroup=(Unit), meta=(BlueprintSpawnableComponent))
class MADNESSPULSE_API UBattleTurnInformation : public UUnitBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBattleTurnInformation();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AUnitBase> UnitOwnerReference;

	//delegate
	UPROPERTY(VisibleAnywhere, BlueprintCallable, BlueprintAssignable)
	FOnTurnChange OnTurnChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	void UpdateGameStateSnapShot(int TurnCount);
	void UpdateFirstTurnStartInfo();
	void UpdateTurnEndInfo();

	void AddToDamageTaken(EDamageType DamageType, float DamageValue);
	void AddToDamageDealt(EDamageType DamageType, float DamageValue);

	float ReturnDamageTakenByType(EDamageType DamageType);
	float ReturnAllDamageTaken();
	float ReturnTotalDamageTakenByType(EDamageType DamageType);

	float ReturnAllTotalDamageTaken();
	
	float ReturnDamageDealtByType(EDamageType DamageType);
	float ReturnAllDamageDealtByType(EDamageType DamageType);
	float ReturnTotalDamageDealtByType(EDamageType DamageType);
	float ReturnAllTotalDamageDealt(EDamageType DamageType);


	//resets all info besides maybe the turn count/rewind info
	//TODO: implement it
	void ClearInfoForEverything();

	//TODO: implement it
	//checking to make sure were not using it on the first turn
	bool IsRewindAvailableForUse() const;
	int GetRewindTurnCount() const;

	
	

	/*UNTIL THE CHARACTERS TURN STARTS*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRewindLifeTime RewindLifeTime;

	
	/*UNTIL THE CHARACTERS TURN STARTS*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLifeTimeTurnStart LifeTimeTurnStart;


	/*UNTIL THE CHARACTERS TURN ENDS, RESETS FOR EVERY CHARACTER*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLifeTimeEachAction LifeTimeEachAction;

	float EachAction_DamageTaken();
 

	/*Permanent category*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLifeTimeBattleInfo LifeTimeBattleInfo;
};
