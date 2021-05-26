#include "Runtime/World/CActor.hpp"
#include "Runtime/World/CAi.hpp"
#include "Runtime/World/CAmbientAI.hpp"
#include "Runtime/World/CDestroyableRock.hpp"
#include "Runtime/World/CEffect.hpp"
#include "Runtime/World/CEntity.hpp"
#include "Runtime/World/CExplosion.hpp"
#include "Runtime/World/CFire.hpp"
#include "Runtime/World/CFishCloud.hpp"
#include "Runtime/World/CFishCloudModifier.hpp"
#include "Runtime/World/CGameLight.hpp"
#include "Runtime/World/CHUDBillboardEffect.hpp"
#include "Runtime/World/CIceImpact.hpp"
#include "Runtime/World/CPatterned.hpp"
#include "Runtime/World/CPlayer.hpp"
#include "Runtime/World/CRepulsor.hpp"
#include "Runtime/World/CScriptActor.hpp"
#include "Runtime/World/CScriptActorKeyframe.hpp"
#include "Runtime/World/CScriptActorRotate.hpp"
#include "Runtime/World/CScriptAiJumpPoint.hpp"
#include "Runtime/World/CScriptAreaAttributes.hpp"
#include "Runtime/World/CScriptBallTrigger.hpp"
#include "Runtime/World/CScriptBeam.hpp"
#include "Runtime/World/CScriptCameraBlurKeyframe.hpp"
#include "Runtime/World/CScriptCameraFilterKeyframe.hpp"
#include "Runtime/World/CScriptCameraHint.hpp"
#include "Runtime/World/CScriptCameraHintTrigger.hpp"
#include "Runtime/World/CScriptCameraPitchVolume.hpp"
#include "Runtime/World/CScriptCameraShaker.hpp"
#include "Runtime/World/CScriptCameraWaypoint.hpp"
#include "Runtime/World/CScriptColorModulate.hpp"
#include "Runtime/World/CScriptControllerAction.hpp"
#include "Runtime/World/CScriptCounter.hpp"
#include "Runtime/World/CScriptCoverPoint.hpp"
#include "Runtime/World/CScriptDamageableTrigger.hpp"
#include "Runtime/World/CScriptDebris.hpp"
#include "Runtime/World/CScriptDebugCameraWaypoint.hpp"
#include "Runtime/World/CScriptDistanceFog.hpp"
#include "Runtime/World/CScriptDock.hpp"
#include "Runtime/World/CScriptDockAreaChange.hpp"
#include "Runtime/World/CScriptDoor.hpp"
#include "Runtime/World/CScriptEMPulse.hpp"
#include "Runtime/World/CScriptEffect.hpp"
#include "Runtime/World/CScriptGenerator.hpp"
#include "Runtime/World/CScriptGrapplePoint.hpp"
#include "Runtime/World/CScriptGunTurret.hpp"
#include "Runtime/World/CScriptHUDMemo.hpp"
#include "Runtime/World/CScriptMazeNode.hpp"
#include "Runtime/World/CScriptMemoryRelay.hpp"
#include "Runtime/World/CScriptMidi.hpp"
#include "Runtime/World/CScriptPickup.hpp"
#include "Runtime/World/CScriptPickupGenerator.hpp"
#include "Runtime/World/CScriptPlatform.hpp"
#include "Runtime/World/CScriptPlayerActor.hpp"
#include "Runtime/World/CScriptPlayerHint.hpp"
#include "Runtime/World/CScriptPlayerStateChange.hpp"
#include "Runtime/World/CScriptPointOfInterest.hpp"
#include "Runtime/World/CScriptRandomRelay.hpp"
#include "Runtime/World/CScriptRelay.hpp"
#include "Runtime/World/CScriptRipple.hpp"
#include "Runtime/World/CScriptRoomAcoustics.hpp"
#include "Runtime/World/CScriptShadowProjector.hpp"
#include "Runtime/World/CScriptSound.hpp"
#include "Runtime/World/CScriptSpawnPoint.hpp"
#include "Runtime/World/CScriptSpecialFunction.hpp"
#include "Runtime/World/CScriptSpiderBallAttractionSurface.hpp"
#include "Runtime/World/CScriptSpiderBallWaypoint.hpp"
#include "Runtime/World/CScriptSpindleCamera.hpp"
#include "Runtime/World/CScriptSteam.hpp"
#include "Runtime/World/CScriptStreamedMusic.hpp"
#include "Runtime/World/CScriptSwitch.hpp"
#include "Runtime/World/CScriptTargetingPoint.hpp"
#include "Runtime/World/CScriptTimer.hpp"
#include "Runtime/World/CScriptTrigger.hpp"
#include "Runtime/World/CScriptVisorFlare.hpp"
#include "Runtime/World/CScriptVisorGoo.hpp"
#include "Runtime/World/CScriptWater.hpp"
#include "Runtime/World/CScriptWaypoint.hpp"
#include "Runtime/World/CScriptWorldTeleporter.hpp"
#include "Runtime/World/CSnakeWeedSwarm.hpp"
#include "Runtime/World/CTeamAiMgr.hpp"
#include "Runtime/World/CWallCrawlerSwarm.hpp"
#include "Runtime/World/CWallWalker.hpp"

