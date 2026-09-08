#include "EditorPicker.h"
#include "Editor/Core/EditorContext.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/SceneManager.h"
#include "Engine/Graphics/Renderer/RenderPipelineLayouts.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Asset/Resources.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/Components.h"
#include "Engine/Graphics/Renderer/DrawList.h"
#include "Engine/Graphics/Utility.h"

namespace editor
{
  engine::Uuid EditorPicker::Pick(uint32_t x, uint32_t y)
  {
    return engine::Uuid{};
  }
}