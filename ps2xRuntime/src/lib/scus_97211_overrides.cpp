#include "game_overrides.h"
#include "ps2_runtime.h"
#include "runtime/ee_scheduler.h"
#include "ps2_recompiled_functions.h"

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

extern uint32_t g_ps2GuestVsyncCounterAddr;
extern uint32_t g_ps2GuestVsyncCounterAddr2;
extern uint32_t g_ps2SkipSceGsSyncVWait;
extern uint32_t g_ps2Pc0ResumeRangeBegin;
extern uint32_t g_ps2Pc0ResumeRangeEnd;
extern uint32_t g_ps2Pc0ResumeTarget;
extern uint32_t g_ps2IdleResumePc;
extern uint32_t g_ps2IdleResumeA0;
extern uint32_t g_ps2HoldEeTimeslices;

namespace
{
    // Screen object configuration parser (0x00204390..0x002043B4 prologue).
    // Rejoin the generated body at 0x002043B8.
    void screenConfig204390(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x150u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &ra, sizeof(ra));
        const auto s1 = GPR_VEC(ctx, 17);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &s1, 16);
        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp), &s0, 16);
        SET_GPR_U64(ctx, 17, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 16, GPR_U64(ctx, 5));
        SET_GPR_U64(ctx, 4, GPR_U64(ctx, 5));
        SET_GPR_U32(ctx, 5, 0x00429E80u);
        SET_GPR_U32(ctx, 31, 0x002043B8u);
        ctx->pc = 0x00141810u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00141810u,
                0x002043B0u, 0x002043B8u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x002043B8u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Highlight_Icon configuration initializer (0x2AD6C0..0x2AD6E0 prologue).
    // Rejoin the generated body at 0x2AD6E4.
    void highlightInit2ad6c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x1D0u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x40u), &ra, sizeof(ra));
        const auto s3 = GPR_VEC(ctx, 19);
        std::memcpy(getMemPtr(rdram, sp + 0x30u), &s3, 16);
        const auto s2 = GPR_VEC(ctx, 18);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &s2, 16);
        const auto s1 = GPR_VEC(ctx, 17);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &s1, 16);
        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp), &s0, 16);
        SET_GPR_U64(ctx, 19, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 18, GPR_U64(ctx, 6));
        SET_GPR_U32(ctx, 31, 0x002AD6E4u);
        ctx->pc = 0x00234FB0u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00234FB0u,
                0x002AD6DCu, 0x002AD6E4u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x002AD6E4u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Animated_Icon configuration initializer (0x2AE600..0x2AE610 prologue).
    // Rejoin the generated body at 0x2AE614.
    void iconInit2ae600(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x20u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &ra, sizeof(ra));
        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp), &s0, 16);
        SET_GPR_U64(ctx, 16, GPR_U64(ctx, 4));
        SET_GPR_U32(ctx, 31, 0x002AE614u);
        ctx->pc = 0x00239420u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00239420u,
                0x002AE60Cu, 0x002AE614u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x002AE614u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Leaf accessor at 0x126FC0: swc1 f12, 0x24(a0); jr ra.
    void leafStoreF12_126fc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *)
    {
        const float f = ctx->f[12];
        std::memcpy(getMemPtr(rdram, getRegU32(ctx, 4) + 0x24u), &f, sizeof(f));
        ctx->pc = getRegU32(ctx, 31);
    }

    // Overlay_TextInput configuration initializer (0x2364C0..0x2364E0 prologue).
    // Rejoin the generated body at 0x2364E4.
    void textInputInit2364c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x2B0u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x50u), &ra, sizeof(ra));
        const auto s3 = GPR_VEC(ctx, 19);
        std::memcpy(getMemPtr(rdram, sp + 0x40u), &s3, 16);
        const auto s2 = GPR_VEC(ctx, 18);
        std::memcpy(getMemPtr(rdram, sp + 0x30u), &s2, 16);
        const auto s1 = GPR_VEC(ctx, 17);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &s1, 16);
        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &s0, 16);
        SET_GPR_U64(ctx, 19, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 18, GPR_U64(ctx, 6));
        SET_GPR_U32(ctx, 31, 0x002364E4u);
        ctx->pc = 0x00234FB0u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00234FB0u,
                0x002364DCu, 0x002364E4u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x002364E4u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Slider configuration initializer (0x2AE950..0x2AE968 prologue).
    // Rejoin the generated body at 0x2AE96C.
    void sliderInit2ae950(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x80u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &ra, sizeof(ra));
        const auto s1 = GPR_VEC(ctx, 17);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &s1, 16);
        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp), &s0, 16);
        SET_GPR_U64(ctx, 17, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 16, GPR_U64(ctx, 6));
        SET_GPR_U32(ctx, 31, 0x002AE96Cu);
        ctx->pc = 0x00239420u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00239420u,
                0x002AE964u, 0x002AE96Cu, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x002AE96Cu;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Resource read wrapper verified by PCSX2: pass its allocated word buffer
    // and byte count to the archive reader, preserving the file handle in a1.
    void resourceRead1ce020(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t object = getRegU32(ctx, 4);
        uint32_t count = 0, buffer = 0, archive = 0;
        std::memcpy(&count, getMemPtr(rdram, object + 0x10u), 4);
        std::memcpy(&buffer, getMemPtr(rdram, object + 0x14u), 4);
        std::memcpy(&archive, getMemPtr(rdram, 0x004975B0u), 4);
        SET_GPR_S32(ctx, 2, static_cast<int32_t>(count));
        SET_GPR_S32(ctx, 1, 0x00490000);
        SET_GPR_S32(ctx, 6, static_cast<int32_t>(buffer));
        SET_GPR_S32(ctx, 8, 1);
        SET_GPR_S32(ctx, 4, static_cast<int32_t>(archive));
        SET_GPR_S32(ctx, 7, static_cast<int32_t>(count << 2));
        ctx->pc = 0x001806C0u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // PCSX2: j 0x1D0890; lw a0,0x30(a0) (delay slot).
    void resourceForward177ae0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        uint32_t object = 0;
        std::memcpy(&object, getMemPtr(rdram, getRegU32(ctx, 4) + 0x30u), 4);
        SET_GPR_S32(ctx, 4, static_cast<int32_t>(object));
        ctx->pc = 0x001D0890u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Registry flags wrapper: preserve existing flags before the tail call.
    void registryFlags1d2fa0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        uint32_t records = 0, flags = 0;
        std::memcpy(&records, getMemPtr(rdram, getRegU32(ctx, 4) + 0x1Cu), 4);
        SET_GPR_S32(ctx, 3, static_cast<int32_t>(records));
        std::memcpy(&flags, getMemPtr(rdram, records + (getRegU32(ctx, 5) << 4) + 8u), 4);
        SET_GPR_S32(ctx, 2, static_cast<int32_t>(flags));
        SET_GPR_U64(ctx, 6, GPR_U64(ctx, 6) | GPR_U64(ctx, 2));
        ctx->pc = 0x001AE5F0u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Name registry entry, verified with both MCPs. Rejoin the generated
    // search loop or its first allocation return, retaining the guest frame.
    void nameRegistry1ae800(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x60u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x50u), &ra, sizeof(ra));
        for (unsigned reg = 16; reg <= 20; ++reg)
        {
            const auto saved = GPR_VEC(ctx, reg);
            std::memcpy(getMemPtr(rdram, sp + (reg - 16u) * 0x10u), &saved, 16);
        }
        SET_GPR_U64(ctx, 20, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 19, GPR_U64(ctx, 5));
        SET_GPR_U64(ctx, 18, GPR_U64(ctx, 6));
        SET_GPR_U64(ctx, 17, 0);
        SET_GPR_U64(ctx, 16, 0);
        int32_t count = 0;
        std::memcpy(&count, getMemPtr(rdram, getRegU32(ctx, 4) + 0x10u), sizeof(count));
        SET_GPR_U64(ctx, 2, count > 0 ? 1 : 0);
        if (count > 0)
        {
            ctx->pc = 0x001AE834u;
        }
        else
        {
            SET_GPR_U32(ctx, 31, 0x001AE89Cu);
            ctx->pc = 0x001AE4C0u;
            if (!runtime->dispatchGuestBranch(rdram, ctx, 0x001AE4C0u,
                    0x001AE894u, 0x001AE89Cu, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
                return;
            ctx->pc = 0x001AE89Cu;
        }
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // PCSX2 native decode: ld v0,0(a0); daddiu v0,1; jr ra; sd v0,0(a0).
    void addRef107460(uint8_t *rdram, R5900Context *ctx, PS2Runtime *)
    {
        uint64_t count = 0;
        uint8_t *object = getMemPtr(rdram, getRegU32(ctx, 4));
        std::memcpy(&count, object, sizeof(count));
        ++count;
        SET_GPR_U64(ctx, 2, count);
        std::memcpy(object, &count, sizeof(count));
        ctx->pc = getRegU32(ctx, 31);
    }

    // Vertex attribute callback: restore the missed prologue and reuse the
    // generated position/color/UV updates beginning at 0x3425B0.
    void vertexUpdate342590(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x30u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &ra, sizeof(ra));
        for (unsigned reg = 16; reg <= 17; ++reg)
        {
            const auto saved = GPR_VEC(ctx, reg);
            std::memcpy(getMemPtr(rdram, sp + (reg - 16u) * 0x10u), &saved, 16);
        }
        SET_GPR_U64(ctx, 17, GPR_U64(ctx, 4));
        uint32_t object = 0;
        std::memcpy(&object, getMemPtr(rdram, getRegU32(ctx, 4) + 0x30u), sizeof(object));
        SET_GPR_S32(ctx, 4, static_cast<int32_t>(object));
        SET_GPR_U64(ctx, 16, GPR_U64(ctx, 6));
        SET_GPR_U32(ctx, 31, 0x003425B0u);
        ctx->pc = 0x003423F0u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x003423F0u,
                0x003425A8u, 0x003425B0u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x003425B0u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // MCP-verified wrapper: call 0x1C0930, then return 1. Reuse its
    // generated epilogue so allocator/scheduler yields retain the guest frame.
    void resourceHook1bd4f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x10u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp), &ra, sizeof(ra));
        SET_GPR_U32(ctx, 31, 0x001BD500u);
        ctx->pc = 0x001C0930u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x001C0930u,
                0x001BD4F8u, 0x001BD500u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x001BD500u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Ghydra and PCSX2 agree on 0x14F610..0x14F658. Only this prologue
    // lacks an entry; the generated body already resumes at 0x14F65C.
    void resourceInit14f610(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0x70u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));
        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x60u), &ra, sizeof(ra));
        for (unsigned reg = 16; reg <= 21; ++reg)
        {
            const auto saved = GPR_VEC(ctx, reg);
            std::memcpy(getMemPtr(rdram, sp + (reg - 16u) * 0x10u), &saved, 16);
        }
        SET_GPR_U64(ctx, 21, GPR_U64(ctx, 4));
        SET_GPR_U64(ctx, 20, GPR_U64(ctx, 5));
        const uint32_t count = getRegU32(ctx, 6);
        std::memcpy(getMemPtr(rdram, getRegU32(ctx, 4) + 0x34u), &count, sizeof(count));
        SET_GPR_S32(ctx, 3, static_cast<int32_t>(count));
        SET_GPR_S32(ctx, 2, static_cast<int32_t>(count * 33u));
        SET_GPR_S32(ctx, 4, static_cast<int32_t>(count * 0x108u));
        SET_GPR_U32(ctx, 5, 0x0041F068u);
        SET_GPR_U32(ctx, 6, 0x524u);
        SET_GPR_U32(ctx, 31, 0x0014F65Cu);
        ctx->pc = 0x00103C20u;
        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x00103C20u,
                0x0014F654u, 0x0014F65Cu, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;
        ctx->pc = 0x0014F65Cu;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    void ctor44c100(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)ctx;
        (void)runtime;

        uint8_t *dst = getMemPtr(rdram, 0x00461750u);
        if (dst == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor44c100: cannot resolve guest dest 0x461750" << std::endl;
            return;
        }

        // FUN_0044c100 writes 4x128-bit const quads {w,x,y,z} at 0x461750..0x46177F:
        // {0,0,0,1} {1,0,0,1} {0,1,0,1} {0,0,1,1} (float 1.0 = 0x3F800000).
        static const uint32_t kMatrixQuads[16] = {
            0x00000000u, 0x00000000u, 0x00000000u, 0x3F800000u,
            0x3F800000u, 0x00000000u, 0x00000000u, 0x3F800000u,
            0x00000000u, 0x3F800000u, 0x00000000u, 0x3F800000u,
            0x00000000u, 0x00000000u, 0x3F800000u, 0x3F800000u,
        };
        std::memcpy(dst, kMatrixQuads, sizeof(kMatrixQuads));
    }

    // default_new_handler__3stdFv @ 0x34d4b0 — called from __nw__FUi (operator new)
    // when allocation fails; address stored in data table at 0x4107A0.
    // Generated code sets up frame, loads v0=0x46ED0, calls 34F210 + 34E090, loops, returns via jr $ra.
    // Runtime policy = SkipCallDebug; we return 1 (non-null handler -> retry alloc) and advance PC via ra.
    void default_new_handler_34d4b0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;

        // Return non-zero (valid handler) in v0 ($2) to indicate "retry allocation"
        SET_GPR_U32(ctx, 2, 1u);

        // CRITICAL: advance PC via return address (ra = $31), otherwise infinite loop
        const uint32_t entryPc = ctx->pc;
        if (ctx->pc == entryPc)
        {
            ctx->pc = getRegU32(ctx, 31);
        }
    }

    // Session-lock enter @ 0x341e60 (vtable slot [8], JALR'd from the stream/IO ctor
    // path: 0x341da0 -> ... -> 0x341f90). Analyzer's jal heuristic missed the boundary.
    // Object layout (u32 offsets): +0x4 mutex sema, +0x8 holder id (-1 = free),
    // +0xC refcount, +0x10 wait sema (WaitsSema target), +0x14 waiter count.
    // Helper twins: func_0x00341e40 = WaitSema(+0x4), func_0x00341e50 = SignalSema(+0x4).
    // Retry path: release mutex, WaitSema(+0x10), re-acquire, retry.
    void sessionLockEnter341e60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t objAddr = getRegU32(ctx, 4);
        const uint32_t newId = getRegU32(ctx, 5);

        uint32_t *obj = reinterpret_cast<uint32_t *>(getMemPtr(rdram, objAddr));
        if (obj == nullptr)
        {
            std::cerr << "[SCUS_972.11] sessionLockEnter341e60: cannot resolve guest obj 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        static unsigned enterReports = 0;
        if (enterReports++ < 10)
        {
            std::cerr << "[sessionLockEnter341e60] obj=0x" << std::hex << objAddr
                      << " newId=0x" << newId << " mutex=" << obj[1] << " holder=0x" << obj[2]
                      << " refcount=" << obj[3] << " waitSema=" << obj[4] << " waiters=" << obj[5]
                      << " ra=0x" << getRegU32(ctx, 31) << " sp=0x" << getRegU32(ctx, 29)
                      << " pc=0x" << ctx->pc << std::dec << std::endl;
        }

        EeScheduler &ee = runtime->eeScheduler();
        ee.bindMainContextForSyscall(*ctx, rdram);

        const int mutexSema = static_cast<int>(obj[1]); // +0x4
        const int waitSema = static_cast<int>(obj[4]);  // +0x10

        ee.waitSemaphore(mutexSema);
        for (;;)
        {
            const uint32_t holder = obj[2]; // +0x8
            if (holder == 0xFFFFFFFFu)
            {
                obj[2] = newId;
                ++obj[3]; // +0xC refcount
                break;
            }
            if (holder == newId)
            {
                ++obj[3];
                break;
            }

            // Held by another session: release the mutex, block on the per-object
            // wait semaphore until the holder signals, re-acquire, retry.
            static unsigned contentionReports = 0;
            if (contentionReports++ < 4)
            {
                std::cerr << "[SCUS_972.11] lock-contention obj=0x" << std::hex << objAddr
                          << " holder=0x" << holder << " requested=0x" << newId
                          << " refcount=0x" << obj[3] << " ra=0x" << getRegU32(ctx, 31)
                          << " sp=0x" << getRegU32(ctx, 29) << std::dec << std::endl;
            }
            ++obj[5]; // +0x14 waiter count
            ee.signalSemaphore(mutexSema, false);
            ee.waitSemaphore(waitSema);
            ee.waitSemaphore(mutexSema);
            --obj[5];
        }
        ee.signalSemaphore(mutexSema, false);
        ctx->pc = getRegU32(ctx, 31);
    }

    // Session-lock leave @ 0x341f30 (vtable slot [c], twin of 0x341e60; JALR'd from
    // 0x341ff8 after the enter wrapper FUN_00341f90). Also missed by the analyzer.
    // Decoded: WaitSema(+0x4); --refcount(+0xC); if 0: if waiters(+0x14)>0
    // SignalSema(+0x10); holder(+0x8)=-1; SignalSema(+0x4).
    void sessionLockLeave341f30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint32_t *obj = reinterpret_cast<uint32_t *>(getMemPtr(rdram, objAddr));
        if (obj == nullptr)
        {
            std::cerr << "[SCUS_972.11] sessionLockLeave341f30: cannot resolve guest obj 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        static unsigned leaveReports = 0;
        if (leaveReports++ < 10)
        {
            std::cerr << "[sessionLockLeave341f30] obj=0x" << std::hex << objAddr
                      << " mutex=" << obj[1] << " holder=0x" << obj[2]
                      << " refcount=" << obj[3] << " waitSema=" << obj[4] << " waiters=" << obj[5]
                      << " ra=0x" << getRegU32(ctx, 31) << " sp=0x" << getRegU32(ctx, 29)
                      << " pc=0x" << ctx->pc << std::dec << std::endl;
        }

        EeScheduler &ee = runtime->eeScheduler();
        ee.bindMainContextForSyscall(*ctx, rdram);

        const int mutexSema = static_cast<int>(obj[1]); // +0x4
        const int waitSema = static_cast<int>(obj[4]);  // +0x10

        ee.waitSemaphore(mutexSema);
        const uint32_t remaining = obj[3] - 1u; // +0xC refcount
        obj[3] = remaining;
        if (remaining == 0u)
        {
            if (obj[5] > 0u) // +0x14 waiter count
            {
                ee.signalSemaphore(waitSema, false);
            }
            obj[2] = 0xFFFFFFFFu; // +0x8 holder free
        }
        ee.signalSemaphore(mutexSema, false);
        ctx->pc = getRegU32(ctx, 31);
    }

    // Base vector ctor @ 0x11a3d0: `sqc2 vf00,(a0); jr ra; dmove v0,a0` — writes the
    // VF00 {0,0,0,1} 128-bit quad to the object and returns it. Called in a loop by
    // array ctor FUN_0034d0d0 (16 elements, stride 0x10, base 0x461d30). Analyzer
    // merged the one-instruction body into the following function.
    void ctor11a3d0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint8_t *dst = getMemPtr(rdram, objAddr);
        if (dst == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor11a3d0: cannot resolve guest dest 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        // VF00 = {0, 0, 0, 1.0f} (0x3F800000), 16 bytes.
        static const uint32_t kVf0Quad[4] = {0u, 0u, 0u, 0x3F800000u};
        std::memcpy(dst, kVf0Quad, sizeof(kVf0Quad));

        SET_GPR_U32(ctx, 2, objAddr); // dmove v0, a0
        ctx->pc = getRegU32(ctx, 31);
    }

    // 4×VF00-quad ctor @ 0x16b5e0: writes VF00 {0,0,0,1} to obj+0x20..+0x5F (4 quads).
    // Called by array ctor FUN_0034d0d0 (base 0x4891b0, stride 0x80).
    void ctor16b5e0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint8_t *dst = getMemPtr(rdram, objAddr);
        if (dst == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor16b5e0: cannot resolve guest dest 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        static const uint32_t kVf0Quad[4] = {0u, 0u, 0u, 0x3F800000u};
        for (int i = 0; i < 4; ++i)
        {
            std::memcpy(dst + 0x20 + i * 0x10, kVf0Quad, sizeof(kVf0Quad));
        }

        SET_GPR_U32(ctx, 2, objAddr); // dmove v0, a0
        ctx->pc = getRegU32(ctx, 31);
    }

    // 2×VF00-quad ctor @ 0x2d1f10: `sqc2 vf00,(a0); addiu v0,a0,0x10; sqc2 vf00,(v0); jr ra; dmove v0,a0`
    // Writes VF00 {0,0,0,1} to obj+0x00 and obj+0x10 (2 quads, 32 bytes), returns obj in v0.
    void ctor2d1f10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint8_t *dst = getMemPtr(rdram, objAddr);
        if (dst == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor2d1f10: cannot resolve guest dest 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        static const uint32_t kVf0Quad[4] = {0u, 0u, 0u, 0x3F800000u};
        std::memcpy(dst, kVf0Quad, sizeof(kVf0Quad));
        std::memcpy(dst + 0x10u, kVf0Quad, sizeof(kVf0Quad));

        SET_GPR_U32(ctx, 2, objAddr); // dmove v0, a0
        ctx->pc = getRegU32(ctx, 31);
    }

    // Element ctor @ 0x137a10: zeroes +0x0/+0x8/+0xC/+0x10, clears bit0 of byte+0x4
    // (flag arg is discarded — reads $zero — per original MIPS), returns a0.
    void ctor137a10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint8_t *p = getMemPtr(rdram, objAddr);
        if (p == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor137a10: cannot resolve guest dest 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        std::memset(p, 0, 4);      // +0x0
        p[4] = static_cast<uint8_t>(p[4] & 0xFEu); // +0x4 bit0 clear
        std::memset(p + 8, 0, 12); // +0x8..+0x13

        SET_GPR_U32(ctx, 2, objAddr); // dmove v0, a0
        ctx->pc = getRegU32(ctx, 31);
    }

    // No-op accessor @ 0x153970: `jr ra; dmove v0, a2` — returns its third argument.
    // Virtual no-op stub missed by the analyzer (merged into the next function).
    void noopRetA2_153970(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, getRegU32(ctx, 6)); // v0 = a2
        ctx->pc = getRegU32(ctx, 31);
    }

    // Element ctor @ 0x331690: calls 0x325580(obj + 4) which zeroes +4..+7, clears bit 0 of +8,
    // zeroes +12..+23 (24-byte element struct). Merged into sub_00331400 without a table slot.
    void ctor331690(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t objAddr = getRegU32(ctx, 4);

        uint8_t *p = getMemPtr(rdram, objAddr + 4u);
        if (p == nullptr)
        {
            std::cerr << "[SCUS_972.11] ctor331690: cannot resolve guest dest 0x"
                      << std::hex << objAddr << std::dec << std::endl;
            return;
        }

        std::memset(p, 0, 4);                      // obj+4..obj+7
        p[4] = static_cast<uint8_t>(p[4] & 0xFEu); // obj+8 bit0 clear
        std::memset(p + 8, 0, 12);                 // obj+12..obj+23

        SET_GPR_U32(ctx, 2, objAddr);              // v0 = a0 (returns object pointer)
        ctx->pc = getRegU32(ctx, 31);
    }

    using RecompiledFn = void (*)(uint8_t *, R5900Context *, PS2Runtime *);
    using RecompiledFunction = PS2Runtime::RecompiledFunction;
    uint32_t callRecompiled4(PS2Runtime *runtime, uint8_t *rdram, R5900Context *callerCtx,
                             uint32_t fnAddr, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
    {
        RecompiledFn fn = runtime->lookupFunction(fnAddr);
        if (fn == nullptr)
        {
            std::cerr << "[SCUS_972.11] callRecompiled4: helper 0x" << std::hex << fnAddr
                      << " not found in table" << std::dec << std::endl;
            return 0u;
        }

        R5900Context scratch = *callerCtx;
        SET_GPR_U32(&scratch, 4, a1);
        SET_GPR_U32(&scratch, 5, a2);
        SET_GPR_U32(&scratch, 6, a3);
        SET_GPR_U32(&scratch, 7, a4);
        fn(rdram, &scratch, runtime);
        return getRegU32(&scratch, 2);
    }

    // Lock wrapper constructor @ 0x341f90:
    // a0 = wrapper struct, a1 = holder/session ID, a2 = lock object
    // Stores a1 at 0(a0), a2 at 4(a0). If a2 != 0, calls lock's enterLock method (slot [2]).
    // If a2 == 0, safely skips calling method on NULL object.
    // Returns wrapper in v0.
    void lockWrapperEnter341f90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t wrapperAddr = getRegU32(ctx, 4); // a0
        const uint32_t holderId = getRegU32(ctx, 5);    // a1
        const uint32_t lockAddr = getRegU32(ctx, 6);    // a2

        uint32_t *wrapper = reinterpret_cast<uint32_t *>(getMemPtr(rdram, wrapperAddr));
        if (wrapper != nullptr)
        {
            wrapper[0] = holderId;
            wrapper[1] = lockAddr;
        }

        if (lockAddr != 0u)
        {
            const uint32_t *lockObj = reinterpret_cast<const uint32_t *>(getMemPtr(rdram, lockAddr));
            if (lockObj != nullptr && lockObj[0] != 0u)
            {
                const uint32_t *vtable = reinterpret_cast<const uint32_t *>(getMemPtr(rdram, lockObj[0]));
                if (vtable != nullptr && vtable[2] != 0u)
                {
                    RecompiledFunction enterFn = runtime->lookupFunction(vtable[2]);
                    if (enterFn != nullptr)
                    {
                        SET_GPR_U32(ctx, 4, lockAddr);
                        SET_GPR_U32(ctx, 5, holderId);
                        enterFn(rdram, ctx, runtime);
                    }
                }
            }
        }

        SET_GPR_U32(ctx, 2, wrapperAddr); // v0 = wrapper
        ctx->pc = getRegU32(ctx, 31);
    }

    // Lock wrapper destructor @ 0x341fd0:
    // a0 = wrapper struct, a1 = flags
    // If wrapper != 0 and wrapper->lock != 0, calls lock's leaveLock method (slot [3]).
    // If (flags & 1) calls node/object free helper 0x103E70(wrapper).
    // Returns wrapper in v0.
    void lockWrapperLeave341fd0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t wrapperAddr = getRegU32(ctx, 4); // a0
        const uint32_t flags = getRegU32(ctx, 5);       // a1

        if (wrapperAddr != 0u)
        {
            const uint32_t *wrapper = reinterpret_cast<const uint32_t *>(getMemPtr(rdram, wrapperAddr));
            if (wrapper != nullptr)
            {
                const uint32_t holderId = wrapper[0];
                const uint32_t lockAddr = wrapper[1];
                if (lockAddr != 0u)
                {
                    const uint32_t *lockObj = reinterpret_cast<const uint32_t *>(getMemPtr(rdram, lockAddr));
                    if (lockObj != nullptr && lockObj[0] != 0u)
                    {
                        const uint32_t *vtable = reinterpret_cast<const uint32_t *>(getMemPtr(rdram, lockObj[0]));
                        if (vtable != nullptr && vtable[3] != 0u)
                        {
                            RecompiledFunction leaveFn = runtime->lookupFunction(vtable[3]);
                            if (leaveFn != nullptr)
                            {
                                SET_GPR_U32(ctx, 4, lockAddr);
                                SET_GPR_U32(ctx, 5, holderId);
                                leaveFn(rdram, ctx, runtime);
                            }
                        }
                    }
                }
            }
            if ((flags & 1u) != 0u)
            {
                RecompiledFunction freeFn = runtime->lookupFunction(0x00103E70u);
                if (freeFn != nullptr)
                {
                    SET_GPR_U32(ctx, 4, wrapperAddr);
                    freeFn(rdram, ctx, runtime);
                }
            }
        }

        SET_GPR_U32(ctx, 2, wrapperAddr);
        ctx->pc = getRegU32(ctx, 31);
    }

    // Mode/display vtable method @ 0x155950 — allocated by the display-modes vtable
    // (slot [4] of the mode object at s0+0x34+0x18). Analyzer missed the IndirectCall JALR.
    // Creates 2 display-mode nodes via 0x103C80, inits via 0x1A8C20/CC0/DE0, computes a
    // capability flag at this+0x1C, returns 1.
    void modeDisplayInit155950(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t thisObj = getRegU32(ctx, 4);
        uint8_t *thisBase = getMemPtr(rdram, thisObj);
        if (thisBase == nullptr)
        {
            std::cerr << "[SCUS_972.11] modeDisplayInit155950: cannot resolve guest this 0x"
                      << std::hex << thisObj << std::dec << std::endl;
            return;
        }

        const uint32_t nameAddr = 0x0042f590u; // "vu0_stats" / display-mode tag

        const uint32_t node0 = callRecompiled4(runtime, rdram, ctx, 0x103C80u,
                                               0x40u, 0x80u, nameAddr, 0x3FBu);
        auto *ws = reinterpret_cast<uint32_t *>(thisBase);
        ws[0x189C / 4] = node0;

        const uint32_t node1 = callRecompiled4(runtime, rdram, ctx, 0x103C80u,
                                               0x40u, 0x80u, nameAddr, 0x3FCu);
        ws[0x18A4 / 4] = node1;

        callRecompiled4(runtime, rdram, ctx, 0x1A8C20u, 0u, 0u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x1A8CC0u, node0, 0u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x1A8DE0u, 0u, 0u, 0u, 0u);

        uint32_t flag = 0u;
        if (node0 != 0u)
        {
            const uint8_t *nb = getMemPtr(rdram, node0);
            if (nb != nullptr && (nb[2] != 0 || nb[3] != 0))
            {
                flag = 1u;
            }
        }
        ws[0x1C / 4] = flag;

        SET_GPR_U32(ctx, 2, 1u); // return 1
    }

    // Node-lookup wrapper @ 0x187880: `dmove a0,a1; li a3,0xE; dmove a1,a2; j 0x103D80`
    // with a2=0x4209a0 — tail-calls the node-registry get (mem_alloc).
    // Original MIPS:
    //   daddu $a0, $a1, $zero (items)
    //   addiu $a3, $zero, 0xE (line = 14)
    //   daddu $a1, $a2, $zero (size)
    //   lui   $a2, 0x42
    //   j     func_103D80
    //   addiu $a2, $a2, 0x9A0 (filename = 0x004209A0)
    void nodeLookup187880(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t items = getRegU32(ctx, 5); // orig a1
        const uint32_t size = getRegU32(ctx, 6);  // orig a2
        SET_GPR_U32(ctx, 4, items);
        SET_GPR_U32(ctx, 5, size);
        SET_GPR_U32(ctx, 6, 0x004209A0u);
        SET_GPR_U32(ctx, 7, 0xEu);
        ctx->pc = 0x00103D80u;
        RecompiledFunction fn = runtime->lookupFunction(0x00103D80u);
        if (fn != nullptr)
        {
            fn(rdram, ctx, runtime);
        }
        else
        {
            std::cerr << "[nodeLookup187880] helper 0x103D80 not found!" << std::endl;
        }
    }

    // Node-release wrapper @ 0x1878a0:
    // Original MIPS:
    //   daddu $a0, $a1, $zero (ptr)
    //   addiu $a2, $zero, 0x13 (line = 19)
    //   lui   $a1, 0x42
    //   j     func_103DF0
    //   addiu $a1, $a1, 0x9A0 (filename = 0x004209A0)
    void nodeRelease1878a0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t ptr = getRegU32(ctx, 5); // orig a1
        SET_GPR_U32(ctx, 4, ptr);
        SET_GPR_U32(ctx, 5, 0x004209A0u);
        SET_GPR_U32(ctx, 6, 0x13u);
        ctx->pc = 0x00103DF0u;
        RecompiledFunction fn = runtime->lookupFunction(0x00103DF0u);
        if (fn != nullptr)
        {
            fn(rdram, ctx, runtime);
        }
        else
        {
            std::cerr << "[nodeRelease1878a0] helper 0x103DF0 not found!" << std::endl;
        }
    }

    // VBlank-chain vtable method @ 0x156e80 (slot [3] of obj+0x50 table), reached via
    // JALR from 0x1538ac inside FUN_00153680. Merged into generated sub_00156C80 but
    // its re-entry switch lacks case 0x156e80u. PCSX2-native MIPS decode (verified 2026-09-15):
    //
    //   addiu sp,-0x30; dmove s0,a0; sw s0,0x489038 (lui at,0x0049 + sw -0x6FC8)
    //   this+0x20=0x22B; this+0xC=2
    //   node0 = 0x103C80(0x40, 0x00020000, 0x0041F950, 0x22E);  this+0x5C = node0
    //   node1 = 0x103C80(0x40, 0x00020000, 0x0041F950, 0x230);  this+0x60=node0 (delay), this+0x68=node1
    //   ctx = this+0x90;  this+0x6C = node0 (delay of first 0x127560 call)
    //   0x127560(ctx, 1, 0x80)   — param-list start (cmd=1, init data=0x80)
    //   0x127B30(ctx, 0x1100); 0x127B30(ctx,0x0300); 0x127B30(ctx,0x0200)
    //   0x127960(ctx, 0x320, 0xC) — begin DMA header (returns index)
    //   stack tag (16B, zeroed via 0x36CB20(buf,0,0x10)) then byte-fiddled to:
    //       {0x01,0x80,0,0, 0,0x40,0,0x10, 0x0F,0,...}  (GIF-tag-like: NLOOP=1 EOP=1 PRIM FLG...)
    //   0x127870(ctx, buf) — append 16-byte tag to the command list
    //   0x1279E0(ctx)        — write DMA command word
    //   0x127B30(ctx,0x1400); 0x127B30(ctx,0x1000)
    //   0x127620(ctx)        — build GIF-tag header at ctx+0x10
    //   0x1588B0(this+0x78, 0x10, 0x10) — alloc vector at this+0x78
    //   this+0x4=0x280; this+0x8=0x1C0
    //   res = 0x103E30(0x20, 0x0041F950, 0x251)  — allocator dispatch
    //   if (res != 0) 0x187270(res, 0xC, 1)      — descriptor node on the buffer
    //   this+0xD4 = res
    //   this+0x24 = 0x3F800000 (delay of 0x1570D0 call)
    //   0x1570D0(this)                            — alloc 3 timer buffers (+0xC8/CC/D0)
    //   0x157510(this, 2, 0x003BF540, 0)          — VIF1 DMA kick #1
    //   0x156760(1, 1)                            — VIF1 wait/tick
    //   0x157510(this, 2, 0x003C0600, 0)          — VIF1 DMA kick #2
    //   0x156760(1, 1)                            — VIF1 wait/tick
    //   return this in v0
    void vtableMethod156e80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t thisObj = getRegU32(ctx, 4);
        uint8_t *thisBase = getMemPtr(rdram, thisObj);
        if (thisBase == nullptr)
        {
            std::cerr << "[SCUS_972.11] vtableMethod156e80: cannot resolve guest this 0x"
                      << std::hex << thisObj << std::dec << std::endl;
            return;
        }

        // sw s0, 0x489038 (lui at,0x0049; sw s0,-0x6FC8(at))
        uint8_t *guestGlobal = getMemPtr(rdram, 0x489038u);
        if (guestGlobal == nullptr)
        {
            std::cerr << "[SCUS_972.11] vtableMethod156e80: cannot resolve guest global 0x489038"
                      << std::endl;
            return;
        }
        std::memcpy(guestGlobal, &thisObj, sizeof(thisObj));

        auto *ws = reinterpret_cast<uint32_t *>(thisBase);
        ws[0x20 / 4] = 0x22Bu; // li v0,0x22B; sw v0,0x20(a0)
        ws[0xC / 4] = 0x2u;    // li v0,0x2; sw v0,0xC(a0)

        // 2 display nodes via 0x103C80. a1 = lui 0x0002 = 0x00020000;
        // a2 = lui 0x0042 + addiu -0x6B0 = 0x0041F950; a3 = 0x22E / 0x230.
        const uint32_t kTag = 0x0041F950u;
        const uint32_t kA1 = 0x00020000u;

        const uint32_t node0 = callRecompiled4(runtime, rdram, ctx, 0x103C80u,
                                               0x40u, kA1, kTag, 0x22Eu);
        ws[0x5C / 4] = node0; // this+0x5C (after first call)

        const uint32_t node1 = callRecompiled4(runtime, rdram, ctx, 0x103C80u,
                                               0x40u, kA1, kTag, 0x230u);
        ws[0x60 / 4] = node0; // delay slot of 2nd jal: stores OLD v0 (node0)
        ws[0x68 / 4] = node1; // this+0x68 (after 2nd call returns)

        // Context list at this+0x90
        const uint32_t ctxAddr = thisObj + 0x90u;
        const uint32_t ctxListAddr = ctxAddr;

        // this+0x6C = node0 — delay slot of the 0x127560 jal (lw v0,0x5C(s0))
        ws[0x6C / 4] = node0;

        // 0x127560(ctx, 1, 0x80): param-list start (cmd=a1, init=a2).
        callRecompiled4(runtime, rdram, ctx, 0x127560u, ctxListAddr, 0x1u, 0x80u, 0u);

        // 0x127B30 context values
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, ctxListAddr, 0x1100u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, ctxListAddr, 0x0300u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, ctxListAddr, 0x0200u, 0u, 0u);

        // 0x127960(ctx, 0x320, 0xC): begin DMA header.
        callRecompiled4(runtime, rdram, ctx, 0x127960u, ctxListAddr, 0x320u, 0xCu, 0u);

        // Stack GIF-tag: zero a 16-byte guest scratch below the caller's sp (the original
        // fn allocated its own 0x30 frame; writing below sp is safe in this runtime model),
        // apply the exact byte fiddle from the MIPS, then append it via 0x127870(ctx, buf).
        const uint32_t sp = getRegU32(ctx, 29);
        const uint32_t tagBufAddr = (sp - 0x20u) & ~0xFu;
        uint8_t *tagBuf = getMemPtr(rdram, tagBufAddr);
        if (tagBuf == nullptr)
        {
            std::cerr << "[SCUS_972.11] vtableMethod156e80: cannot resolve guest tag buf 0x"
                      << std::hex << tagBufAddr << std::dec << std::endl;
            return;
        }
        std::memset(tagBuf, 0, 0x10); // 0x36CB20(buf, 0, 0x10)

        uint16_t t0 = static_cast<uint16_t>(tagBuf[0] | (tagBuf[1] << 8));
        uint16_t t5 = static_cast<uint16_t>((t0 & 0x8000u) | 0x1u); // lhu + andi/ori
        tagBuf[0] = static_cast<uint8_t>(t5 & 0xFFu);
        tagBuf[1] = static_cast<uint8_t>(t5 >> 8);

        tagBuf[5] = static_cast<uint8_t>((tagBuf[5] & ~0x40u) | 0x40u);   // lbu 0x25(sp)
        tagBuf[1] = static_cast<uint8_t>((tagBuf[1] & ~0x80u) | 0x80u);   // lbu 0x21(sp)
        tagBuf[7] = static_cast<uint8_t>((tagBuf[7] & ~0xCu) | 0x0u);     // lbu 0x27(sp), clear bits 2-3
        tagBuf[7] = static_cast<uint8_t>((tagBuf[7] & ~0xF0u) | 0x10u);   // set bits 4
        tagBuf[8] = static_cast<uint8_t>((tagBuf[8] & ~0xFu) | 0xFu);     // lbu 0x28(sp)

        callRecompiled4(runtime, rdram, ctx, 0x127870u, ctxListAddr, tagBufAddr, 0u, 0u);

        // 0x1279E0(ctx) — write the DMA command word.
        callRecompiled4(runtime, rdram, ctx, 0x1279E0u, ctxListAddr, 0u, 0u, 0u);

        // 0x127B30 again
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, ctxListAddr, 0x1400u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, ctxListAddr, 0x1000u, 0u, 0u);

        // 0x127620(ctx) — GIF-tag header at ctx+0x10.
        callRecompiled4(runtime, rdram, ctx, 0x127620u, ctxListAddr, 0u, 0u, 0u);

        // 0x1588B0(this+0x78, 0x10, 0x10) — alloc 64-byte vector descriptor.
        callRecompiled4(runtime, rdram, ctx, 0x1588B0u, thisObj + 0x78u, 0x10u, 0x10u, 0u);

        // Display size fields.
        ws[0x4 / 4] = 0x280u; // li v1,0x280; sw v1,0x4(s0)
        ws[0x8 / 4] = 0x1C0u; // li v0,0x1C0; sw v0,0x8(s0)

        // res = 0x103E30(0x20, 0x0041F950, 0x251) — allocator dispatch (a0=0x20).
        const uint32_t res = callRecompiled4(runtime, rdram, ctx, 0x103E30u,
                                             0x20u, 0x0041F950u, 0x251u, 0u);

        // beqz res -> skip 0x187270(res, 0xC, 1); descriptor node on the buffer.
        if (res != 0u)
        {
            callRecompiled4(runtime, rdram, ctx, 0x187270u, res, 0xCu, 0x1u, 0u);
        }
        ws[0xD4 / 4] = res; // this+0xD4 = res

        // Tail: this+0x24 = 1.0f (delay slot of 0x1570D0 jal), then 0x1570D0(this).
        ws[0x24 / 4] = 0x3F800000u; // lui v0,0x3F80 (delay of jal 0x1570D0)
        callRecompiled4(runtime, rdram, ctx, 0x1570D0u, thisObj, 0u, 0u, 0u);

        // VIF1 DMA kick #1: 0x157510(this, 2, 0x003BF540, 0).
        callRecompiled4(runtime, rdram, ctx, 0x157510u, thisObj, 0x2u, 0x003BF540u, 0u);
        // 0x156760(1, 1) — VIF1 wait/tick.
        callRecompiled4(runtime, rdram, ctx, 0x156760u, 0x1u, 0x1u, 0u, 0u);
        // VIF1 DMA kick #2: 0x157510(this, 2, 0x003C0600, 0) then 0x156760(1, 1).
        callRecompiled4(runtime, rdram, ctx, 0x157510u, thisObj, 0x2u, 0x003C0600u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x156760u, 0x1u, 0x1u, 0u, 0u);

        SET_GPR_U32(ctx, 2, thisObj); // dmove v0, s0
    }

    // Boot-path vtable method @ 0x176d60, reached via JALR from 0x10455c:
    //   t9 = *(*(a0+8) + 0x30) — virtual dispatch on the display-system object that
    //   FUN_0010451c created (allocated via 0x103E30, stored at 0x4891C0). Called with
    //   (this=0x531c10, 0x3, 0x0, 0x3). Analyzer missed the IndirectCall boundary.
    // PCSX2-native MIPS decode (verified 2026-09-15):
    //
    //   0x16B900(this)                     — alloc + init sub-struct, this+0x30
    //   v = 0x103E30(4, 0x4207A0, 0x61C); if (v) 0x185000(v)   (zero-init)
    //       this+0x80 = 0x185010(v, 0x609) (alloc + descriptor node)
    //   v = 0x103E30(4, 0x4207A0, 0x61D); if (v) v = 0x187120(v); this+0x84 = v
    //   v = 0x103E30(0x20, 0x4207A0, 0x61F); if (v) v = 0x187270(v, 4, 1); this+0x88 = v
    //   if (this+0x88): 0x1873A0 / 0x1873C0 / 0x187430(x,1) / 0x1874C0(x,0x3F,0) / 0x1875E0
    //   0x359E80(5, 0x174AE0, 0); 0x35ACE8(5); 0x359EB0(1, 0x174D40, 0)
    //   VIF1_TOPS (0x10003C30) = 0
    //   param lists: +0x90 {0x127560(1,0x80), 0x9300, 0x127620}
    //                +0xC0 {0x127560(1,0x80), 0x07000001, 0x9300, 0x127620}
    //                +0x50 {0x127560(1,0x80), 0x1100, 0x0300, 0x0200, 0x127960(0x320,0xC),
    //                       tag(+0x127870,+0x1279E0), 0x1400, 0x1000, 0x1400, 0x127620}
    //                +0xF0 {0x127560(1,0x80), 0x1300, 0x127620}
    //   cnt = (int16)this+0x14C; this+0x144 = 0x103C20(cnt<<5, 0x4207A0, 0x658)
    //   0x36CB20(this+0x144, 0, cnt<<5)   — zero fill
    //   return this in v0
    void vtableMethod176d60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t thisObj = getRegU32(ctx, 4);
        uint8_t *thisBase = getMemPtr(rdram, thisObj);
        if (thisBase == nullptr)
        {
            std::cerr << "[SCUS_972.11] vtableMethod176d60: cannot resolve guest this 0x"
                      << std::hex << thisObj << std::dec << std::endl;
            return;
        }
        auto *ws = reinterpret_cast<uint32_t *>(thisBase);

        // jal 0x16B900(this): allocate 0x28 blob (0x103E30(0x28, 0x425720, 0x25)), init via
        // 0x1D0780, store this+0x30, then 0x106230(this).
        callRecompiled4(runtime, rdram, ctx, 0x16B900u, thisObj, 0u, 0u, 0u);

        // First descriptor: 0x103E30(4, 0x4207A0, 0x61C); if (v) 0x185000(v);
        // then this+0x80 = 0x185010(v, 0x609).
        uint32_t v = callRecompiled4(runtime, rdram, ctx, 0x103E30u, 0x4u, 0x004207A0u, 0x61Cu, 0u);
        if (v != 0u)
        {
            v = callRecompiled4(runtime, rdram, ctx, 0x185000u, v, 0u, 0u, 0u);
        }
        ws[0x80 / 4] = callRecompiled4(runtime, rdram, ctx, 0x185010u, v, 0x609u, 0u, 0u);

        // Second descriptor: 0x103E30(4, 0x4207A0, 0x61D); if (v) v = 0x187120(v);
        // this+0x84 = v.
        v = callRecompiled4(runtime, rdram, ctx, 0x103E30u, 0x4u, 0x004207A0u, 0x61Du, 0u);
        if (v != 0u)
        {
            v = callRecompiled4(runtime, rdram, ctx, 0x187120u, v, 0u, 0u, 0u);
        }
        ws[0x84 / 4] = v;

        // Frame buffer descriptor: 0x103E30(0x20, 0x4207A0, 0x61F); if (v) v = 0x187270(v, 4, 1);
        // this+0x88 = v.
        v = callRecompiled4(runtime, rdram, ctx, 0x103E30u, 0x20u, 0x004207A0u, 0x61Fu, 0u);
        if (v != 0u)
        {
            v = callRecompiled4(runtime, rdram, ctx, 0x187270u, v, 0x4u, 0x1u, 0u);
        }
        ws[0x88 / 4] = v;

        // Descriptor ops on this+0x88 (all unconditional in MIPS).
        const uint32_t r88 = ws[0x88 / 4];
        if (r88 != 0u)
        {
            callRecompiled4(runtime, rdram, ctx, 0x1873A0u, r88, 0u, 0u, 0u);
            callRecompiled4(runtime, rdram, ctx, 0x1873C0u, r88, 0u, 0u, 0u);
            callRecompiled4(runtime, rdram, ctx, 0x187430u, r88, 0x1u, 0u, 0u);
            callRecompiled4(runtime, rdram, ctx, 0x1874C0u, r88, 0x3Fu, 0u, 0u);
            callRecompiled4(runtime, rdram, ctx, 0x1875E0u, r88, 0u, 0u, 0u);
        }

        // Thread/intc setup.
        callRecompiled4(runtime, rdram, ctx, 0x359E80u, 0x5u, 0x00174AE0u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x35ACE8u, 0x5u, 0u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x359EB0u, 0x1u, 0x00174D40u, 0u, 0u);

        // VIF1_TOPS = 0 (lui at,0x1000; sw zero,0x3C30(at)) — via the memory model so
        // MMIO routing applies; ignore if the address is unmapped.
        try
        {
            runtime->memory().write32(0x10003C30u, 0u);
        }
        catch (...)
        {
        }

        // this+0x90 list: {cmd=1, init=0x80}, 0x9300, gif-tag header.
        const uint32_t list90 = thisObj + 0x90u;
        callRecompiled4(runtime, rdram, ctx, 0x127560u, list90, 0x1u, 0x80u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list90, 0x9300u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127620u, list90, 0u, 0u, 0u);

        // this+0xC0 list: {cmd=1, init=0x80}, 0x07000001, 0x9300, gif-tag header.
        const uint32_t listC0 = thisObj + 0xC0u;
        callRecompiled4(runtime, rdram, ctx, 0x127560u, listC0, 0x1u, 0x80u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, listC0, 0x07000001u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, listC0, 0x9300u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127620u, listC0, 0u, 0u, 0u);

        // this+0x50 list: cmd start, values, 0x127960(0x320, 0xC), GIF-tag, three paddings.
        const uint32_t list50 = thisObj + 0x50u;
        callRecompiled4(runtime, rdram, ctx, 0x127560u, list50, 0x1u, 0x80u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x1100u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x0300u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x0200u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127960u, list50, 0x320u, 0xCu, 0u);

        // Stack GIF-tag (same fiddle as 0x156e80) appended via 0x127870(list50, buf).
        const uint32_t sp = getRegU32(ctx, 29);
        const uint32_t tagBufAddr = (sp - 0x20u) & ~0xFu;
        uint8_t *tagBuf = getMemPtr(rdram, tagBufAddr);
        if (tagBuf == nullptr)
        {
            std::cerr << "[SCUS_972.11] vtableMethod176d60: cannot resolve guest tag buf 0x"
                      << std::hex << tagBufAddr << std::dec << std::endl;
            return;
        }
        std::memset(tagBuf, 0, 0x10); // 0x36CB20(buf, 0, 0x10)

        uint16_t t0 = static_cast<uint16_t>(tagBuf[0] | (tagBuf[1] << 8));
        uint16_t t5 = static_cast<uint16_t>((t0 & 0x8000u) | 0x1u);
        tagBuf[0] = static_cast<uint8_t>(t5 & 0xFFu);
        tagBuf[1] = static_cast<uint8_t>(t5 >> 8);
        tagBuf[5] = static_cast<uint8_t>((tagBuf[5] & ~0x40u) | 0x40u);
        tagBuf[1] = static_cast<uint8_t>((tagBuf[1] & ~0x80u) | 0x80u);
        tagBuf[7] = static_cast<uint8_t>(tagBuf[7] & ~0xCu);
        tagBuf[7] = static_cast<uint8_t>((tagBuf[7] & ~0xF0u) | 0x10u);
        tagBuf[8] = static_cast<uint8_t>((tagBuf[8] & ~0xFu) | 0xFu);

        callRecompiled4(runtime, rdram, ctx, 0x127870u, list50, tagBufAddr, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x1279E0u, list50, 0u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x1400u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x1000u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, list50, 0x1400u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127620u, list50, 0u, 0u, 0u);

        // this+0xF0 list: {cmd=1, init=0x80}, 0x1300, gif-tag header.
        const uint32_t listF0 = thisObj + 0xF0u;
        callRecompiled4(runtime, rdram, ctx, 0x127560u, listF0, 0x1u, 0x80u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127B30u, listF0, 0x1300u, 0u, 0u);
        callRecompiled4(runtime, rdram, ctx, 0x127620u, listF0, 0u, 0u, 0u);

        // cnt = (int16)this+0x14C; this+0x144 = 0x103C20(cnt<<5, 0x4207A0, 0x658);
        // 0x36CB20(this+0x144, 0, cnt<<5).
        const int32_t cnt = static_cast<int16_t>(ws[0x14C / 4] & 0xFFFFu);
        const uint32_t size144 = static_cast<uint32_t>(cnt) << 5;
        ws[0x144 / 4] = callRecompiled4(runtime, rdram, ctx, 0x103C20u,
                                        size144, 0x004207A0u, 0x658u, 0u);
        uint8_t *buf144 = getMemPtr(rdram, ws[0x144 / 4]);
        if (buf144 != nullptr && size144 != 0u)
        {
            std::memset(buf144, 0, size144);
        }

        SET_GPR_U32(ctx, 2, thisObj); // dmove v0, s0
    }

    // Sound driver RPC dispatcher @ 0x002f0150 (sub_002F0150, PS2SOUND.IRX).
    // Called with $a0=context, $a1=cmd, $a2=flags, $a3=sendbuf, $t0=sendsize, $t1=recvbuf, $t2=recvsize.
    // Cmd 1 = query driver version -> caller at 0x2f1288 checks for 0x0221 (version 2.21).
    void soundRpcCall2f0150(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t ctxAddr = getRegU32(ctx, 4);  // $a0
        const uint32_t cmd = getRegU32(ctx, 5);      // $a1
        const uint32_t respAddr = ctxAddr + 0x300u;  // receive buffer offset
        uint32_t *resp = reinterpret_cast<uint32_t *>(getMemPtr(rdram, respAddr));

        uint32_t retVal = 0u;
        if (cmd == 1u)
        {
            retVal = 0x0221u; // Driver v2.21 expected by 0x2f1288
        }

        if (resp != nullptr)
        {
            *resp = retVal;
        }

        SET_GPR_U32(ctx, 2, retVal); // v0 = return value
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // Display/timer IOP RPC @ 0x001a8cc0 (sub_001A8CC0, sid=0x80000211).
    // Returns 0 on success.
    void displayRpcCall1a8cc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 0u); // v0 = 0 (success)
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // sceMtapGetConnection @ 0x00378928 (sub_00378928).
    // $a0 = port (0 or 1). Returns 1 for direct pad attached without multitap.
    void mtapGetConnection378928(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 1u); // v0 = 1 (1 controller slot)
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // sceMtapChangeThreadPriority @ 0x00378848 (sub_00378848).
    // Returns 0 on success.
    void mtapChangeThreadPriority378848(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 0u); // v0 = 0 (success)
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // sceMtapInit @ 0x003785c8 (sub_003785C8).
    // Returns 0 on success.
    void mtapInit3785c8(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 0u); // v0 = 0 (success)
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // CDVD streaming status / check @ 0x00376408 (sub_00376408, calls SIF RPC sid=0x8000059A).
    // In libcdvd, this checks streaming/disk ready status.
    // Returns 0 on success. Returning 6 is an error code that aborts streaming CD reads in sub_0033F5C0.
    void cdvdStreamStatus376408(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 0u); // v0 = 0 (success / ready)
        ctx->pc = getRegU32(ctx, 31); // return via ra
    }

    // newlib _strtod_r @ 0x00370430.
    // Signature: double _strtod_r(struct _reent *reent, const char *text, char **endptr).
    // The analyzer identified this 3860-byte libc routine, but it was excluded as a
    // heavy-loop function and therefore has no generated function-table entry.
    void strtodR370430(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;

        const uint32_t reentAddr = getRegU32(ctx, 4);
        const uint32_t textAddr = getRegU32(ctx, 5);
        const uint32_t endPtrAddr = getRegU32(ctx, 6);
        constexpr size_t kMaxInputBytes = 4096u;

        std::string text;
        text.reserve(64u);
        bool terminated = false;
        if (textAddr != 0u)
        {
            for (size_t i = 0; i < kMaxInputBytes; ++i)
            {
                const uint64_t guestAddr = static_cast<uint64_t>(textAddr) + i;
                if (guestAddr > 0xFFFFFFFFull)
                {
                    break;
                }

                const uint8_t *guestChar = getMemPtr(rdram, static_cast<uint32_t>(guestAddr));
                if (guestChar == nullptr)
                {
                    break;
                }
                if (*guestChar == 0u)
                {
                    terminated = true;
                    break;
                }
                text.push_back(static_cast<char>(*guestChar));
            }
        }

        uint32_t guestEndAddr = textAddr;
        double value = 0.0;
        int parseErrno = 0;
        if (terminated)
        {
            errno = 0;
            char *hostEnd = nullptr;
            value = std::strtod(text.c_str(), &hostEnd);
            parseErrno = errno;
            const size_t consumed = (hostEnd != nullptr)
                                        ? static_cast<size_t>(hostEnd - text.c_str())
                                        : 0u;
            guestEndAddr += static_cast<uint32_t>(consumed);
        }
        else
        {
            std::cerr << "[SCUS_972.11] _strtod_r: invalid or unterminated guest string at 0x"
                      << std::hex << textAddr << std::dec << std::endl;
        }

        if (endPtrAddr != 0u)
        {
            uint8_t *guestEndPtr = getMemPtr(rdram, endPtrAddr);
            if (guestEndPtr != nullptr)
            {
                std::memcpy(guestEndPtr, &guestEndAddr, sizeof(guestEndAddr));
            }
            else
            {
                std::cerr << "[SCUS_972.11] _strtod_r: cannot resolve endptr at 0x"
                          << std::hex << endPtrAddr << std::dec << std::endl;
            }
        }

        // newlib's struct _reent begins with its errno field. Mirror host range errors.
        if (parseErrno != 0 && reentAddr != 0u)
        {
            uint8_t *guestErrno = getMemPtr(rdram, reentAddr);
            if (guestErrno != nullptr)
            {
                const int32_t guestErrnoValue = static_cast<int32_t>(parseErrno);
                std::memcpy(guestErrno, &guestErrnoValue, sizeof(guestErrnoValue));
            }
        }

        uint64_t resultBits = 0u;
        static_assert(sizeof(resultBits) == sizeof(value));
        std::memcpy(&resultBits, &value, sizeof(resultBits));
        setReturnU64(ctx, resultBits);

        static uint32_t logCount = 0u;
        if (logCount < 32u)
        {
            std::cerr << "[SCUS_972.11] _strtod_r: text='" << text
                      << "' consumed=" << (guestEndAddr - textAddr)
                      << " result=" << value << std::endl;
            ++logCount;
        }

        // 0x371364 tail-jumps here, so there is no DirectCall fallthrough to advance PC.
        ctx->pc = getRegU32(ctx, 31);
    }

    // Expanding chunk-list element allocator @ 0x001ABC50:
    // Merged into generated sub_001ABAE0 without re-entry switch support.
    // If $a1 != 0, returns $a1 in $v0.
    // If $a1 == 0, returns obj->0x40 if non-zero; else if obj->0x38 == obj->0x30,
    // expands chunk table via 0x103CF0 (realloc) and allocates new chunk via 0x103C20 (alloc);
    // returns chunk[offset] in $v0 and advances ctx->pc = $ra.
    void chunkListAlloc1abc50(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t a0 = getRegU32(ctx, 4);
        const uint32_t a1 = getRegU32(ctx, 5);

        if (a1 != 0u)
        {
            SET_GPR_U32(ctx, 2, a1);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        uint8_t *obj = getMemPtr(rdram, a0);
        if (obj == nullptr)
        {
            SET_GPR_U32(ctx, 2, 0u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        const uint32_t f40 = *reinterpret_cast<const uint32_t *>(obj + 0x40);
        if (f40 != 0u)
        {
            SET_GPR_U32(ctx, 2, f40);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        const uint32_t f38 = *reinterpret_cast<const uint32_t *>(obj + 0x38);
        const uint32_t f30 = *reinterpret_cast<const uint32_t *>(obj + 0x30);

        if (f38 == f30)
        {
            const uint32_t f34 = *reinterpret_cast<const uint32_t *>(obj + 0x34);
            const uint32_t f3c = *reinterpret_cast<const uint32_t *>(obj + 0x3C);

            // 0x103CF0: realloc chunk pointer table
            const uint32_t newChunkTable = callRecompiled4(
                runtime, rdram, ctx, 0x00103CF0u,
                f3c, (f34 + 1u) << 2, 0x004223B8u, 0x56u);
            *reinterpret_cast<uint32_t *>(obj + 0x3C) = newChunkTable;

            // 0x103C20: alloc new chunk
            const uint32_t newChunk = callRecompiled4(
                runtime, rdram, ctx, 0x00103C20u,
                f30 << 2, 0x004223B8u, 0x59u, 0u);

            uint8_t *tblPtr = getMemPtr(rdram, newChunkTable + (f34 << 2));
            if (tblPtr != nullptr)
            {
                *reinterpret_cast<uint32_t *>(tblPtr) = newChunk;
            }

            *reinterpret_cast<uint32_t *>(obj + 0x34) = f34 + 1u;
            *reinterpret_cast<uint32_t *>(obj + 0x38) = 0u;
        }

        const uint32_t f34_now = *reinterpret_cast<const uint32_t *>(obj + 0x34);
        const uint32_t f3c_now = *reinterpret_cast<const uint32_t *>(obj + 0x3C);
        const uint32_t f38_now = *reinterpret_cast<const uint32_t *>(obj + 0x38);

        uint8_t *tblPtr = getMemPtr(rdram, f3c_now + (f34_now << 2) - 4u);
        const uint32_t chunkBase = (tblPtr != nullptr) ? *reinterpret_cast<const uint32_t *>(tblPtr) : 0u;
        const uint32_t retVal = chunkBase + (f38_now << 2);

        SET_GPR_U32(ctx, 2, retVal);
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001ABC40: tail calls 0x1abc50 with a1 = a3
    void forward1abc40(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        SET_GPR_U64(ctx, 5, GPR_U64(ctx, 7));
        chunkListAlloc1abc50(rdram, ctx, runtime);
    }

    // 0x001ABC20: tail calls 0x1abc50 with a1 = a2
    void forward1abc20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        SET_GPR_U64(ctx, 5, GPR_U64(ctx, 6));
        chunkListAlloc1abc50(rdram, ctx, runtime);
    }

    // 0x001ABC30: tail calls 0x1abc50 with current a1
    void forward1abc30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        chunkListAlloc1abc50(rdram, ctx, runtime);
    }

    // 0x001ABD20: inserts element into chunk list and updates ref/count
    void chunkListInsert1abd20(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0 = getRegU32(ctx, 4);
        const uint32_t a1 = getRegU32(ctx, 5);
        const uint32_t a2 = getRegU32(ctx, 6);

        uint8_t *obj = getMemPtr(rdram, a0);
        if (obj != nullptr)
        {
            uint32_t v1 = *reinterpret_cast<const uint32_t *>(obj + 0x40);
            if (v1 != 0u && a2 == v1)
            {
                uint8_t *v1Ptr = getMemPtr(rdram, v1);
                if (v1Ptr != nullptr)
                {
                    const uint32_t a3 = *reinterpret_cast<const uint32_t *>(v1Ptr);
                    *reinterpret_cast<uint32_t *>(obj + 0x40) = a3 & ~3u;
                }
            }
            else
            {
                const uint32_t cnt = *reinterpret_cast<const uint32_t *>(obj + 0x38);
                *reinterpret_cast<uint32_t *>(obj + 0x38) = cnt + 1u;
            }
        }

        uint8_t *dest = getMemPtr(rdram, a2);
        if (dest != nullptr)
        {
            *reinterpret_cast<uint32_t *>(dest) = a1;
        }

        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001ABD60: leaf vtable method (obj+0x40 tracking)
    void chunkListMethod1abd60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0 = getRegU32(ctx, 4);
        const uint32_t a2 = getRegU32(ctx, 6);
        uint8_t *obj = getMemPtr(rdram, a0);
        uint8_t *dest = getMemPtr(rdram, a2);
        if (obj != nullptr && dest != nullptr)
        {
            const uint32_t v1 = *reinterpret_cast<const uint32_t *>(obj + 0x40);
            *reinterpret_cast<uint32_t *>(dest) = v1 | 2u;
            *reinterpret_cast<uint32_t *>(obj + 0x40) = a2;
        }
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001ABD80: leaf vtable method (flags reset)
    void chunkListMethod1abd80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0 = getRegU32(ctx, 4);
        uint8_t *obj = getMemPtr(rdram, a0);
        if (obj != nullptr)
        {
            *reinterpret_cast<uint32_t *>(obj + 0x44) = 0u;
            *reinterpret_cast<uint32_t *>(obj + 0x48) = 0u;
            const uint8_t b16 = obj[0x16];
            obj[0x16] = static_cast<uint8_t>((b16 & 0xFEu) | 1u);
        }
        SET_GPR_U32(ctx, 2, 1u);
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001ABF30: leaf vtable method (flags reset)
    void chunkListMethod1abf30(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0 = getRegU32(ctx, 4);
        uint8_t *obj = getMemPtr(rdram, a0);
        if (obj != nullptr)
        {
            *reinterpret_cast<uint32_t *>(obj + 0x44) = 0u;
            *reinterpret_cast<uint32_t *>(obj + 0x48) = 0u;
            const uint8_t b16 = obj[0x16];
            obj[0x16] = static_cast<uint8_t>((b16 & 0xFEu) | 1u);
        }
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001ABF60: leaf vtable method (flags reset)
    void chunkListMethod1abf60(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0 = getRegU32(ctx, 4);
        uint8_t *obj = getMemPtr(rdram, a0);
        if (obj != nullptr)
        {
            *reinterpret_cast<uint32_t *>(obj + 0x44) = 0u;
            *reinterpret_cast<uint32_t *>(obj + 0x48) = 0u;
            const uint8_t b16 = obj[0x16];
            obj[0x16] = static_cast<uint8_t>(b16 & 0xFEu);
        }
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x00101E80: leaf refcount getter (returns 64-bit count from obj+0x00)
    void getRef101e80(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        uint64_t val = 0;
        uint8_t *p = getMemPtr(rdram, getRegU32(ctx, 4));
        if (p != nullptr)
        {
            std::memcpy(&val, p, sizeof(val));
        }
        SET_GPR_U64(ctx, 2, val);
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x001A23F0: no-op leaf method
    void noop1a23f0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        ctx->pc = getRegU32(ctx, 31);
    }

    // Resource node copier @ 0x001BAA90:
    // Merged into generated sub_001BA970 without a table slot.
    // Deep-copies payload from src+0x38 into a newly allocated node at obj+0x2C via 0x103C80.
    void resourceNodeCopy1baa90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t objAddr = getRegU32(ctx, 4); // a0
        const uint32_t srcAddr = getRegU32(ctx, 5); // a1

        uint8_t *obj = getMemPtr(rdram, objAddr);
        uint8_t *src = getMemPtr(rdram, srcAddr);
        if (obj == nullptr || src == nullptr)
        {
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        const uint32_t existingNode = *reinterpret_cast<const uint32_t *>(obj + 0x2C);
        if (existingNode != 0u)
        {
            callRecompiled4(runtime, rdram, ctx, 0x00103DF0u, existingNode, 0x00424970u, 0x292u, 0u);
        }

        const uint32_t srcSubAddr = *reinterpret_cast<const uint32_t *>(src + 0x38);
        uint8_t *srcSub = getMemPtr(rdram, srcSubAddr);
        if (srcSub != nullptr)
        {
            const uint16_t count = *reinterpret_cast<const uint16_t *>(srcSub);
            const uint32_t payloadSize = static_cast<uint32_t>(count) << 4;

            const uint32_t newNode = callRecompiled4(
                runtime, rdram, ctx, 0x00103C80u,
                0x40u, payloadSize + 0x10u, 0x00424970u, 0x295u);

            *reinterpret_cast<uint32_t *>(obj + 0x2C) = newNode;

            uint8_t *newNodePtr = getMemPtr(rdram, newNode);
            if (newNodePtr != nullptr)
            {
                std::memcpy(newNodePtr + 0x10u, srcSub + 0x10u, payloadSize);

                *reinterpret_cast<uint16_t *>(newNodePtr + 0) = *reinterpret_cast<const uint16_t *>(srcSub + 0);
                newNodePtr[2] = srcSub[2];
                newNodePtr[3] = srcSub[3];
                *reinterpret_cast<uint32_t *>(newNodePtr + 4) = *reinterpret_cast<const uint32_t *>(srcSub + 4);
                *reinterpret_cast<uint32_t *>(newNodePtr + 8) = *reinterpret_cast<const uint32_t *>(srcSub + 8);
                *reinterpret_cast<uint32_t *>(newNodePtr + 12) = *reinterpret_cast<const uint32_t *>(srcSub + 12);

                *reinterpret_cast<uint32_t *>(newNodePtr + 4) = newNode + 0x10u;
            }
        }

        ctx->pc = getRegU32(ctx, 31);
    }

    // Equality comparison @ 0x001B7E10:
    // Merged into generated sub_001B7C20 without a function table slot.
    // Compares two resource/sound descriptors.
    // Returns 1 if equal, 0 if not equal.
    void resourceDescEquals1b7e10(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)runtime;
        const uint32_t a0Addr = getRegU32(ctx, 4);
        const uint32_t a1Addr = getRegU32(ctx, 5);

        if (a0Addr == a1Addr)
        {
            SET_GPR_U32(ctx, 2, 1u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        uint8_t *a0 = getMemPtr(rdram, a0Addr);
        uint8_t *a1 = getMemPtr(rdram, a1Addr);
        if (a0 == nullptr || a1 == nullptr)
        {
            SET_GPR_U32(ctx, 2, 0u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        // Compare scalar fields
        if (*reinterpret_cast<const int16_t *>(a0 + 0x28) != *reinterpret_cast<const int16_t *>(a1 + 0x28) ||
            a0[0x12] != a1[0x12] ||
            *reinterpret_cast<const int16_t *>(a0 + 0x14) != *reinterpret_cast<const int16_t *>(a1 + 0x14) ||
            *reinterpret_cast<const int16_t *>(a0 + 0x16) != *reinterpret_cast<const int16_t *>(a1 + 0x16) ||
            a0[0x18] != a1[0x18] ||
            a0[0x19] != a1[0x19])
        {
            SET_GPR_U32(ctx, 2, 0u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        // Bitfields at 0x2A (bits 1 and 2)
        const uint8_t b0 = a0[0x2A];
        const uint8_t b1 = a1[0x2A];
        if (((b0 >> 1) & 3u) != ((b1 >> 1) & 3u))
        {
            SET_GPR_U32(ctx, 2, 0u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        // Sub-payload at +0x38
        const uint32_t sub0Addr = *reinterpret_cast<const uint32_t *>(a0 + 0x38);
        const uint32_t sub1Addr = *reinterpret_cast<const uint32_t *>(a1 + 0x38);
        if (sub0Addr != sub1Addr)
        {
            uint8_t *sub0 = getMemPtr(rdram, sub0Addr);
            uint8_t *sub1 = getMemPtr(rdram, sub1Addr);
            if (sub0 == nullptr || sub1 == nullptr)
            {
                SET_GPR_U32(ctx, 2, 0u);
                ctx->pc = getRegU32(ctx, 31);
                return;
            }
            const uint16_t cnt0 = *reinterpret_cast<const uint16_t *>(sub0);
            const uint16_t cnt1 = *reinterpret_cast<const uint16_t *>(sub1);
            if (cnt0 != cnt1)
            {
                SET_GPR_U32(ctx, 2, 0u);
                ctx->pc = getRegU32(ctx, 31);
                return;
            }
            const size_t sz = static_cast<size_t>(cnt0) << 4;
            if (std::memcmp(sub0 + 0x10, sub1 + 0x10, sz) != 0)
            {
                SET_GPR_U32(ctx, 2, 0u);
                ctx->pc = getRegU32(ctx, 31);
                return;
            }
        }

        // Compare field +0x10
        const int16_t h0 = *reinterpret_cast<const int16_t *>(a0 + 0x10);
        const int16_t h1 = *reinterpret_cast<const int16_t *>(a1 + 0x10);
        if (h0 == -2 || h0 != h1)
        {
            SET_GPR_U32(ctx, 2, 0u);
            ctx->pc = getRegU32(ctx, 31);
            return;
        }

        SET_GPR_U32(ctx, 2, 1u);
        ctx->pc = getRegU32(ctx, 31);
    }

    // 0x00179320: leaf vtable method `jr ra; li v0, 1`
    void leafRet1_179320(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        (void)rdram;
        (void)runtime;
        SET_GPR_U32(ctx, 2, 1u);
        ctx->pc = getRegU32(ctx, 31);
    }

    // Resource configuration parser (0x0014E210..0x0014E240 prologue).
    // Rejoin the generated body sub_0014DFD0 at 0x0014E244.
    void resourceConfig14e210(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        const uint32_t sp = getRegU32(ctx, 29) - 0xC0u;
        SET_GPR_S32(ctx, 29, static_cast<int32_t>(sp));

        const uint64_t ra = GPR_U64(ctx, 31);
        std::memcpy(getMemPtr(rdram, sp + 0x30u), &ra, sizeof(ra));

        const auto s2 = GPR_VEC(ctx, 18);
        std::memcpy(getMemPtr(rdram, sp + 0x20u), &s2, 16);

        const auto s1 = GPR_VEC(ctx, 17);
        std::memcpy(getMemPtr(rdram, sp + 0x10u), &s1, 16);

        const auto s0 = GPR_VEC(ctx, 16);
        std::memcpy(getMemPtr(rdram, sp), &s0, 16);

        const uint64_t a0Val = GPR_U64(ctx, 4);
        const uint64_t a1Val = GPR_U64(ctx, 5);
        const uint64_t a2Val = GPR_U64(ctx, 6);

        SET_GPR_U64(ctx, 18, a0Val); // s2 = a0
        SET_GPR_U64(ctx, 17, a1Val); // s1 = a1
        SET_GPR_U64(ctx, 16, a2Val); // s0 = a2

        uint8_t *a0Ptr = getMemPtr(rdram, static_cast<uint32_t>(a0Val));
        if (a0Ptr != nullptr)
        {
            *reinterpret_cast<uint32_t *>(a0Ptr + 0x114u) = static_cast<uint32_t>(a2Val);
        }

        SET_GPR_U64(ctx, 4, a1Val);          // a0 = s1 (input filename)
        SET_GPR_U32(ctx, 5, 0x0041F078u);     // a1 = 0x41F078
        SET_GPR_U32(ctx, 31, 0x0014E244u);    // ra = 0x14E244
        ctx->pc = 0x0036F8B8u;

        if (!runtime->dispatchGuestBranch(rdram, ctx, 0x0036F8B8u,
                0x0014E23Cu, 0x0014E244u, PS2Runtime::GuestBranchKind::DirectCall, "JAL"))
            return;

        ctx->pc = 0x0014E244u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // VIF1 sync routine @ 0x00157BC0:
    //   li a0, 1
    //   j  0x00156760
    //   li a1, 3
    void vif1Sync157bc0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        SET_GPR_U32(ctx, 4, 1u);
        SET_GPR_U32(ctx, 5, 3u);
        ctx->pc = 0x00156760u;
        runtime->lookupFunction(ctx->pc)(rdram, ctx, runtime);
    }

    // Bypass movie playback in func_25B7C0 (0x25B7C0).
    // The frontend state machine uses this to play "Rainbow.pss" (state 10 -> next 9)
    // and "Intro.pss" (state 11 -> next 14).
    void movieStart25b7c0(uint8_t *rdram, R5900Context *ctx, PS2Runtime *)
    {
        const uint32_t movieObj = getRegU32(ctx, 4);
        const uint32_t parentObj = getRegU32(ctx, 5);
        const uint32_t moviePathAddr = getRegU32(ctx, 6);
        const uint32_t nextState = getRegU32(ctx, 7);
        const char *moviePath = moviePathAddr ? reinterpret_cast<const char*>(getMemPtr(rdram, moviePathAddr)) : "null";

        std::cerr << "[MOVIE:BYPASS] movieObj=0x" << std::hex << movieObj
                  << " parentObj=0x" << parentObj
                  << " path=" << (moviePath ? moviePath : "null")
                  << " nextState=" << std::dec << nextState << std::endl;

        if (parentObj != 0u)
        {
            *reinterpret_cast<uint32_t*>(getMemPtr(rdram, parentObj + 0xC8u)) = nextState;
        }
        if (movieObj != 0u)
        {
            *reinterpret_cast<uint32_t*>(getMemPtr(rdram, movieObj + 0x40u)) = parentObj;
            *reinterpret_cast<uint32_t*>(getMemPtr(rdram, movieObj + 0x150u)) = nextState;
        }

        SET_GPR_S32(ctx, 2, 0);
        ctx->pc = GPR_U32(ctx, 31);
    }

    // PSXMediaControl::IsFinished (0x18AC80)
    void mediaIsFinished18ac80(uint8_t *, R5900Context *ctx, PS2Runtime *)
    {
        SET_GPR_S32(ctx, 2, 0);
        ctx->pc = GPR_U32(ctx, 31);
    }

    // PSXMediaControl::Stop (0x18AC20)
    void mediaStop18ac20(uint8_t *, R5900Context *ctx, PS2Runtime *)
    {
        SET_GPR_S32(ctx, 2, 0);
        ctx->pc = GPR_U32(ctx, 31);
    }

    // PSXMediaControl::StartPlayback (0x18A510)
    void mediaStartPlayback18a510(uint8_t *, R5900Context *ctx, PS2Runtime *)
    {
        SET_GPR_S32(ctx, 2, 1);
        ctx->pc = GPR_U32(ctx, 31);
    }

    // Frame limiter hook @ 0x0015DA90
    // Runs on the main EE thread every frame.
    // Phase 1: Create viewport by calling func_1DFB00 (skipped because we bypass video).
    // Phase 2: Tick FrontendManager (Update + Draw) every frame.
    void frameLimiter15da90(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
    {
        static int s_frameCount = 0;
        static bool s_viewportCreated = false;
        static bool s_screenCreated = false;
        ++s_frameCount;

        if (s_frameCount > 60)
        {
            uint32_t gameObj = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, 0x00404818u));
            if (gameObj != 0u)
            {
                // --- Phase 1: Create viewport (one-shot) ---
                // On real hardware, State 2 calls func_1DFB00(gameObj) to create the
                // 3D viewport, open the CD archive, and load menu resources.
                // Since we bypass video, State 2 never runs, so we call it directly.
                if (!s_viewportCreated)
                {
                    uint32_t viewport = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, gameObj + 0x29Cu));
                    if (viewport == 0u)
                    {
                        std::cerr << "[ATV2:BOOT] Creating viewport via func_1DFB00(gameObj=0x"
                                  << std::hex << gameObj << ")..." << std::dec << std::endl;

                        R5900Context savedCtx = *ctx;
                        SET_GPR_U32(ctx, 4, gameObj);
                        SET_GPR_U32(ctx, 31, 0x0015DA90u);
                        ctx->pc = 0x001DFB00u;
                        sub_001DFB00_0x1dfb00(rdram, ctx, runtime);
                        *ctx = savedCtx;

                        viewport = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, gameObj + 0x29Cu));
                        std::cerr << "[ATV2:BOOT] After func_1DFB00: viewport=0x"
                                  << std::hex << viewport << std::dec << std::endl;
                    }
                    s_viewportCreated = true;
                }

                // --- Phase 1b: Create the main menu screen (one-shot, after viewport) ---
                // State 14 handler calls func_1E0810(gameObj, feMgr->screenId, feMgr->transId, 1)
                // to build the menu UI. We call it with the stored screen/trans IDs.
                if (s_viewportCreated && !s_screenCreated && s_frameCount > 90)
                {
                    uint32_t feMgr = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, gameObj + 0x2B0u));
                    if (feMgr != 0u)
                    {
                        uint32_t screenId = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0xC0u));
                        uint32_t transId  = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0xC4u));

                        std::cerr << "[ATV2:BOOT] Creating menu screen via func_1E0810(gameObj=0x"
                                  << std::hex << gameObj << ", screen=" << std::dec << screenId
                                  << ", trans=" << transId << ", flags=1)..." << std::endl;

                        R5900Context savedCtx = *ctx;
                        SET_GPR_U32(ctx, 4, gameObj);
                        SET_GPR_U32(ctx, 5, screenId);
                        SET_GPR_U32(ctx, 6, transId);
                        SET_GPR_U32(ctx, 7, 1u);
                        SET_GPR_U32(ctx, 31, 0x0015DA90u);
                        ctx->pc = 0x001E0810u;
                        sub_001E0810_0x1e0810(rdram, ctx, runtime);
                        *ctx = savedCtx;

                        // Also unlink feMgr from scene graph as State 14 does
                        {
                            R5900Context savedCtx2 = *ctx;
                            SET_GPR_U32(ctx, 4, feMgr);
                            SET_GPR_U32(ctx, 31, 0x0015DA90u);
                            ctx->pc = 0x00106050u;
                            sub_00106050_0x106050(rdram, ctx, runtime);
                            *ctx = savedCtx2;
                        }

                        s_screenCreated = true;
                        std::cerr << "[ATV2:BOOT] Menu screen creation done." << std::endl;
                    }
                }

                // --- Phase 2: Tick FrontendManager every frame ---
                uint32_t feMgr = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, gameObj + 0x2B0u));
                if (feMgr != 0u)
                {
                    uint32_t *pCur = reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0xCCu));
                    uint32_t *pPend = reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0xC8u));
                    uint32_t *pFlags20 = reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0x20u));
                    uint32_t *pFlags24 = reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0x24u));

                    // Force visibility flags on feMgr
                    *pFlags20 |= 0x8u;
                    *pFlags24 |= 0x8u;

                    // Ensure feScreen (feMgr + 0xB8) also has visibility flags
                    uint32_t feScreen = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, feMgr + 0xB8u));
                    if (feScreen != 0u)
                    {
                        *reinterpret_cast<uint32_t*>(getMemPtr(rdram, feScreen + 0x20u)) |= 0x8u;
                        *reinterpret_cast<uint32_t*>(getMemPtr(rdram, feScreen + 0x24u)) |= 0x8u;
                    }

                    // 1. Tick FrontendManager::Update(feMgr, dt=0.03333333f)
                    {
                        R5900Context savedCtx = *ctx;
                        SET_GPR_U32(ctx, 4, feMgr);
                        ctx->f[12] = 0.03333333f;
                        SET_GPR_U32(ctx, 31, 0x0015DA90u);
                        ctx->pc = 0x00256110u;
                        sub_002560A0_0x2560a0(rdram, ctx, runtime);
                        *ctx = savedCtx;
                    }

                    // 2. Draw feMgr via Node::Draw (0x00106370)
                    {
                        R5900Context savedCtx = *ctx;
                        SET_GPR_U32(ctx, 4, feMgr);
                        SET_GPR_U32(ctx, 31, 0x0015DA90u);
                        ctx->pc = 0x00106370u;
                        sub_00106370_0x106370(rdram, ctx, runtime);
                        *ctx = savedCtx;
                    }

                    // 3. Draw feScreen directly (via 0x0025C680)
                    if (feScreen != 0u)
                    {
                        R5900Context savedCtx = *ctx;
                        SET_GPR_U32(ctx, 4, feScreen);
                        SET_GPR_U32(ctx, 31, 0x0015DA90u);
                        ctx->pc = 0x0025C680u;
                        sub_0025C680_0x25c680(rdram, ctx, runtime);
                        *ctx = savedCtx;
                    }

                    static int s_feTicks = 0;
                    if (++s_feTicks <= 10 || s_feTicks % 60 == 0)
                    {
                        uint32_t viewport = *reinterpret_cast<uint32_t*>(getMemPtr(rdram, gameObj + 0x29Cu));
                        std::cerr << "[ATV2:FE_TICK] tick=" << s_feTicks
                                  << " feMgr=0x" << std::hex << feMgr
                                  << " cur=" << std::dec << *pCur
                                  << " pend=" << *pPend
                                  << " feScreen=0x" << std::hex << feScreen
                                  << " viewport=0x" << viewport
                                  << std::dec << std::endl;
                    }
                }
            }
        }

        ctx->pc = 0x0015DA90u;
        sub_0015DA90_0x15da90(rdram, ctx, runtime);
    }

    void applyScus97211Overrides(PS2Runtime &runtime)
    {
        runtime.replaceFunction(0x0014F610u, &resourceInit14f610);
        runtime.replaceFunction(0x001BD4F0u, &resourceHook1bd4f0);
        runtime.replaceFunction(0x00342590u, &vertexUpdate342590);
        runtime.replaceFunction(0x00107460u, &addRef107460);
        runtime.replaceFunction(0x001AE800u, &nameRegistry1ae800);
        runtime.replaceFunction(0x001D2FA0u, &registryFlags1d2fa0);
        runtime.replaceFunction(0x00177AE0u, &resourceForward177ae0);
        runtime.replaceFunction(0x001CE020u, &resourceRead1ce020);
        // PCSX2 confirms the adjacent resource type uses identical instructions.
        runtime.replaceFunction(0x001CE0C0u, &resourceRead1ce020);
        runtime.replaceFunction(0x002AE950u, &sliderInit2ae950);
        runtime.replaceFunction(0x002364C0u, &textInputInit2364c0);
        runtime.replaceFunction(0x00126FC0u, &leafStoreF12_126fc0);
        runtime.replaceFunction(0x002AE600u, &iconInit2ae600);
        runtime.replaceFunction(0x002AD6C0u, &highlightInit2ad6c0);
        runtime.replaceFunction(0x00204390u, &screenConfig204390);
        runtime.replaceFunction(0x002D1F10u, &ctor2d1f10);
        runtime.replaceFunction(0x001ABC50u, &chunkListAlloc1abc50);
        runtime.replaceFunction(0x001ABC40u, &forward1abc40);
        runtime.replaceFunction(0x001ABC20u, &forward1abc20);
        runtime.replaceFunction(0x001ABC30u, &forward1abc30);
        runtime.replaceFunction(0x001ABD20u, &chunkListInsert1abd20);
        runtime.replaceFunction(0x001ABD60u, &chunkListMethod1abd60);
        runtime.replaceFunction(0x001ABD80u, &chunkListMethod1abd80);
        runtime.replaceFunction(0x001ABDB0u, &chunkListMethod1abd80);
        runtime.replaceFunction(0x001ABF30u, &chunkListMethod1abf30);
        runtime.replaceFunction(0x001ABF60u, &chunkListMethod1abf60);
        runtime.replaceFunction(0x00101E80u, &getRef101e80);
        runtime.replaceFunction(0x001A23F0u, &noop1a23f0);
        runtime.replaceFunction(0x001BAA90u, &resourceNodeCopy1baa90);
        runtime.replaceFunction(0x001B7E10u, &resourceDescEquals1b7e10);
        runtime.replaceFunction(0x00179320u, &leafRet1_179320);
        runtime.replaceFunction(0x0014E210u, &resourceConfig14e210);
        runtime.replaceFunction(0x00157BC0u, &vif1Sync157bc0);
        std::cerr << "[SCUS_972.11] applying ATV Offroad Fury 2 override" << std::endl;
        runtime.setMissingFunctionPolicy(PS2Runtime::MissingFunctionPolicy::SkipCallDebug);
        g_ps2SkipSceGsSyncVWait = 1u;
        // ATV2 guest frame counter = 0x489008 (read by the frame-lockstep limiter
        // FUN_0015da90: `while (iRam00489008 < param_1)`). Its only in-game writer is the
        // GS-flip tail of sub_0015D4A0 (called from sub_001CD090), which never runs because
        // the game's VBlank INTC handler (FUN_00156590, registered on channel 9) is not
        // dispatched by the runtime as a frame tick. Host pump bumps this + the VBlank tick
        // 0x488f38 once per host frame (ps2_runtime.cpp bumpGuestCounter).
        g_ps2GuestVsyncCounterAddr = 0x00489008u;
        g_ps2GuestVsyncCounterAddr2 = 0x00488f38u;

        // pc=0 resume machinery (ATV1 pattern): when main thread goes Dormant at pc=0,
        // EeScheduler's usableResume() will resume it to the GS flip handler (0x164160)
        // if the last non-zero PC was in the frame-limiter range.
        g_ps2Pc0ResumeRangeBegin = 0x15DA90u;
        g_ps2Pc0ResumeRangeEnd = 0x15E500u;
        g_ps2Pc0ResumeTarget = 0x164160u;  // GS flip entry — writes 0x489008
        g_ps2IdleResumePc = 0x164160u;
        g_ps2IdleResumeA0 = 0u;

        // Static-init ctor table walker FUN_0034d9f0 JALR target missed by the analyzer's
        // jal heuristic. Slot is empty in the generated dense table; fill it natively.
        runtime.replaceFunction(0x0044c100u, &ctor44c100);

        // default_new_handler__3stdFv @ 0x34d4b0 — called via data-table pointer from __nw__FUi
        // (sub_0034D5A0 at 0x34d5d0 JALRs to it). Merged fn sub_0034D3A0 has no re-entry case for 0x34d4b0.
        // Dense table now spans to 0x4515f4 so replaceFunction succeeds.
        runtime.replaceFunction(0x0034d4b0u, &default_new_handler_34d4b0);

        // Session-lock enter @ 0x341e60 — vtable slot [8] target missed by the analyzer.
        runtime.replaceFunction(0x00341e60u, &sessionLockEnter341e60);

        // Session-lock leave @ 0x341f30 — vtable slot [c] twin, also missed.
        runtime.replaceFunction(0x00341f30u, &sessionLockLeave341f30);

        // Keep the generated lock wrapper constructor/destructor (@ 0x341f90 / 0x341fd0)
        // so guest call/return state and virtual dispatch are preserved.
        // runtime.replaceFunction(0x00341f90u, &lockWrapperEnter341f90);
        // runtime.replaceFunction(0x00341fd0u, &lockWrapperLeave341fd0);

        // 1-instruction vector ctor @ 0x11a3d0 (array-init via FUN_0034d0d0).
        runtime.replaceFunction(0x0011a3d0u, &ctor11a3d0);

        // 4×VF00-quad ctor @ 0x16b5e0 (array-init: writes {0,0,0,1} at +0x20..+0x5F).
        runtime.replaceFunction(0x0016b5e0u, &ctor16b5e0);

        // Element ctor @ 0x137a10 (queue/stream node init, stride 0x14).
        runtime.replaceFunction(0x00137a10u, &ctor137a10);

        // Element ctor @ 0x331690 (stride 0x18, called by __arrctor 0x34d0d0).
        runtime.replaceFunction(0x00331690u, &ctor331690);

        // No-op accessor @ 0x153970 (returns 3rd arg).
        runtime.replaceFunction(0x00153970u, &noopRetA2_153970);

        // Display-mode vtable method @ 0x155950 (mode init, missed IndirectCall JALR).
        runtime.replaceFunction(0x00155950u, &modeDisplayInit155950);

        // Node-lookup tail-call wrapper @ 0x187880.
        runtime.replaceFunction(0x00187880u, &nodeLookup187880);

        // Node-release tail-call wrapper @ 0x1878a0.
        runtime.replaceFunction(0x001878a0u, &nodeRelease1878a0);

        // VBlank-chain vtable method @ 0x156e80 (JALR from 0x1538ac in the VBlank
        // chain). Merged boundary; re-entry switch lacks case 0x156e80u.
        runtime.replaceFunction(0x00156e80u, &vtableMethod156e80);

        // Boot-path vtable method @ 0x176d60 (JALR from 0x10455c after the display
        // system allocates its object at 0x4891C0). Merged boundary; missed target.
        runtime.replaceFunction(0x00176d60u, &vtableMethod176d60);

        // Sound driver RPC dispatcher (PS2SOUND.IRX, sid=0x34567).
        runtime.replaceFunction(0x002f0150u, &soundRpcCall2f0150);

        // Display/engine IOP RPC (sid=0x80000211).
        runtime.replaceFunction(0x001a8cc0u, &displayRpcCall1a8cc0);

        // MTAPMAN (Multitap Manager) stubs: report single controller connected.
        runtime.replaceFunction(0x00378928u, &mtapGetConnection378928);
        runtime.replaceFunction(0x00378848u, &mtapChangeThreadPriority378848);
        runtime.replaceFunction(0x003785c8u, &mtapInit3785c8);

        // CDVD streaming status / check (sid=0x8000059A).
        runtime.replaceFunction(0x00376408u, &cdvdStreamStatus376408);

        // newlib _strtod_r. The TOML names this routine but its heavy-loop skip leaves
        // 0x370430 without a generated entry; the strtod wrapper tail-jumps here.
        runtime.replaceFunction(0x00370430u, &strtodR370430);

        // Movie & video playback bypass
        runtime.replaceFunction(0x0025B7C0u, &movieStart25b7c0);
        runtime.replaceFunction(0x0018AC80u, &mediaIsFinished18ac80);
        runtime.replaceFunction(0x0018AC20u, &mediaStop18ac20);
        runtime.replaceFunction(0x0018A510u, &mediaStartPlayback18a510);

        // Frame limiter: ticks FrontendManager update and draw every frame
        runtime.replaceFunction(0x0015DA90u, &frameLimiter15da90);
    }
}

PS2_REGISTER_GAME_OVERRIDE(
    "atv-offroad-fury-2-us",
    "SCUS_972.11",
    0x00100008u,
    0u,
    applyScus97211Overrides);
