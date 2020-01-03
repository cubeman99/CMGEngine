// ECS Tests

#include <gtest/gtest.h>
#include <cmgCore/ecs/cmgECS.h>


struct ECSTestComponentA : public ECSComponent<ECSTestComponentA>
{
	int16 x;
	int16 y;
};

struct ECSTestComponentB : public ECSComponent<ECSTestComponentB>
{
	int32 x;
	int32 y;
};

class ECSTestSystemA : public BaseECSSystem
{
public:
	ECSTestSystemA() : BaseECSSystem()
	{
		AddComponentType<ECSTestComponentA>();
	}

	virtual void UpdateComponents(float delta, BaseECSComponent** components)
	{
		ECSTestComponentA* a = (ECSTestComponentA*) components[0];
		a->x++;
		a->y--;
	}
};

class ECSTestSystemAB : public BaseECSSystem
{
public:
	ECSTestSystemAB() : BaseECSSystem()
	{
		AddComponentType<ECSTestComponentA>();
		AddComponentType<ECSTestComponentB>();
	}

	virtual void UpdateComponents(float delta, BaseECSComponent** components)
	{
		ECSTestComponentA* a = (ECSTestComponentA*) components[0];
		ECSTestComponentB* b = (ECSTestComponentB*) components[1];
		a->x += 5;
		a->y -= 4;
		b->x *= 2;
		b->y /= 2;
	}
};


//-----------------------------------------------------------------------------
// ECS tests
//-----------------------------------------------------------------------------

TEST(ECSComponent, SIZE)
{
	EXPECT_EQ(sizeof(ECSTestComponentA), ECSTestComponentA::SIZE);
	EXPECT_EQ(sizeof(ECSTestComponentB), ECSTestComponentB::SIZE);

}

TEST(ECSComponent, ID)
{
	EXPECT_NE(ECSTestComponentA::ID, ECSTestComponentB::ID);
}

TEST(ECSComponent, IsTypeValid)
{
	EXPECT_TRUE(BaseECSComponent::IsTypeValid(ECSTestComponentA::ID));
	EXPECT_TRUE(BaseECSComponent::IsTypeValid(ECSTestComponentB::ID));
	EXPECT_TRUE(!BaseECSComponent::IsTypeValid(99999999));
}

TEST(ECSComponent, GetTypeSize)
{
	EXPECT_EQ(sizeof(ECSTestComponentA), BaseECSComponent::GetTypeSize(ECSTestComponentA::ID));
	EXPECT_EQ(sizeof(ECSTestComponentB), BaseECSComponent::GetTypeSize(ECSTestComponentB::ID));
}

TEST(ECSComponent, GetTypeCreateFunction)
{
	EXPECT_EQ(ECSTestComponentA::CREATE_FUNCTION, BaseECSComponent::GetTypeCreateFunction(ECSTestComponentA::ID));
	EXPECT_EQ(ECSTestComponentB::CREATE_FUNCTION, BaseECSComponent::GetTypeCreateFunction(ECSTestComponentB::ID));
}

TEST(ECSComponent, GetTypeFreeFunction)
{
	EXPECT_EQ(ECSTestComponentA::FREE_FUNCTION, BaseECSComponent::GetTypeFreeFunction(ECSTestComponentA::ID));
	EXPECT_EQ(ECSTestComponentB::FREE_FUNCTION, BaseECSComponent::GetTypeFreeFunction(ECSTestComponentB::ID));
}

TEST(ECS, CreateEntity)
{
	ECS ecs;
	ECSTestComponentA* aa;
	ECSTestComponentB* bb;
	ECSTestComponentA a;
	a.x = 1;
	a.y = 2;
	ECSTestComponentB b;
	b.x = 3;
	b.y = 4;
	
	// Create with no components
	EntityHandle entity = ecs.CreateEntity();
	EXPECT_TRUE(entity != NULL_ENTITY_HANDLE);
	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	EXPECT_TRUE(aa == nullptr);

	// Create with one components
	entity = ecs.CreateEntity(a);
	EXPECT_TRUE(entity != NULL_ENTITY_HANDLE);
	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	ASSERT_TRUE(aa != nullptr);
	EXPECT_EQ(1, aa->x);
	EXPECT_EQ(2, aa->y);

	// Create with two components
	entity = ecs.CreateEntity(a, b);
	EXPECT_TRUE(entity != NULL_ENTITY_HANDLE);
	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	ASSERT_TRUE(aa != nullptr);
	EXPECT_EQ(1, aa->x);
	EXPECT_EQ(2, aa->y);
	bb = ecs.GetComponent<ECSTestComponentB>(entity);
	ASSERT_TRUE(bb != nullptr);
	EXPECT_EQ(3, bb->x);
	EXPECT_EQ(4, bb->y);
}