#include "Runtime/Camera/CGameCamera.hpp"
#include "Runtime/Camera/CCinematicCamera.hpp"
#include "Runtime/Camera/CFirstPersonCamera.hpp"
#include "Runtime/Camera/CInterpolationCamera.hpp"
#include "Runtime/Camera/CPathCamera.hpp"

#include "Runtime/Collision/CCollisionActor.hpp"

#include "Runtime/Weapon/CWeapon.hpp"
#include "Runtime/Weapon/CBeamProjectile.hpp"
#include "Runtime/Weapon/CBomb.hpp"
#include "Runtime/Weapon/CElectricBeamProjectile.hpp"
#include "Runtime/Weapon/CFlameThrower.hpp"
#include "Runtime/Weapon/CGameProjectile.hpp"
#include "Runtime/Weapon/CNewFlameThrower.hpp"
#include "Runtime/Weapon/CPlasmaProjectile.hpp"
#include "Runtime/Weapon/CPowerBomb.hpp"
#include "Runtime/Weapon/CTargetableProjectile.hpp"
#include "Runtime/Weapon/CWaveBuster.hpp"

#include "Runtime/MP1/World/CActorContraption.hpp"
#include "Runtime/MP1/World/CAtomicAlpha.hpp"
#include "Runtime/MP1/World/CAtomicBeta.hpp"
#include "Runtime/MP1/World/CBabygoth.hpp"
#include "Runtime/MP1/World/CBeetle.hpp"
#include "Runtime/MP1/World/CBloodFlower.hpp"
#include "Runtime/MP1/World/CBouncyGrenade.hpp"
#include "Runtime/MP1/World/CBurrower.hpp"
#include "Runtime/MP1/World/CChozoGhost.hpp"
#include "Runtime/MP1/World/CDrone.hpp"
#include "Runtime/MP1/World/CDroneLaser.hpp"
#include "Runtime/MP1/World/CElitePirate.hpp"
#include "Runtime/MP1/World/CEnergyBall.hpp"
#include "Runtime/MP1/World/CEyeball.hpp"
#include "Runtime/MP1/World/CFireFlea.hpp"
#include "Runtime/MP1/World/CFlaahgra.hpp"
#include "Runtime/MP1/World/CFlaahgraProjectile.hpp"
#include "Runtime/MP1/World/CFlaahgraTentacle.hpp"
#include "Runtime/MP1/World/CFlickerBat.hpp"
#include "Runtime/MP1/World/CFlyingPirate.hpp"
#include "Runtime/MP1/World/CGrenadeLauncher.hpp"
#include "Runtime/MP1/World/CIceAttackProjectile.hpp"
#include "Runtime/MP1/World/CIceSheegoth.hpp"
#include "Runtime/MP1/World/CJellyZap.hpp"
#include "Runtime/MP1/World/CMagdolite.hpp"
#include "Runtime/MP1/World/CMetaree.hpp"
#include "Runtime/MP1/World/CMetroid.hpp"
#include "Runtime/MP1/World/CMetroidBeta.hpp"
#include "Runtime/MP1/World/CMetroidPrimeEssence.hpp"
#include "Runtime/MP1/World/CMetroidPrimeExo.hpp"
#include "Runtime/MP1/World/CMetroidPrimeProjectile.hpp"
#include "Runtime/MP1/World/CMetroidPrimeRelay.hpp"
#include "Runtime/MP1/World/CNewIntroBoss.hpp"
#include "Runtime/MP1/World/COmegaPirate.hpp"
#include "Runtime/MP1/World/CParasite.hpp"
#include "Runtime/MP1/World/CPhazonHealingNodule.hpp"
#include "Runtime/MP1/World/CPhazonPool.hpp"
#include "Runtime/MP1/World/CPuddleSpore.hpp"
#include "Runtime/MP1/World/CPuddleToadGamma.hpp"
#include "Runtime/MP1/World/CPuffer.hpp"
#include "Runtime/MP1/World/CRidley.hpp"
#include "Runtime/MP1/World/CRipper.hpp"
#include "Runtime/MP1/World/CSeedling.hpp"
#include "Runtime/MP1/World/CShockWave.hpp"
#include "Runtime/MP1/World/CSpacePirate.hpp"
#include "Runtime/MP1/World/CSpankWeed.hpp"
#include "Runtime/MP1/World/CThardus.hpp"
#include "Runtime/MP1/World/CThardusRockProjectile.hpp"
#include "Runtime/MP1/World/CTryclops.hpp"
#include "Runtime/MP1/World/CWarWasp.hpp"

