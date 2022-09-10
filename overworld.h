#pragma once
#include "zelda_rtl.h"
#include "variables.h"

const uint8 *GetMap8toTileAttr();
const uint16 *GetMap16toMap8Table();
bool LookupInOwEntranceTab(uint16 r0, uint16 r2);
int LookupInOwEntranceTab2(uint16 pos);
bool CanEnterWithTagalong(int e);
int DirToEnum(int dir);
void Overworld_ResetMosaicDown();
void Overworld_Func1D();
void Overworld_Func1E();
uint16 Overworld_GetSignText(int area);
const uint8 *GetOverworldSpritePtr(int area);
uint8 GetOverworldBgPalette(int idx);
void Sprite_LoadGraphicsProperties();
void Sprite_LoadGraphicsProperties_light_world_only();
void InitializeMirrorHDMA();
void MirrorWarp_BuildWavingHDMATable();
void MirrorWarp_BuildDewavingHDMATable();
void TakeDamageFromPit();
void Module08_OverworldLoad();
void PreOverworld_LoadProperties();
void AdjustLinkBunnyStatus();
void ForceNonbunnyStatus();
void RecoverPositionAfterDrowning();
void Module0F_SpotlightClose();
void Dungeon_PrepExitWithSpotlight();
void Spotlight_ConfigureTableAndControl();
void OpenSpotlight_Next2();
void Module10_SpotlightOpen();
void Module10_00_OpenIris();
void SetTargetOverworldWarpToPyramid();
void ResetAncillaAndCutscene();
void Module09_Overworld();
void OverworldOverlay_HandleRain();
void Module09_00_PlayerControl();
void OverworldHandleTransitions();
void Overworld_LoadGFXAndScreenSize();
void ScrollAndCheckForSOWExit();
void Module09_LoadAuxGFX();
void Overworld_FinishTransGfx();
void Module09_LoadNewMapAndGFX();
void Overworld_RunScrollTransition();
void Module09_LoadNewSprites();
void Overworld_StartScrollTransition();
void Overworld_EaseOffScrollTransition();
void Module09_0A_WalkFromExiting_FacingDown();
void Module09_0B_WalkFromExiting_FacingUp();
void Module09_09_OpenBigDoorFromExiting();
void Overworld_DoMapUpdate32x32_B();
void Module09_0C_OpenBigDoor();
void Overworld_DoMapUpdate32x32_conditional();
void Overworld_DoMapUpdate32x32();
void Overworld_StartMosaicTransition();
void Overworld_LoadOverlays();
void PreOverworld_LoadOverlays();
void Overworld_LoadOverlays2();
void Module09_FadeBackInFromMosaic();
void Overworld_Func1C();
void OverworldMosaicTransition_LoadSpriteGraphicsAndSetMosaic();
void Overworld_Func22();
void Overworld_Func18();
void Overworld_Func19();
void Module09_MirrorWarp();
void MirrorWarp_FinalizeAndLoadDestination();
void Overworld_DrawScreenAtCurrentMirrorPosition();
void MirrorWarp_LoadSpritesAndColors();
void Overworld_Func2B();
void Overworld_WeathervaneExplosion();
void Module09_2E_Whirlpool();
void Overworld_Func2F();
void Module09_2A_RecoverFromDrowning();
void Module09_2A_00_ScrollToLand();
void Overworld_OperateCameraScroll();
int OverworldCameraBoundaryCheck(int xa, int ya, int vd, int r8);
int OverworldScrollTransition();
void Overworld_SetCameraBoundaries(int big, int area);
void Overworld_FinalizeEntryOntoScreen();
void Overworld_Func1F();
void ConditionalMosaicControl();
void Overworld_ResetMosaic_alwaysIncrease();
void Overworld_SetSongList();
void LoadOverworldFromDungeon();
void Overworld_LoadNewScreenProperties();
void LoadCachedEntranceProperties();
void Overworld_EnterSpecialArea();
void LoadOverworldFromSpecialOverworld();
void FluteMenu_LoadTransport();
void Overworld_LoadBirdTravelPos(int k);
void FluteMenu_LoadSelectedScreenPalettes();
void FindPartnerWhirlpoolExit();
void Overworld_LoadAmbientOverlay(bool load_map_data);
void Overworld_LoadAmbientOverlayFalse();
void Overworld_LoadAndBuildScreen();
void Module08_02_LoadAndAdvance();
void Overworld_DrawQuadrantsAndOverlays();
void Overworld_HandleOverlaysAndBombDoors();
void TriggerAndFinishMapLoadStripe_Y(int n);
void TriggerAndFinishMapLoadStripe_X(int n);
void SomeTileMapChange();
void CreateInitialNewScreenMapToScroll();
void CreateInitialOWScreenView_Big_North();
void CreateInitialOWScreenView_Big_South();
void CreateInitialOWScreenView_Big_West();
void CreateInitialOWScreenView_Big_East();
void CreateInitialOWScreenView_Small_North();
void CreateInitialOWScreenView_Small_South();
void CreateInitialOWScreenView_Small_West();
void CreateInitialOWScreenView_Small_East();
void OverworldTransitionScrollAndLoadMap();
uint16 *BuildFullStripeDuringTransition_North(uint16 *dst);
uint16 *BuildFullStripeDuringTransition_South(uint16 *dst);
uint16 *BuildFullStripeDuringTransition_West(uint16 *dst);
uint16 *BuildFullStripeDuringTransition_East(uint16 *dst);
void OverworldHandleMapScroll();
uint16 *CheckForNewlyLoadedMapAreas_North(uint16 *dst);
uint16 *CheckForNewlyLoadedMapAreas_South(uint16 *dst);
uint16 *CheckForNewlyLoadedMapAreas_West(uint16 *dst);
uint16 *CheckForNewlyLoadedMapAreas_East(uint16 *dst);
uint16 *BufferAndBuildMap16Stripes_X(uint16 *dst);
uint16 *BufferAndBuildMap16Stripes_Y(uint16 *dst);
void Overworld_DecompressAndDrawAllQuadrants();
void Overworld_DecompressAndDrawOneQuadrant(uint16 *dst, int screen);
void Overworld_ParseMap32Definition(uint16 *dst, uint16 input);
void OverworldLoad_LoadSubOverlayMap32();
void LoadOverworldOverlay();
void Map16ToMap8(const uint8 *src, int r20);
void OverworldCopyMap16ToBuffer(const uint8 *src, uint16 r20, int r14, uint16 *r10);
void MirrorBonk_RecoverChangedTiles();
void DecompressEnemyDamageSubclasses();
int Decompress_bank02(uint8 *dst, const uint8 *src);
uint8 Overworld_ReadTileAttribute(uint16 x, uint16 y);
void Overworld_SetFixedColAndScroll();
void Overworld_Memorize_Map16_Change(uint16 pos, uint16 value);
void HandlePegPuzzles(uint16 pos);
void GanonTowerEntrance_Func1();
void Overworld_CheckSpecialSwitchArea();
const uint16 *Overworld_GetMap16OfLink_Mult8();
void Palette_AnimGetMasterSword();
void Palette_AnimGetMasterSword2();
void Palette_AnimGetMasterSword3();
void Overworld_DwDeathMountainPaletteAnimation();
void Overworld_LoadEventOverlay();
void Ancilla_TerminateWaterfallSplashes();
void Overworld_GetPitDestination();
void Overworld_UseEntrance();
uint16 Overworld_ToolAndTileInteraction(uint16 x, uint16 y);
void Overworld_PickHammerSfx(uint16 a);
uint16 Overworld_GetLinkMap16Coords(Point16U *xy);
uint8 Overworld_HandleLiftableTiles(Point16U *pt_arg);
uint8 Overworld_LiftingSmallObj(uint16 a, uint16 pos, uint16 y, Point16U pt);
int Overworld_SmashRockPile(bool down_one_tile, Point16U *pt);
uint8 SmashRockPile_fromLift(uint16 a, uint16 pos, uint16 y, Point16U pt);
void Overworld_BombTiles32x32(uint16 x, uint16 y);
void Overworld_BombTile(int x, int y);
void Overworld_AlterWeathervane();
void OpenGargoylesDomain();
void CreatePyramidHole();
uint16 Overworld_RevealSecret(uint16 pos);
void AdjustSecretForPowder();
void Overworld_DrawMap16_Persist(uint16 pos, uint16 value);
void Overworld_DrawMap16(uint16 pos, uint16 value);
void Overworld_AlterTileHardcore(uint16 pos, uint16 value);
uint16 Overworld_FindMap16VRAMAddress(uint16 addr);
void Overworld_AnimateEntrance();
void Overworld_AnimateEntrance_PoD();
void Overworld_AnimateEntrance_Skull();
void Overworld_AnimateEntrance_Mire();
void Overworld_AnimateEntrance_TurtleRock();
void OverworldEntrance_PlayJingle();
void OverworldEntrance_DrawManyTR();
void Overworld_AnimateEntrance_GanonsTower();
void OverworldEntrance_AdvanceAndBoom();
