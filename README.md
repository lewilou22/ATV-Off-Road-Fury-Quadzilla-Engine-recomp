# ATV-Off-Road-Fury-Quadzilla-Engine-recomp
ATV Off Road Fury Recomp

We are using https://github.com/hkmodd/ps2-recomp-Agent-SKILL  to do the heavy lifting . 

WIP , Findings can be found below 

<img width="640" height="448" alt="image" src="https://github.com/user-attachments/assets/5400ca07-d779-4dbf-b600-e464cc1195ea" />


Session Log

2026-09-07





Actions: Pointed Ghidra 11.4.2 at existing Microsoft JDK 21 (JAVA_HOME_OVERRIDE). Did not change system JAVA_HOME (still JDK 25).



Actions: Cloned ps2-recomp-Agent-SKILL and PS2Recomp. Extracted USA v1.00 ISO from the user zip. Pulled SYSTEM.CNF + SCUS_971.04. Parsed ELF headers/symbols.



Discovered: Fully symbolicated CodeWarrior C++ binary (3845 functions). Rainbow Studios Quadzilla engine. clang-cl missing. No PS2Recomp build dir.



Actions: Configured PS2Recomp\build64 (Ninja + MSVC cl 19.50 + Release, user-approved). Built ps2_analyzer.exe and ps2_recomp.exe (exit 0).



Actions: Ran ps2_analyzer on SCUS_971.04 → game\SCUS_971.04.toml (exit 0, ~198 KB). 3847 functions, 11276 symbols, 412 stubs, 346 untracked library-like names, 37 self-mod patches, skip=[].



Actions: Pass-1 TOML: single_file_output = true, cleared 37 SMC nops. Ran ps2_recomp (exit 0). 3434 recompiled / 413 stubs / 0 skipped / 0 decode failures / 0 unhandled opcodes. 1222 JR/JALR fallback warnings (C++ vtables). Output: game\output\ps2_recompiled_functions.cpp (84 MB) + register_functions.cpp (21 MB).



Actions: First boot of ps2EntryRunner.exe (20s). Window opened (640x448). ELF ran. IOP loaded sio2man/padman/mtapman/mcman/mcserv/libsd. Unhandled pad RPC 0x80000901/0x80000903. Then spun on sceCdRead LBN 0xbc3 — no CD image configured.



Actions: Auto-detect CD image in configureIoPathsFromElf (parent of ELF / PS2X_CD_IMAGE). Incremental rebuild. Boot 3 used game\ATV Offroad Fury (USA) (v1.00).iso. CD spin gone. Still running at 20s; unhandled IOP RPC 0x80000901/0x80000903 and 0x123456 @ 0x100ef0.



Actions: Core IOP HLE for MTAPMAN (0x80000901–905) and 989SND (0x123456). Boot 4: still running at 20s, stderr is only the CD-image line (no unhandled RPC).



Actions: Unblocked intro FMV drain: sceMpegGetPicture no longer waits on an empty FFmpeg queue; SCUS override restores DecodeBitstream this; host vsync increments 0x36C3D0 (stand-in for VSyncTimerHandler). Boot 14 presents 639×448 with ~286k non-black pixels (PS2Recomp\logs\gs_frame_live.png) — swizzled red/blue pattern, not a real movie frame. EE then goes dormant at pc=0.



Current Blocker: After Game::PrepareFrame / PSXVideoCard::Flip, the EE thread becomes dormant at pc=0. Displayed frame is swizzled/garbage FMV, not a title screen. Do not --clean-first.

2026-09-08





Actions: Native __floatdisf / fptosi at entry; wrap litodp to salvage $ra; replaceFunction every dense-table slot. Scheduler prefers last usable PC over GameObject::Tick. Boot 20: no helper pc=0.



Actions: RemoveFromChain +0x1C walk was poison 0x802030e0 (not a real long list). Break invalid next without writing kernel-looking addrs. Boot 22: GIF climbs (151→6964), Flip/PrepareFrame loop stays alive.



