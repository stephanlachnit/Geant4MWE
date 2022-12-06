#include "ActionInitialization.hpp"

#include "PrimaryGeneratorAction.hpp"

void ActionInitialization::BuildForMaster() const
{ }

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());
}