#include "Runtime/CStateManager.hpp"
#include "Runtime/GameGlobalObjects.hpp"

#include "ImGuiConsole.hpp"
#include "imgui.h"

#define IMGUI_ENTITY_INSPECT(CLS, PARENT, NAME, BLOCK)                                                                 \
  std::string_view CLS::ImGuiType() { return #NAME; }                                                                  \
  void CLS::ImGuiInspect() {                                                                                           \
    PARENT::ImGuiInspect();                                                                                            \
    if (ImGui::CollapsingHeader(#NAME))                                                                                \
      BLOCK                                                                                                            \
  }

#define BITFIELD_CHECKBOX(label, bf)                                                                                   \
  {                                                                                                                    \
    bool b = (bf);                                                                                                     \
    ImGui::Checkbox(label, &b);                                                                                        \
    if (b != (bf))                                                                                                     \
      (bf) = b;                                                                                                        \
  }

namespace metaforce {
static std::string_view FromScriptObjectState(EScriptObjectState state) {
  switch (state) {
  case EScriptObjectState::Any:
    return "Any"sv;
  case EScriptObjectState::Active:
    return "Active"sv;
  case EScriptObjectState::Arrived:
    return "Arrived"sv;
  case EScriptObjectState::Closed:
    return "Closed"sv;
  case EScriptObjectState::Entered:
    return "Entered"sv;
  case EScriptObjectState::Exited:
    return "Exited"sv;
  case EScriptObjectState::Inactive:
    return "Inactive"sv;
  case EScriptObjectState::Inside:
    return "Inside"sv;
  case EScriptObjectState::MaxReached:
    return "MaxReached"sv;
  case EScriptObjectState::Open:
    return "Open"sv;
  case EScriptObjectState::Zero:
    return "Zero"sv;
  case EScriptObjectState::Attack:
    return "Attack"sv;
  case EScriptObjectState::CloseIn:
    return "CloseIn"sv;
  case EScriptObjectState::Retreat:
    return "Retreat"sv;
  case EScriptObjectState::Patrol:
    return "Patrol"sv;
  case EScriptObjectState::Dead:
    return "Dead"sv;
  case EScriptObjectState::CameraPath:
    return "CameraPath"sv;
  case EScriptObjectState::CameraTarget:
    return "CameraTarget"sv;
  case EScriptObjectState::DeactivateState:
    return "DeactivateState"sv;
  case EScriptObjectState::Play:
    return "Play"sv;
  case EScriptObjectState::MassiveDeath:
    return "MassiveDeath"sv;
  case EScriptObjectState::DeathRattle:
    return "DeathRattle"sv;
  case EScriptObjectState::AboutToMassivelyDie:
    return "AboutToMassivelyDie"sv;
  case EScriptObjectState::Damage:
    return "Damage"sv;
  case EScriptObjectState::InvulnDamage:
    return "InvulnDamage"sv;
  case EScriptObjectState::MassiveFrozenDeath:
    return "MassiveFrozenDeath"sv;
  case EScriptObjectState::Modify:
    return "Modify"sv;
  case EScriptObjectState::ScanStart:
    return "ScanStart"sv;
  case EScriptObjectState::ScanProcessing:
    return "ScanProcessing"sv;
  case EScriptObjectState::ScanDone:
    return "ScanDone"sv;
  case EScriptObjectState::UnFrozen:
    return "Unfrozen"sv;
  case EScriptObjectState::Default:
    return "Default"sv;
  case EScriptObjectState::ReflectedDamage:
    return "ReflectedDamage"sv;
  case EScriptObjectState::InheritBounds:
    return "InheritBounds"sv;
  }
}

static std::string_view FromScriptObjectMessage(EScriptObjectMessage msg) {
  switch (msg) {
  case EScriptObjectMessage::None:
    return "None"sv;
  case EScriptObjectMessage::UNKM0:
    return "UNKM0"sv;
  case EScriptObjectMessage::Activate:
    return "Activate"sv;
  case EScriptObjectMessage::Arrived:
    return "Arrived"sv;
  case EScriptObjectMessage::Close:
    return "Close"sv;
  case EScriptObjectMessage::Deactivate:
    return "Deactivate"sv;
  case EScriptObjectMessage::Decrement:
    return "Decrement"sv;
  case EScriptObjectMessage::Follow:
    return "Follow"sv;
  case EScriptObjectMessage::Increment:
    return "Increment"sv;
  case EScriptObjectMessage::Next:
    return "Next"sv;
  case EScriptObjectMessage::Open:
    return "Open"sv;
  case EScriptObjectMessage::Reset:
    return "Reset"sv;
  case EScriptObjectMessage::ResetAndStart:
    return "ResetAndStart"sv;
  case EScriptObjectMessage::SetToMax:
    return "SetToMax"sv;
  case EScriptObjectMessage::SetToZero:
    return "SetToZero"sv;
  case EScriptObjectMessage::Start:
    return "Start"sv;
  case EScriptObjectMessage::Stop:
    return "Stop"sv;
  case EScriptObjectMessage::StopAndReset:
    return "StopAndReset"sv;
  case EScriptObjectMessage::ToggleActive:
    return "ToggleActive"sv;
  case EScriptObjectMessage::UNKM18:
    return "UNKM18"sv;
  case EScriptObjectMessage::Action:
    return "Action"sv;
  case EScriptObjectMessage::Play:
    return "Play"sv;
  case EScriptObjectMessage::Alert:
    return "Alert"sv;
  case EScriptObjectMessage::InternalMessage00:
    return "InernalMessage00"sv;
  case EScriptObjectMessage::OnFloor:
    return "OnFloor"sv;
  case EScriptObjectMessage::InternalMessage02:
    return "InternalMessage02"sv;
  case EScriptObjectMessage::InternalMessage03:
    return "InternalMessage03"sv;
  case EScriptObjectMessage::Falling:
    return "Falling"sv;
  case EScriptObjectMessage::OnIceSurface:
    return "OnIceSurface"sv;
  case EScriptObjectMessage::OnMudSlowSurface:
    return "OnMudSlowSurface"sv;
  case EScriptObjectMessage::OnNormalSurface:
    return "OnNormalSurface"sv;
  case EScriptObjectMessage::Touched:
    return "Touched"sv;
  case EScriptObjectMessage::AddPlatformRider:
    return "AddPlatformRider"sv;
  case EScriptObjectMessage::LandOnNotFloor:
    return "LandOnNotFloor"sv;
  case EScriptObjectMessage::Registered:
    return "Registered"sv;
  case EScriptObjectMessage::Deleted:
    return "Deleted"sv;
  case EScriptObjectMessage::InitializedInArea:
    return "InitializedInArea"sv;
  case EScriptObjectMessage::WorldInitialized:
    return "WorldInitialized"sv;
  case EScriptObjectMessage::AddSplashInhabitant:
    return "AddSplashInhabitant"sv;
  case EScriptObjectMessage::UpdateSplashInhabitant:
    return "UpdateSplashInhabitant"sv;
  case EScriptObjectMessage::RemoveSplashInhabitant:
    return "RemoveSplashInhabitant"sv;
  case EScriptObjectMessage::Jumped:
    return "Jumped"sv;
  case EScriptObjectMessage::Damage:
    return "Damage"sv;
  case EScriptObjectMessage::InvulnDamage:
    return "InvulnDamage"sv;
  case EScriptObjectMessage::ProjectileCollide:
    return "ProjectileCollide"sv;
  case EScriptObjectMessage::InSnakeWeed:
    return "InSnakeWeed"sv;
  case EScriptObjectMessage::AddPhazonPoolInhabitant:
    return "AddPhazonPoolInhabitant"sv;
  case EScriptObjectMessage::UpdatePhazonPoolInhabitant:
    return "UpdatePhazonPoolInhabitant"sv;
  case EScriptObjectMessage::RemovePhazonPoolInhabitant:
    return "RemovePhazonPoolInhabitant"sv;
  case EScriptObjectMessage::SuspendedMove:
    return "SuspendedMove"sv;
  }
}

std::string_view CEntity::ImGuiType() { return "Entity"; }

void CEntity::ImGuiInspect() {
  if (ImGui::CollapsingHeader("Connections")) {
    if (ImGui::BeginTable("Connections", 6,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 0, 'id');
      ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 0, 'type');
      ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0, 'name');
      ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 0, 'stat');
      ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthFixed, 0, 'msg');
      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableHeadersRow();
      for (const auto& item : x20_conns) {
        const auto uid = g_StateManager->GetIdForScript(item.x8_objId);
        CEntity* ent = g_StateManager->ObjectById(uid);
        if (ent == nullptr) {
          continue;
        }
        ImGui::PushID(uid.Value());
        ImGui::TableNextRow();
        if (ImGui::TableNextColumn()) {
          auto text = fmt::format(FMT_STRING("{:x}"), uid.Value());
          if (TCastToPtr<CActor> act = ent) {
            ImGui::Selectable(text.c_str(), &act->m_debugSelected,
                              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap);
          } else {
            ImGui::TextUnformatted(text.c_str());
          }
        }
        if (ImGui::TableNextColumn()) {
          ImGuiStringViewText(ent->ImGuiType());
        }
        if (ImGui::TableNextColumn()) {
          ImGuiStringViewText(ent->GetName());
        }
        if (ImGui::TableNextColumn()) {
          ImGuiStringViewText(FromScriptObjectState(item.x0_state));
        }
        if (ImGui::TableNextColumn()) {
          ImGuiStringViewText(FromScriptObjectMessage(item.x4_msg));
        }
        if (ImGui::TableNextColumn()) {
          if (ImGui::SmallButton("View")) {
            ImGuiConsole::inspectingEntities.insert(uid);
          }
        }
        ImGui::PopID();
      }
      ImGui::EndTable();
    }
  }
  if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("ID: %x", x8_uid.Value());
    ImGui::Text("Name: %s", x10_name.c_str());
    BITFIELD_CHECKBOX("Active", x30_24_active);
  }
}