Actions: Forced PSXMediaControl::IsPlaying false after 90 Flips (MPEG stub never sets +0x80/+0x7C). Boot 23: TurnThePage then Blt 0x1e3664 ra=0. Reverted the skip.



Actions: ISO9660 lookup in registerCdFile when the loose cdRoot file is missing; real disc LBN; fioOpen / IOP host extract to extracted\.cdcache. Do not treat the .iso as a host file with a relative LBN.



Actions: Capture intro PSXMediaControl from Create/Initialize (ui\rainbow.pss) and set stop flags on that object only after 180 Flips. Boot 25: SAFETY ALERT splash, LoadMainIcons, GIF still moving then freeze in FadeOut.



Actions: FadeOut wait uses software vsync at 0x35F108 (not 0x36C3D0). Host frame now increments both. Boot 26/28: FadeOut finishes; SlaveMainMenu::Create/Tick run (ui\intro.pss, ui\backgrnd.pss).



Actions: After menu create, pulse DualShock CROSS via setPadOverrideState (active-low ~bit14). Wrap Track/Rider/LoadScreen/LoadScene. FadeIn wait slots mirrored from FadeOut (0x1f7850/0x1f7948/0x1f7954). AddChild cycle now restores s0/ra/sp and links the new child instead of jumping to a missing 0x164dc8 slot.



Discovered: Boot 41 CROSS → SlaveMainMenu::KeyDown → TurnThePage 14 → SlaveLoadScreen::Create (no Track/Rider menus). LoadScreen never Ticks; LoadScene not reached. Jumping AddChild to ra without stack restore (boot 40) executed garbage PCs (0x802030e0). Do not do that.



Actions: HLE zcalloc/zcfree as MMI move + host calloc/free, pc=$ra. Do not salvage LoadScreen Create pc to GUIMaster $ra (boot 52 reached LoadScene that way with a half-built object). Create wrapper pump of inflate livelocks at 0x2be9c8 (checkpoint every timeslice). Disabled GUIMaster idle-resume after race load starts.



Actions: Boot 59 still left Create at pc=0x2c0060. Boot 60: skip dispatch checkpoint on zcalloc/zcfree and drain leftover Create PCs under g_ps2HoldEeTimeslices. Create returned to 0x175de8; Tick + LoadScene + LoadTrackDefinition ran; scene AddChild spam; GIF still climbing (~8500) at ~frame 6000. StartRacers not seen. clang-cl still missing. EE Reloaded + GhydraMCP copied into Ghidra 11.4.2 Extensions; PCSX2-MCP extracted. User still needs to enable the Ghidra plugins and launch the DebugServer pcsx2-qt.exe.



Actions: PCSX2 DebugServer connected with ATV ELF running (Flip wait 0x1f65fc). Boot 61: OpenFile scenes\national\nat20.scn v0=1; LoadScene/LoadTrackDefinition v0=1; QuadzillaRaceEvent::Tick runs. StartRacers not reached — Tick skips that path when *( *(0x2D1C10)+0x1B8 ) != 0.



Discovered: Live PCSX2 tutorial has mgr+0x1B8=0 and mode object *(mgr+0x370)+0xC == 4. StartRacers (0x21dab0) has a single call site at Tick 0x20fa4c and is skipped when that mode is 4. Training starts the player by writing first bike+0xA98=1 at TrainingRaceEvent::Tick 0x287c70. Boot 63: busy cleared, mode=0x4, still no StartRacers. Do not set PCSX2 BPs on KeyDown/Tick. Do not salvage LoadScreen Create pc to GUIMaster $ra.



Actions: Boot 64 wraps TrainingRaceEvent::Tick 0x287840: one-shot +0x918 so RelocateQuad runs, then set first bike +0xA98=1 (training equivalent of StartRacers).



Actions: Boot 65: TrainingRaceEvent::Tick runs (this=0x4f7f30, racers/bike valid). Forced bike+0xA98=1. EE stays alive, GIF still climbing. LoadTrackDefinition returned v0=0 (yield 0x2be9c8). Present is a checkerboard, not a 3D view. Do not BP KeyDown/Tick in PCSX2.