TEST(ECS, RemoveEntity)
{
	ECS ecs;
	ECSTestComponentA a;
	a.x = 1;
	a.y = 2;
	ECSTestComponentB b;
	b.x = 3;
	b.y = 4;

	ECSTestSystemAB system;
	ECSSystemList systems;
	systems.AddSystem(system);


	// Create with no components
	a.x = 1;
	b.x = -1;
	EntityHandle entity1 = ecs.CreateEntity(a, b);
	a.x = 3;
	b.x = -3;
	EntityHandle entity2 = ecs.CreateEntity(a, b);
	a.x = 5;
	b.x = -5;
	EntityHandle entity3 = ecs.CreateEntity(a, b);
	EXPECT_TRUE(entity1 != NULL_ENTITY_HANDLE);
	EXPECT_TRUE(entity2 != NULL_ENTITY_HANDLE);
	EXPECT_TRUE(entity3 != NULL_ENTITY_HANDLE);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity1) != nullptr);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentB>(entity1) != nullptr);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentA>(entity1)->x, 1);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentB>(entity1)->x, -1);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity2) != nullptr);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentB>(entity2) != nullptr);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentA>(entity2)->x, 3);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentB>(entity2)->x, -3);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity3) != nullptr);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentB>(entity3) != nullptr);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentA>(entity3)->x, 5);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentB>(entity3)->x, -5);
	ecs.UpdateSystems(systems, 1.0f);
	ecs.RemoveEntity(entity3);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity1) != nullptr);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentB>(entity1) != nullptr);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentA>(entity1)->x, 6);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentB>(entity1)->x, -2);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity2) != nullptr);
	EXPECT_TRUE(ecs.GetComponent<ECSTestComponentB>(entity2) != nullptr);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentA>(entity2)->x, 8);
	EXPECT_EQ(ecs.GetComponent<ECSTestComponentB>(entity2)->x, -6);
	ecs.UpdateSystems(systems, 1.0f);
	ecs.RemoveEntity(entity1);
	ecs.UpdateSystems(systems, 1.0f);
	//EXPECT_TRUE(ecs.GetComponent<ECSTestComponentA>(entity2) != nullptr);
	ecs.RemoveEntity(entity2);
	ecs.UpdateSystems(systems, 1.0f);
}

TEST(ECS, AddGetHasComponent)
{
	ECS ecs;

	ECSTestComponentA a;
	a.x = 1;
	a.y = 2;

	EntityHandle entity = ecs.CreateEntity();
	EXPECT_TRUE(entity != NULL_ENTITY_HANDLE);
	EXPECT_FALSE(ecs.HasComponent<ECSTestComponentA>(entity));
	EXPECT_TRUE(nullptr == ecs.GetComponent<ECSTestComponentA>(entity));

	ecs.AddComponent(entity, a);
	EXPECT_TRUE(ecs.HasComponent<ECSTestComponentA>(entity));
	
	ECSTestComponentA* aa = ecs.GetComponent<ECSTestComponentA>(entity);
	EXPECT_TRUE(aa != nullptr);
	EXPECT_EQ(1, aa->x);
	EXPECT_EQ(2, aa->y);

	ecs.RemoveComponent<ECSTestComponentA>(entity);
	EXPECT_FALSE(ecs.HasComponent<ECSTestComponentA>(entity));
}


TEST(ECS, UpdateSystems)
{
	ECS ecs;
	ECSTestComponentA* aa;
	ECSTestSystemA system;
	ECSSystemList systems;
	ECSTestComponentA a;

	a.x = 3;
	a.y = 7;

	systems.AddSystem(system);

	EntityHandle entity = ecs.CreateEntity(a);

	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	EXPECT_EQ(aa->x, 3);
	EXPECT_EQ(aa->y, 7);

	ecs.UpdateSystems(systems, 1.0f);

	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	EXPECT_EQ(aa->x, 4);
	EXPECT_EQ(aa->y, 6);
}


TEST(ECS, UpdateSystemWithMultipleComponents)
{
	ECS ecs;
	ECSTestComponentA* aa;
	ECSTestComponentB* bb;
	ECSTestSystemAB system;
	ECSSystemList systems;
	ECSTestComponentA a;
	ECSTestComponentB b;

	a.x = 3;
	a.y = 7;
	b.x = 8;
	b.y = 8;

	systems.AddSystem(system);

	EntityHandle entity = ecs.CreateEntity(a, b);

	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	bb = ecs.GetComponent<ECSTestComponentB>(entity);
	EXPECT_EQ(aa->x, 3);
	EXPECT_EQ(aa->y, 7);
	EXPECT_EQ(bb->x, 8);
	EXPECT_EQ(bb->y, 8);

	ecs.UpdateSystems(systems, 1.0f);

	aa = ecs.GetComponent<ECSTestComponentA>(entity);
	bb = ecs.GetComponent<ECSTestComponentB>(entity);
	EXPECT_EQ(aa->x, 8);
	EXPECT_EQ(aa->y, 3);
	EXPECT_EQ(bb->x, 16);
	EXPECT_EQ(bb->y, 4);
}