// <- CEntity
IMGUI_ENTITY_INSPECT(CActor, CEntity, Actor, {
  const zeus::CVector3f& pos = GetTranslation();
  ImGui::Text("Position: %f, %f, %f", pos.x(), pos.y(), pos.z());
  ImGui::Checkbox("Highlight", &m_debugSelected);
})
IMGUI_ENTITY_INSPECT(MP1::CFireFlea::CDeathCameraEffect, CEntity, FireFleaDeathCameraEffect, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidPrimeRelay, CEntity, MetroidPrimeRelay, {})
IMGUI_ENTITY_INSPECT(CScriptActorKeyframe, CEntity, ScriptActorKeyframe, {})
IMGUI_ENTITY_INSPECT(CScriptActorRotate, CEntity, ScriptActorRotate, {})
IMGUI_ENTITY_INSPECT(CScriptAreaAttributes, CEntity, ScriptAreaAttributes, {})
IMGUI_ENTITY_INSPECT(CScriptCameraBlurKeyframe, CEntity, ScriptCameraBlurKeyframe, {})
IMGUI_ENTITY_INSPECT(CScriptCameraFilterKeyframe, CEntity, ScriptCameraFilterKeyframe, {})
IMGUI_ENTITY_INSPECT(CScriptCameraShaker, CEntity, ScriptCameraShaker, {})
IMGUI_ENTITY_INSPECT(CScriptColorModulate, CEntity, ScriptColorModulate, {})
IMGUI_ENTITY_INSPECT(CScriptControllerAction, CEntity, ScriptControllerAction, {})
IMGUI_ENTITY_INSPECT(CScriptCounter, CEntity, ScriptCounter, {})
IMGUI_ENTITY_INSPECT(CScriptDistanceFog, CEntity, ScriptDistanceFog, {})
IMGUI_ENTITY_INSPECT(CScriptDockAreaChange, CEntity, ScriptDockAreaChange, {})
IMGUI_ENTITY_INSPECT(CScriptGenerator, CEntity, ScriptGenerator, {})
IMGUI_ENTITY_INSPECT(CScriptHUDMemo, CEntity, ScriptHUDMemo, {})
IMGUI_ENTITY_INSPECT(CScriptMemoryRelay, CEntity, ScriptMemoryRelay, {})
IMGUI_ENTITY_INSPECT(CScriptMidi, CEntity, ScriptMidi, {})
IMGUI_ENTITY_INSPECT(CScriptPickupGenerator, CEntity, ScriptPickupGenerator, {})
IMGUI_ENTITY_INSPECT(CScriptPlayerStateChange, CEntity, ScriptPlayerStateChange, {})
IMGUI_ENTITY_INSPECT(CScriptRandomRelay, CEntity, ScriptRandomRelay, {})
IMGUI_ENTITY_INSPECT(CScriptRelay, CEntity, ScriptRelay, {})
IMGUI_ENTITY_INSPECT(CScriptRipple, CEntity, ScripleRipple, {})
IMGUI_ENTITY_INSPECT(CScriptRoomAcoustics, CEntity, ScriptRoomAcoustics, {})
IMGUI_ENTITY_INSPECT(CScriptSpawnPoint, CEntity, ScriptSpawnPoint, {})
IMGUI_ENTITY_INSPECT(CScriptStreamedMusic, CEntity, ScriptStreamedMusic, {})
IMGUI_ENTITY_INSPECT(CScriptSwitch, CEntity, ScriptSwitch, {})
IMGUI_ENTITY_INSPECT(CScriptTimer, CEntity, ScriptTimer, {})
IMGUI_ENTITY_INSPECT(CScriptWorldTeleporter, CEntity, ScriptWorldTeleporter, {})
IMGUI_ENTITY_INSPECT(CTeamAiMgr, CEntity, TeamAiMgr, {})