Actions: Boot 66 drains LoadTrackDefinition / LoadScene / LoadTerrain / AttachScene under g_ps2HoldEeTimeslices so inflate/ReadFaceTree mid-slots can finish (boot 65 left LoadTrack at inflate_fast 0x2be9c8). Keep PCSX2 training as ground truth; no KeyDown/Tick BPs.



Actions: Boot 66: LoadTerrain/LoadTrackDefinition/AttachScene all v0=1. AttachScene returned to Training Create 0x287268. Bike +0xA98=1. EE alive, GIF climbing. Present is no longer a flat checkerboard: foggy scene + title overlay + GS noise. Do not BP KeyDown/Tick.



Actions: Boot 67: ReadFaceTree self-jal skip at 0x11d8b4 let LoadScene reach ReadVertexTree 0x11dc5c, then drain treated a nested jr ra as stuck. GIF froze at 8067.



Actions: Drain now continues when pc is unchanged but $sp moved (goto-recursion + C++ return from jr ra). Do not skip second tree children.



Discovered: Boot 68: LoadScene leave pc=0x20df9c (Create fallthrough) spins=514. Track/scene attached. Render3D runs with vis(this+0x38)=0, which beqzs the whole 3D body. GIF ~9k.



Actions: Boot 70: set this+0x38=1 once track+scene look like heap. Render3D vis=0x1. GIF climbs ~9k → 42k and keeps rising. Present nonzero still ~283k (nearly full 640×448). Heartbeat often at GameObject::Render3D 0x1651cc. Boot 69 flaked: LoadScreen Tick then EE pc=0 (same as boot 64). Do not BP KeyDown/Tick.



Discovered (live racing 2026-09-08): Terrain 0x6CD840, D1C=0x88, tile +0x80 mixed 0/1/0xF, mesh at +0x88, tile DMA +0x90=0x30000003, Terrain DMA +0xE30=0x30000016. PSXRenderTarget+0x80=1. gp skip-init 0x2F5F00..0x2F5F10 and 0x2F5F9C are 1. Do not treat RaceEvent+0x38 as the 3D enable.



Actions: Boot 82: RenderNearClip ~8/frame, RenderPlain ~300–400/frame, meshes/DMA tags match live. GIF still crawls. Boot 83: AddToRenderQueue rt80=1 (matches live), qadd climbs; SendRenderQueue/TrySend run (sendF=1). Leftover FB still nonzero≈283465.



Actions: Boot 84 wrapped DmaSend/BeginScene + always-on send redirect → EE dormant after LoadScreen Tick. Boot 85 extra +0x90/+0x9C reads before Send during load → same hang. Boot 86: redirect + +0x174 bit 4 only after Terrain::Create; training reached; qadd 165→3782; GIF 9704→9975; leftover FB unchanged. Boot 87 flush-Send from Terrain::Render3D hung load. Boot 88 took ui\rainbow.pss at page 14 then dormant.



Actions: Boot 89 (no pad): training. Post-Create Send which=2 c9c=0x143 src94=0x4ce280 f174=0x1e (redirect fired), then which=1 c90=0x2f. GIF 9681→9960, leftover FB nonzero=283465. Queue submit is done; remaining hole is GIF DMA / VU1 PATH1 of those chains.



Discovered: Game DmaSend enum is ch0=GIF, ch1=VIF0, ch2=VIF1. Terrain is VIF UNPACK + MSCAL PC 0xC8 + VU1 XGKICK PATH1. Boot 90: first filled VIF1 send tags=324 bytes=279376 mpg=21 unpack=1223 mscal=142; XGKICK fires; vif= climbs; bike +0xA98=1.



Actions: Boot 91/92 repeating or late CROSS + page-14 rainbow.pss during FadeOut → EE pc=0. Boot 93: CROSS at flip 310, skip page-14 rainbow, FadeOut leave, training + XGKICK. Dump showed untextured quad silhouette + HUD (L2 Look Back, Thrill Camera [R2]).



