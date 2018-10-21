/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2018 m0slevin, all rights reserved.
See license.txt for more information
===========================================================================*/
/**
    @file   semihosting.cpp
    @brief  ARM Semihosting support
 */
#include "semihosting.h"

//---------------------------------------------------------------------------
namespace
{
using namespace Mark3;

//---------------------------------------------------------------------------
enum class SemiHostingOpcodeType : uint32_t {
    Open            = 0x01,
    Close           = 0x02,
    Writec          = 0x03,
    Write0          = 0x04,
    Write           = 0x05,
    Read            = 0x06,
    Readc           = 0x07,
    IsError         = 0x08,
    IsTTY           = 0x09,
    Seek            = 0x0A,
    FLen            = 0x0C,
    Tmpnam          = 0x0D,
    Remove          = 0x0E,
    Clock           = 0x10,
    Time            = 0x11,
    System          = 0x12,
    Errno           = 0x13,
    GetCmdLine      = 0x15,
    HeapInfo        = 0x16,
    SWIEnterSvc     = 0x17,
    ReportException = 0x18
};

//---------------------------------------------------------------------------
enum class AdpExceptionType : uint32_t {
    BreakPoint          = 0x20020,
    WatchPoint          = 0x20021,
    StepComplete        = 0x20022,
    RunTimeErrorUnknown = 0x20023,
    InternalError       = 0x20024,
    UserInterruption    = 0x20025,
    ApplicationExit     = 0x20026,
    StackOverflow       = 0x20027,
    DivisionByZero      = 0x20028,
    OSSpecific          = 0x20029
};

//---------------------------------------------------------------------------
typedef union __attribute__((packed)) {
    struct __attribute__((packed)) {
        const char*   szName;
        FileModeFlags eFileMode;
    } Open;
    struct __attribute__((packed)) {
        int64_t s64Fd;
    } Close;
    struct __attribute__((packed)) {
        int64_t     s64Fd;
        const void* pvData;
        size_t      sCount;
    } Read;
    struct __attribute__((packed)) {
        int64_t     s64Fd;
        const void* pvData;
        size_t      sCount;
    } Write;
    struct __attribute__((packed)) {
        uint64_t u64Rc;
    } Exit;
    uint32_t data[3];
} SemiHostingArgument_t;

//---------------------------------------------------------------------------
extern "C" {
/**
 * Execute the semihosting operation described by the combination of opcode
 * and argument.  This is implemented using inline assembly, as the interface
 * requires deterministic manipulation of the r0, r1 registers, and the execution
 * of the bkpt instruction.
 *
 * @param eOpcode_ opcode to execute
 * @param pstArgs_ argument array corresponding to the argument (see the
 * SemiHostingArgument_t type for argument formats)
 * @return contents of r0 returned from the semihosting operation.
 */
void* Execute(SemiHostingOpcodeType eOpcode_, void* pstArgs_)
{
    void* pvRC;
    asm volatile(" mov x0, %[opcode] \n "
                 " mov x1, %[data] \n"
                 " hlt 0xf000 \n "
                 " str x0, [%[rc]] \n "
                 : [rc] "=r"(pvRC)
                 : [opcode] "r"(eOpcode_), [data] "r"(pstArgs_)
				 : "x0", "x1", "memory");
    return pvRC;
}
} // extern "C"
} // anonymous namespace

namespace Mark3
{
//---------------------------------------------------------------------------
int64_t SemiHosting::Open(const char* szName_, FileModeFlags eFlags_)
{
    SemiHostingArgument_t stArg = { 0 };
    stArg.Open.eFileMode        = eFlags_;
    stArg.Open.szName           = szName_;

    return reinterpret_cast<int64_t>(Execute(SemiHostingOpcodeType::Open, stArg.data));
}

//---------------------------------------------------------------------------
int64_t SemiHosting::Close(int64_t s64Fd_)
{
    SemiHostingArgument_t stArg = { 0 };
    stArg.Close.s64Fd           = s64Fd_;

    return reinterpret_cast<int64_t>(Execute(SemiHostingOpcodeType::Close, stArg.data));
}

//---------------------------------------------------------------------------
int64_t SemiHosting::Read(int64_t s64Fd_, const void* pvData_, size_t sCount_)
{
    SemiHostingArgument_t stArg = { 0 };
    stArg.Read.s64Fd            = s64Fd_;
    stArg.Read.pvData           = pvData_;
    stArg.Read.sCount           = sCount_;

    return reinterpret_cast<int64_t>(Execute(SemiHostingOpcodeType::Read, stArg.data));
}

//---------------------------------------------------------------------------
int64_t SemiHosting::Write(int64_t s64Fd_, const void* pvData_, size_t sCount_)
{
    SemiHostingArgument_t stArg = { 0 };
    stArg.Write.s64Fd           = s64Fd_;
    stArg.Write.pvData          = pvData_;
    stArg.Write.sCount          = sCount_;

    return reinterpret_cast<int64_t>(Execute(SemiHostingOpcodeType::Write, stArg.data));
}

//---------------------------------------------------------------------------
void SemiHosting::Exit(uint64_t u64Rc_)
{
    SemiHostingArgument_t stArg = { 0 };
    stArg.Exit.u64Rc            = u64Rc_;

    Execute(SemiHostingOpcodeType::ReportException, stArg.data);
}
} // namespace Mark3