// <- CActor
IMGUI_ENTITY_INSPECT(CPhysicsActor, CActor, PhysicsActor, {})
IMGUI_ENTITY_INSPECT(MP1::CDroneLaser, CActor, DroneLaser, {})
IMGUI_ENTITY_INSPECT(CEffect, CActor, Effect, {})
IMGUI_ENTITY_INSPECT(CFire, CActor, Fire, {})
IMGUI_ENTITY_INSPECT(CFishCloud, CActor, FishCloud, {})
IMGUI_ENTITY_INSPECT(CFishCloudModifier, CActor, FishCloudModifier, {})
IMGUI_ENTITY_INSPECT(MP1::CFlaahgraPlants, CActor, FlaahgraPlants, {})
IMGUI_ENTITY_INSPECT(MP1::CFlaahgraRenderer, CActor, FlaahgraRenderer, {})
IMGUI_ENTITY_INSPECT(MP1::COmegaPirate::CFlash, CActor, OmegaPirateFlash, {})
IMGUI_ENTITY_INSPECT(CGameCamera, CActor, GameCamera, {})
IMGUI_ENTITY_INSPECT(CGameLight, CActor, GameLight, {})
IMGUI_ENTITY_INSPECT(MP1::CIceAttackProjectile, CActor, IceAttackProjectile, {})
IMGUI_ENTITY_INSPECT(CRepulsor, CActor, Repulsor, {})
IMGUI_ENTITY_INSPECT(CScriptAiJumpPoint, CActor, ScriptAIJumpPoint, {})
IMGUI_ENTITY_INSPECT(CScriptBeam, CActor, ScriptBeam, {})
IMGUI_ENTITY_INSPECT(CScriptCameraHint, CActor, ScriptCameraHint, {})
IMGUI_ENTITY_INSPECT(CScriptCameraHintTrigger, CActor, ScriptCameraHintTrigger, {})
IMGUI_ENTITY_INSPECT(CScriptCameraPitchVolume, CActor, ScriptCameraPitchVolume, {})
IMGUI_ENTITY_INSPECT(CScriptCameraWaypoint, CActor, ScriptCameraWaypoint, {})
IMGUI_ENTITY_INSPECT(CScriptCoverPoint, CActor, ScriptCoverPoint, {})
IMGUI_ENTITY_INSPECT(CScriptDamageableTrigger, CActor, ScriptDamageableTrigger, {})
IMGUI_ENTITY_INSPECT(CScriptDebugCameraWaypoint, CActor, ScriptDebugCameraWaypoint, {})
IMGUI_ENTITY_INSPECT(CScriptEffect, CActor, ScriptEffect, {})
IMGUI_ENTITY_INSPECT(CScriptEMPulse, CActor, ScriptEMPulse, {})
IMGUI_ENTITY_INSPECT(CScriptGrapplePoint, CActor, ScriptGrapplePoint, {})
IMGUI_ENTITY_INSPECT(CScriptMazeNode, CActor, ScriptMazeNode, {})
IMGUI_ENTITY_INSPECT(CScriptPlayerHint, CActor, ScriptPlayerHint, {})
IMGUI_ENTITY_INSPECT(CScriptPointOfInterest, CActor, ScriptPointOfInterest, {})
IMGUI_ENTITY_INSPECT(CScriptShadowProjector, CActor, ScriptShadowProjector, {})
IMGUI_ENTITY_INSPECT(CScriptSound, CActor, ScriptSound, {})
IMGUI_ENTITY_INSPECT(CScriptSpecialFunction, CActor, ScriptSpecialFunction, {})
IMGUI_ENTITY_INSPECT(CScriptSpiderBallAttractionSurface, CActor, ScriptSpiderballAttractionSurface, {})
IMGUI_ENTITY_INSPECT(CScriptSpiderBallWaypoint, CActor, ScriptSpiderBallWaypoint, {})
IMGUI_ENTITY_INSPECT(CScriptTargetingPoint, CActor, ScriptTargetingPoint, {})
IMGUI_ENTITY_INSPECT(CScriptTrigger, CActor, ScriptTrigger, {})
IMGUI_ENTITY_INSPECT(CScriptVisorFlare, CActor, ScriptVisorFlare, {})
IMGUI_ENTITY_INSPECT(CScriptVisorGoo, CActor, ScriptVisorGoo, {})
IMGUI_ENTITY_INSPECT(CScriptWaypoint, CActor, ScriptWaypoint, {})
IMGUI_ENTITY_INSPECT(MP1::CShockWave, CActor, ShockWave, {})
IMGUI_ENTITY_INSPECT(CSnakeWeedSwarm, CActor, SnakeWeedSwarm, {})
IMGUI_ENTITY_INSPECT(CWallCrawlerSwarm, CActor, WallCrawlerSwarm, {})
IMGUI_ENTITY_INSPECT(CWeapon, CActor, Weapon, {})

