#pragma once

#include "ICombatAction.h"
#include <string>


class GenericShootAction : public ICombatAction 
{
public:
  void BeginTargeting(Engine::Uuid casterId) override 
  {

    
  }

  void UpdateTargeting(Grid* grid, int mouseX, int mouseY) override 
  {
    //GridRenderer::HighlightTile(mouseX, mouseY, Color::Red);
  }

  void CancelTargeting() override 
  {
    //GridRenderer::ClearHighlights();
  }

  void Execute(Engine::Uuid casterId, Grid* grid, int targetX, int targetY) override 
  {
    CancelTargeting(); // Clean up the targeting visuals
  }
};