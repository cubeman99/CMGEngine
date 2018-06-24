// ECS Tests

#include <gtest/gtest.h>
#include <cmgCore/ecs/cmgECS.h>

//BEGIN EXAMPLE CODE

struct ECSTestComponentA : public ECSComponent<ECSTestComponentA>
{
	uint16 x;
	uint16 y;
};

struct ECSTestComponentB : public ECSComponent<ECSTestComponentB>
{
	uint32 x;
	uint32 y;
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