// <- CEffect
IMGUI_ENTITY_INSPECT(CExplosion, CEffect, Explosion, {})
IMGUI_ENTITY_INSPECT(CHUDBillboardEffect, CEffect, HUDBillboardEffect, {})
IMGUI_ENTITY_INSPECT(CIceImpact, CEffect, IceImpact, {})

// <- CGameCamera
IMGUI_ENTITY_INSPECT(CBallCamera, CGameCamera, BallCamera, {})
IMGUI_ENTITY_INSPECT(CCinematicCamera, CGameCamera, CinematicCamera, {})
IMGUI_ENTITY_INSPECT(CFirstPersonCamera, CGameCamera, FirstPersonCamera, {})
IMGUI_ENTITY_INSPECT(CInterpolationCamera, CGameCamera, InterpolationCamera, {})
IMGUI_ENTITY_INSPECT(CPathCamera, CGameCamera, PathCamera, {})
IMGUI_ENTITY_INSPECT(CScriptSpindleCamera, CGameCamera, ScriptSpindleCamera, {})

// <- CScriptTrigger
IMGUI_ENTITY_INSPECT(MP1::CPhazonPool, CScriptTrigger, PhazonPool, {})
IMGUI_ENTITY_INSPECT(CScriptBallTrigger, CScriptTrigger, ScriptBallTrigger, {})
IMGUI_ENTITY_INSPECT(CScriptSteam, CScriptTrigger, ScriptSteam, {})
IMGUI_ENTITY_INSPECT(CScriptWater, CScriptTrigger, ScriptWater, {})

