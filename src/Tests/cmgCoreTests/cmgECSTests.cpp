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
	//EXPECT_NE(ECSTestComponentA::ID, ECSTestComponentB::ID);
	EXPECT_EQ(ECSTestComponentB::SIZE, ECSTestComponentA::SIZE + 4);
}

TEST(ECSComponent, ID)
{
	//EXPECT_NE(ECSTestComponentA::ID, ECSTestComponentB::ID);
	EXPECT_EQ(ECSTestComponentB::SIZE, ECSTestComponentA::SIZE + 4);
}

TEST(ECSComponent, IsTypeValid)
{
	//EXPECT_TRUE(BaseECSComponent::IsTypeValid(ECSTestComponentA::GetId()));
	//EXPECT_TRUE(BaseECSComponent::IsTypeValid(ECSTestComponentB::GetId()));

	//printf("%d\n", ECSTestComponentA::ID);
	//printf("%d\n", ECSTestComponentB::ID);
	//printf("%u\n", BaseECSComponent::componentTypes.size());

	//
	//printf("%d\n", ECSTestComponentA::GetID());
	//printf("%d\n", ECSTestComponentB::GetID());
	//printf("%u\n", BaseECSComponent::componentTypes.size());
}

TEST(ECS, CreateEntity)
{
	ECS ecs;

	ECSTestComponentA a;
	ECSTestComponentB b;

	ECSTestComponentA a2;
	ECSTestComponentB b2;

	/*BaseECSComponent components[2] = { a, b };
	uint32 componentIds[2] = { ECSTestComponentA::ID, ECSTestComponentB::ID };
	EntityHandle entity = ecs.CreateEntity(components, componentIds, 2);*/
}