Actions: Boot 94 holds analog ly=0x00 + R1 (not R2). bike+0x70 world position moves (~2 units in Z, Y bouncing). Present of display FBP 0/140 went near-black this boot; physics is live.



Discovered (boot 97–99): Training PNG nonzero can be LSB noise (maxRgb=1, bright=0). Boot 93 nonzero≈277k was leftover menu, not a live 3D view. HOST→LOCAL blit does hit TBP 0x3480 (256×256, dpsm=0) during the menu. Display PATH1 XGKICK geom XYZ is all 0x7FFF (GS 2048,2048); first 4096 post-XGKICK prims deg=4096 real=0. Later real untextured strips draw to offscreen FBP 420 (fbw=4, ofx/ofy=1920). Next: VU1 transform / UNPACK of the camera-to-GS 12.4 convert (not more pad). Do not force analogMode.



Discovered (boot 100): First VIF1 MSCAL pc=0xc8 is top=0 with n320=0 n2048=0; XGKICK XYZ all 0x7FFF. Second MSCAL top=0x8d has 320.0 at VU qword 20 and 2048.0 at qword 23 (after-mscal xyz0=(6785,34690)). Same-frame TrySend c90=0x143 c9c=0 — tile chain in which=1, camera which=2 empty. Live PCSX2 had camera/viewport on the other queue. Do not BP KeyDown/Tick.



Discovered (boot 101): Camera 0x2f is sent after the 0x143 tile chain. Viewport is a matrix q20=(320,0,0,0) q21=(0,-224,0,0) q23=(2048,2048,…). First MSCAL still BASE=0 OFFSET=0. Later XGKICK XYZ is real (~2062,2021) and GS rasters it to FBP 420 (ofx/ofy=1920, rgba=0,0,0,128).



Actions: Boot 102/103 CROSS during MPEG → LoadScreen Tick dormant. Boot 104 FadeIn-gate expired the Create+24 press window (no CROSS). Pulse now starts at menu FadeIn+8. Do not press the DualShock during agent boots.



Discovered (boot 105–106): Display FRAME does switch to FBP 0/140 fbw=10 ofx=1728 ofy=1824. Real display strips are tme=0 rgba=(0,0,0,128) in a ~20px cluster at GS (2062,2020) = screen ~(334,196). Training PNG bright=0. Next hole is VU1 camera/vertex transform (world collapsed to clip origin), not missing PATH1 to the display FB.



Discovered (boot 108–109): execLower already runs bit-31 specials (XTOP 0x800106bc). First 24 pairs of pc=0xc8 are setup + BAL imm=992 → 0x2038. V4-16 verts ITOF0 to ~41000. Second MSCAL ran 65536 cycles from a leftover m_cycle and was cut off (afterhex still 7FFF).