// <- CWeapon
IMGUI_ENTITY_INSPECT(CBomb, CWeapon, Bomb, {})
IMGUI_ENTITY_INSPECT(CGameProjectile, CWeapon, GameProjectile, {})
IMGUI_ENTITY_INSPECT(CPowerBomb, CWeapon, PowerBomb, {})

// <- CGameProjectile
IMGUI_ENTITY_INSPECT(CBeamProjectile, CGameProjectile, BeamProjectile, {})
IMGUI_ENTITY_INSPECT(CEnergyProjectile, CGameProjectile, EnergyProjectile, {})
IMGUI_ENTITY_INSPECT(CFlameThrower, CGameProjectile, FlameThrower, {})
IMGUI_ENTITY_INSPECT(CNewFlameThrower, CGameProjectile, NewFlameThrower, {})
IMGUI_ENTITY_INSPECT(CWaveBuster, CGameProjectile, WaveBuster, {})

// <- CBeamProjectile
IMGUI_ENTITY_INSPECT(CElectricBeamProjectile, CBeamProjectile, ElectricBeamProjectile, {})
IMGUI_ENTITY_INSPECT(CPlasmaProjectile, CBeamProjectile, PlasmaProjectile, {})

// <- CEnergyProjectile
IMGUI_ENTITY_INSPECT(MP1::CFlaahgraProjectile, CEnergyProjectile, FlaahgraProjectile, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidPrimeProjectile, CEnergyProjectile, MetroidPrimeProjectile, {})
IMGUI_ENTITY_INSPECT(CTargetableProjectile, CEnergyProjectile, TargetableProjectile, {})

// <- CPhysicsActor
IMGUI_ENTITY_INSPECT(CAi, CPhysicsActor, AI, {})
IMGUI_ENTITY_INSPECT(CAmbientAI, CPhysicsActor, AmbientAI, {})
IMGUI_ENTITY_INSPECT(MP1::CBouncyGrenade, CPhysicsActor, BouncyGrenade, {})
IMGUI_ENTITY_INSPECT(CCollisionActor, CPhysicsActor, CollisionActor, {})
IMGUI_ENTITY_INSPECT(MP1::CGrenadeLauncher, CPhysicsActor, GrenadeLauncher, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidPrimeExo::CPhysicsDummy, CPhysicsActor, MetroidPrimeExoPhysicsDummy, {})
IMGUI_ENTITY_INSPECT(CPlayer, CPhysicsActor, Player, {})
IMGUI_ENTITY_INSPECT(CScriptActor, CPhysicsActor, ScriptActor, {})
IMGUI_ENTITY_INSPECT(CScriptDebris, CPhysicsActor, ScriptDebris, {})
IMGUI_ENTITY_INSPECT(CScriptDock, CPhysicsActor, ScriptDock, {})
IMGUI_ENTITY_INSPECT(CScriptDoor, CPhysicsActor, ScriptDoor, {})
IMGUI_ENTITY_INSPECT(CScriptGunTurret, CPhysicsActor, ScriptGunTurret, {})
IMGUI_ENTITY_INSPECT(CScriptPickup, CPhysicsActor, ScriptPickup, {})
IMGUI_ENTITY_INSPECT(CScriptPlatform, CPhysicsActor, ScriptPlatform, {})

// <- CScriptActor
IMGUI_ENTITY_INSPECT(MP1::CActorContraption, CScriptActor, ActorContraption, {})
IMGUI_ENTITY_INSPECT(CScriptPlayerActor, CScriptActor, PlayerActor, {})

// <- CScriptPlatform
IMGUI_ENTITY_INSPECT(MP1::CRipperControlledPlatform, CScriptPlatform, RipperControlledPlatform, {})