Actions (boot 110): Reset m_cycle in execute(), always flushPipelines(), MSCAL budget 262144. Programs finish (cyc≈109k, no budget log). Training PNG is a solid sky gray-blue (#7585a1, bright=286720, maxRgb=158) — 3D clear/fog is live. Mesh still a 20px black blob at GS 2062. Next: clip-space matrix / ITOF scale for those V4-16 verts (q0–q3 is scale-3 identity; q8/q12 are zero).

2026-09-09





Discovered: MultiplyMatrix 0x124A90 is VU0 COP2. Host 4×4 HLE is the D3D row-vector product (M * inv(M) ≈ I). InvertMatrix 0x1149B0 is also COP2. Terrain uploads 0x36EF20 (scale-3), then Camera+0x240 / +0x280. Those slots are filled by PSXCamera::UpdateMatrices 0x1DD3C0 COP2 (viewproj * viewport and view * proj), not Camera::UpdateMatrices +0xB0/+0x70/+0xF0.



Actions (boot 111–113): HLE MultiplyMatrix / InvertMatrix / SetViewParameters vector copies. Host look-at written onto Camera+0x240 from Terrain::Render3D using gfx+4. Boot 112 applied that to the menu camera with a 2048 “viewport” and missed training in 90s. Boot 113 reached training: +0x240 is a real look-at (t.z≈3056, rotation off-diagonal), not -2048. Early PATH1 prims sit at GS 4095 (tme=1) because that MSCAL’s q20–q23 were identity. Training PNG at dump time is black; later uploads show tens of thousands of maxRgb=255 pixels (do not treat 283465 as a 3D win). Do not press the DualShock during agent boots.



Discovered (boot 114–117): Viewport q20–q23 restored. Training cam +0x180 is at=(0,-1,0) (dirAt=1), +0x1A0 copies eye — not a world look-at point. Releasing R2 does not change that. Native PSXCamera::UpdateMatrices writes +0x240 = viewproj * viewport and +0x280 = view * proj. Baking viewport into +0x240 made q8.x=544.8 (1.70*320) and fog PATH1 spread on FBP 140 (GS 7–3904), but look-down puts m[5]=-2048 and no vert lands in the OFX window [1728,2368]. Display dump is FBP 0 bright=0. CreateViewMatrix treats at as a point (at-eye); SetViewParameters COP2 on at/up is still memcpy. Do not write +0x70/+0xB0/+0x130. Do not hold R2 during training.



Discovered (boot 119): FollowCamera jal SetViewParameters sets a2=a3=0 (por from $zero). Native skips null at/up. +0x180 ctor default is not R2. bike+0x70 starts at origin then relocates under the camera.



Actions (boot 120): HLE CreateViewMatrix 0x1B7990 as LookAt (at as point). Host ProjectionMatrix uses tan(fov/2) and m11 = m00 * aspect from +0x1C0 (live m00≈0.839). Prefer native +0xB0 when it has translation. Training view m00≈0.044, |q8.x|≈1463 vs live ~1480, t.z≈3055. PATH1 in OFX at GS (2051,2129) but still a black sliver (bright=0). Bike +0x70 moves (2198,89,2115). Next: why those OFX tris stay maxRgb=1 (texture / VU vert scale / dump FBP 0 vs draw 140), not another look-down HLE.



Discovered (boot 122): Display PATH1 PRIM is fge=1 fog≈175 fogcol=(0,0,0) tfx=3 tpsm=0x2 tbp0=13440. Skip FGE while FOGCOL is 0 (boot 121 blit-spam missed training). FBP 140 still maxRgb=1; FBP 0 dump shows faint TIME/LAP HUD. First tris remain a ~20px cluster. Do not log FMV 16×16 HOST→LOCAL every 64 transfers.



Discovered (live + boot 123–124): Native CreateViewMatrix copies at into the view Z axis (LookTo), not at-eye. FollowCamera 0x11EA00 writes +0x180 = normalize(lookPoint-eye) with VU0 vrsqrt. Recomp never ran that COP2, so +0x180 stayed (0,-1,0) and terrain collapsed / looked down. HLE 0x11EA00 + LookTo: boot 124 dir matches live ~45° down, |q8.x|≈1468, PATH1 spans the GS. FBP 140 nonzero≈273k still maxRgb=1. Next is color (fog=255 / HIGHLIGHT2 / tme=0 fill), not clip-scale.



Actions (boot 125–126): Native SetRenderState(D3DRS_FOGCOLOR=0x22) is a no-op; wrap Fog::Render3D writes GS FOGCOL from Fog+0x30 (0xFF76869E / #76869e). HLE ProjectionMatrix 0x1B7830 (projOk=1, q12 XY filled). Training CT16 HOST→LOCAL to 0x3600–0x36e0 (includes 0x3680). Boot 126 FGE/tme1 writes 262k pixels rgb≈(77,78,78) maxRgb=255, then presented FBP 140 is maxRgb=1 (later non-FGE pass). Do not treat training unpause + PrepareFrame 0x163748 full-bright as the 3D view.