// <- CAi
IMGUI_ENTITY_INSPECT(CDestroyableRock, CAi, DestroyableRock, {})
IMGUI_ENTITY_INSPECT(CPatterned, CAi, Patterned,
                     { BITFIELD_CHECKBOX("Enable state machine", x403_25_enableStateMachine); })

// <- CPatterned
IMGUI_ENTITY_INSPECT(MP1::CAtomicAlpha, CPatterned, AtomicAlpha, {})
IMGUI_ENTITY_INSPECT(MP1::CAtomicBeta, CPatterned, AtomicBeta, {})
IMGUI_ENTITY_INSPECT(MP1::CBabygoth, CPatterned, Babygoth, {})
IMGUI_ENTITY_INSPECT(MP1::CBeetle, CPatterned, Beetle, {})
IMGUI_ENTITY_INSPECT(MP1::CBloodFlower, CPatterned, BloodFlower, {})
IMGUI_ENTITY_INSPECT(MP1::CBurrower, CPatterned, Burrower, {})
IMGUI_ENTITY_INSPECT(MP1::CChozoGhost, CPatterned, ChozoGhost, {})
IMGUI_ENTITY_INSPECT(MP1::CDrone, CPatterned, Drone, {})
IMGUI_ENTITY_INSPECT(MP1::CElitePirate, CPatterned, ElitePirate, {})
IMGUI_ENTITY_INSPECT(MP1::CEnergyBall, CPatterned, EnergyBall, {})
IMGUI_ENTITY_INSPECT(MP1::CEyeball, CPatterned, EyeBall, {})
IMGUI_ENTITY_INSPECT(MP1::CFireFlea, CPatterned, FireFlea, {})
IMGUI_ENTITY_INSPECT(MP1::CFlaahgra, CPatterned, Flaahgra, {})
IMGUI_ENTITY_INSPECT(MP1::CFlaahgraTentacle, CPatterned, FlaahgraTentacle, {})
IMGUI_ENTITY_INSPECT(MP1::CFlickerBat, CPatterned, FlickerBat, {})
IMGUI_ENTITY_INSPECT(MP1::CFlyingPirate, CPatterned, FlyingPirate, {})
IMGUI_ENTITY_INSPECT(MP1::CIceSheegoth, CPatterned, IceSheegoth, {})
IMGUI_ENTITY_INSPECT(MP1::CJellyZap, CPatterned, JellyZap, {})
IMGUI_ENTITY_INSPECT(MP1::CMagdolite, CPatterned, Magdolite, {})
IMGUI_ENTITY_INSPECT(MP1::CMetaree, CPatterned, Metaree, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroid, CPatterned, Metroid, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidBeta, CPatterned, MetroidBeta, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidPrimeEssence, CPatterned, MetroidPrimeEssence, {})
IMGUI_ENTITY_INSPECT(MP1::CMetroidPrimeExo, CPatterned, MetroidPrimeExo, {})
IMGUI_ENTITY_INSPECT(MP1::CNewIntroBoss, CPatterned, NewIntroBoss, {})
IMGUI_ENTITY_INSPECT(MP1::CPhazonHealingNodule, CPatterned, PhazonHealingNodule, {})
IMGUI_ENTITY_INSPECT(MP1::CPuddleSpore, CPatterned, PuddleSpore, {})
IMGUI_ENTITY_INSPECT(MP1::CPuddleToadGamma, CPatterned, PuddleToadGamma, {})
IMGUI_ENTITY_INSPECT(MP1::CPuffer, CPatterned, Puffer, {})
IMGUI_ENTITY_INSPECT(MP1::CRidley, CPatterned, Ridley, {})
IMGUI_ENTITY_INSPECT(MP1::CRipper, CPatterned, Ripper, {})
IMGUI_ENTITY_INSPECT(MP1::CSpacePirate, CPatterned, SpacePirate, {})
IMGUI_ENTITY_INSPECT(MP1::CSpankWeed, CPatterned, SpankWeed, {})
IMGUI_ENTITY_INSPECT(MP1::CThardus, CPatterned, Thardus, {})
IMGUI_ENTITY_INSPECT(MP1::CThardusRockProjectile, CPatterned, ThardusRockProjectile, {})
IMGUI_ENTITY_INSPECT(MP1::CTryclops, CPatterned, Tryclops, {})
IMGUI_ENTITY_INSPECT(CWallWalker, CPatterned, WallWalker, {})
IMGUI_ENTITY_INSPECT(MP1::CParasite, CWallWalker, Parasite, {})
IMGUI_ENTITY_INSPECT(MP1::CSeedling, CWallWalker, Seedling, {})
IMGUI_ENTITY_INSPECT(MP1::CWarWasp, CPatterned, WarWasp, {})
IMGUI_ENTITY_INSPECT(MP1::COmegaPirate, MP1::CElitePirate, OmegaPirate, {})
} // namespace metaforce
