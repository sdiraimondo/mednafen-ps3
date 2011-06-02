/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
 * PSX assembly interpreter.
 */
#include <stdint.h>
#include "psxcommon.h"
#include "r3000a.h"
#include "gte.h"
#include "psxhle.h"

#define PASS_IT_ON	(*aResolve)(aCode, aResolve)

static int branch = 0;
static int branch2 = 0;
static u32 branchPC;

// Subsets
psxOpFunc psxBSC[64];
psxOpFunc psxSPC[64];
psxOpFunc psxREG[32];
psxOpFunc psxCP0[32];
psxOpFunc psxCP2[64];
psxOpFunc psxCP2BSC[32];

OPFUNC(psxADD_a);
OPFUNC(psxADDI_a);

// Execute one instruction
void TOTAL_INLINE execI()
{ 
	//ROBO: This will go crashy crashy if you try to execute garbage addresses !
	psxRegs.pc += 4;
	psxRegs.cycle += BIAS;

	psxRegs.code = GETLE32(PSXMfast(psxRegs.pc - 4));
	(*PSXMop(psxRegs.pc - 4))(psxRegs.code, PSXMop(psxRegs.pc - 4));

//	psxBSC[psxRegs.code >> 26](psxRegs.code);
}


static void delayRead(int reg, u32 bpc) {
	u32 rold, rnew;

//	SysPrintf("delayRead at %x!\n", psxRegs.pc);

	rold = psxRegs.GPR.r[reg];
//	psxBSC[psxRegs.code >> 26](psxRegs.code); // branch delay load
	rnew = psxRegs.GPR.r[reg];

	psxRegs.pc = bpc;

	psxBranchTest();

	psxRegs.GPR.r[reg] = rold;
	execI(); // first branch opcode
	psxRegs.GPR.r[reg] = rnew;

	branch = 0;
}

static void delayWrite(int reg, u32 bpc) {

/*	SysPrintf("delayWrite at %x!\n", psxRegs.pc);

	SysPrintf("%s\n", disR3000AF(psxRegs.code, psxRegs.pc-4));
	SysPrintf("%s\n", disR3000AF(PSXMu32(bpc), bpc));*/

	// no changes from normal behavior

//	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}

static void delayReadWrite(int reg, u32 bpc) {

//	SysPrintf("delayReadWrite at %x!\n", psxRegs.pc);

	// the branch delay load is skipped

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}

// this defines shall be used with the tmp 
// of the next func (instead of _Funct_...)
#define _tFunct_  ((tmp      ) & 0x3F)  // The funct part of the instruction register 
#define _tRd_     ((tmp >> 11) & 0x1F)  // The rd part of the instruction register 
#define _tRt_     ((tmp >> 16) & 0x1F)  // The rt part of the instruction register 
#define _tRs_     ((tmp >> 21) & 0x1F)  // The rs part of the instruction register 
#define _tSa_     ((tmp >>  6) & 0x1F)  // The sa part of the instruction register

//TODO: ? Change this to a table of function pointers?
//      Turning the main instruction handler into a switch caused slowdown
//		so maybe going the other way here will be speed up?
int psxTestLoadDelay(int reg, u32 tmp) {
	if (tmp == 0) return 0; // NOP
	switch (tmp >> 26) {
		case 0x00: // SPECIAL
			switch (_tFunct_) {
				case 0x00: // SLL
				case 0x02: case 0x03: // SRL/SRA
					if (_tRd_ == reg && _tRt_ == reg) return 1; else
					if (_tRt_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x08: // JR
					if (_tRs_ == reg) return 2;
					break;
				case 0x09: // JALR
					if (_tRd_ == reg && _tRs_ == reg) return 1; else
					if (_tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				// SYSCALL/BREAK just a break;

				case 0x20: case 0x21: case 0x22: case 0x23:
				case 0x24: case 0x25: case 0x26: case 0x27: 
				case 0x2a: case 0x2b: // ADD/ADDU...
				case 0x04: case 0x06: case 0x07: // SLLV...
					if (_tRd_ == reg && (_tRt_ == reg || _tRs_ == reg)) return 1; else
					if (_tRt_ == reg || _tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x10: case 0x12: // MFHI/MFLO
					if (_tRd_ == reg) return 3;
					break;
				case 0x11: case 0x13: // MTHI/MTLO
					if (_tRs_ == reg) return 2;
					break;

				case 0x18: case 0x19:
				case 0x1a: case 0x1b: // MULT/DIV...
					if (_tRt_ == reg || _tRs_ == reg) return 2;
					break;
			}
			break;

		case 0x01: // REGIMM
			switch (_tRt_) {
				case 0x00: case 0x01:
				case 0x10: case 0x11: // BLTZ/BGEZ...
					// Xenogears - lbu v0 / beq v0
					// - no load delay (fixes battle loading)
					break;

					if (_tRs_ == reg) return 2;
					break;
			}
			break;

		// J would be just a break;
		case 0x03: // JAL
			if (31 == reg) return 3;
			break;

		case 0x04: case 0x05: // BEQ/BNE
			// Xenogears - lbu v0 / beq v0
			// - no load delay (fixes battle loading)
			break;

			if (_tRs_ == reg || _tRt_ == reg) return 2;
			break;

		case 0x06: case 0x07: // BLEZ/BGTZ
			// Xenogears - lbu v0 / beq v0
			// - no load delay (fixes battle loading)
			break;

			if (_tRs_ == reg) return 2;
			break;

		case 0x08: case 0x09: case 0x0a: case 0x0b:
		case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x0f: // LUI
			if (_tRt_ == reg) return 3;
			break;

		case 0x10: // COP0
			switch (_tFunct_) {
				case 0x00: // MFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x02: // CFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x04: // MTC0
					if (_tRt_ == reg) return 2;
					break;
				case 0x06: // CTC0
					if (_tRt_ == reg) return 2;
					break;
				// RFE just a break;
			}
			break;

		case 0x12: // COP2
			switch (_tFunct_) {
				case 0x00: 
					switch (_tRs_) {
						case 0x00: // MFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x02: // CFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x04: // MTC2
							if (_tRt_ == reg) return 2;
							break;
						case 0x06: // CTC2
							if (_tRt_ == reg) return 2;
							break;
					}
					break;
				// RTPS... break;
			}
			break;

		case 0x22: case 0x26: // LWL/LWR
			if (_tRt_ == reg) return 3; else
			if (_tRs_ == reg) return 2;
			break;

		case 0x20: case 0x21: case 0x23:
		case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2e: // SB/SH/SWL/SW/SWR
			if (_tRt_ == reg || _tRs_ == reg) return 2;
			break;

		case 0x32: case 0x3a: // LWC2/SWC2
			if (_tRs_ == reg) return 2;
			break;
	}

	return 0;
}

/*void psxDelayTest(int reg, u32 bpc) {
	u32 *code;
	u32 tmp;

	// Don't execute yet - just peek
	code = Read_ICache(bpc, TRUE);

	tmp = ((code == NULL) ? 0 : GETLE32(code));
	branch = 1;

	switch (psxTestLoadDelay(reg, tmp)) {
		case 1:
			delayReadWrite(reg, bpc); return;
		case 2:
			delayRead(reg, bpc); return;
		case 3:
			delayWrite(reg, bpc); return;
	}
	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}*/

__inline void doBranch(uint32_t aCode, u32 tar) {
	u32 *code;
	u32 tmp;

	branch2 = branch = 1;
	branchPC = tar;
	execI();
	branch = 0;
	psxRegs.pc = branchPC;


	// branch delay slot
/*	code = Read_ICache(psxRegs.pc, TRUE);

//	psxRegs.code = ((code == NULL) ? 0 : SWAP32(*code));
	psxRegs.code = ((code == NULL) ? 0 : GETLE32(code));

	psxRegs.pc += 4;
	psxRegs.cycle += BIAS;

	// check for load delay
	tmp = psxRegs.code >> 26;
	switch (tmp) {
		case 0x10: // COP0
			switch (_Rs_) {
				case 0x00: // MFC0
				case 0x02: // CFC0
					psxDelayTest(_Rt_, branchPC);
					return;
			}
			break;
		case 0x12: // COP2
			switch (_Funct_) {
				case 0x00:
					switch (_Rs_) {
						case 0x00: // MFC2
						case 0x02: // CFC2
							psxDelayTest(_Rt_, branchPC);
							return;
					}
					break;
			}
			break;
		case 0x32: // LWC2
			psxDelayTest(_Rt_, branchPC);
			return;
		default:
			if (tmp >= 0x20 && tmp <= 0x26) { // LB/LH/LWL/LW/LBU/LHU/LWR
				psxDelayTest(_Rt_, branchPC);
				return;
			}
			break;
	}

	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = branchPC;

	*/
	psxBranchTest();
}

OPFUNC(psxNULL)		{}

/*********************************************************
* ALU                                                    *
*********************************************************/
/*********************************************************
* Arithmetic with immediate operand                      *
* Format:  OP rt, rs, immediate                          *
*********************************************************/
OPFUNC(psxSETRT0)	{ _rRt_ = 0;					}
OPFUNC(psxSETRTRS)	{ _rRt_ = _rRs_;				}
OPFUNC(psxSETRTIMM)	{ _rRt_ = _Imm_;				}
OPFUNC(psxSETRTIMMU){ _rRt_ = _ImmU_;				}

OPFUNC(psxADDI_a) 	{ _rRt_ = _rRs_ + _Imm_ ;		}				// Rt = Rs + Im 	(Exception on Integer Overflow)
OPFUNC(psxADDIU_a) 	{ _rRt_ = _rRs_ + _Imm_ ;		}				// Rt = Rs + Im
OPFUNC(psxANDI_a) 	{ _rRt_ = _rRs_ & _ImmU_;		}				// Rt = Rs And Im
OPFUNC(psxORI_a) 	{ _rRt_ = _rRs_ | _ImmU_;		}				// Rt = Rs Or  Im
OPFUNC(psxXORI_a) 	{ _rRt_ = _rRs_ ^ _ImmU_;		}				// Rt = Rs Xor Im

OPFUNC(psxSLTI_a) 	{ _rRt_ = _rRsS_ < _Imm_ ;		}				// Rt = Rs < Im		(Signed)
OPFUNC(psxSLTI_b) 	{ _rRt_ = _rRsS_ < 0;			}				// Rt = Rs < Im		(Signed)
OPFUNC(psxSLTI_c) 	{ _rRt_ = 0 < _Imm_ ;			}				// Rt = Rs < Im		(Signed)

OPFUNC(psxSLTIU_a) 	{ _rRt_ = _rRs_ < ((u32)_Imm_);	}				// Rt = Rs < Im		(Unsigned)
OPFUNC(psxSLTIU_b) 	{ _rRt_ = _rRs_ < 0ul;			}				// Rt = Rs < Im		(Unsigned)
OPFUNC(psxSLTIU_c) 	{ _rRt_ = 0 < ((u32)_Imm_);		}				// Rt = Rs < Im		(Unsigned)

OPFUNC(psxLUI_a)	{ _rRt_ = psxRegs.code << 16;	}				// Upper halfword of Rt = Im

static const psxOpFunc			ALUIMMTable[4][8] = 
{
	{psxADDI_a,		psxADDIU_a,		psxSLTI_a,		psxSLTIU_a,		psxANDI_a,		psxORI_a,		psxXORI_a,		psxLUI_a},
	{psxSETRTIMM,	psxSETRTIMM,	psxSLTI_a,		psxSLTIU_b,		psxSETRT0,		psxSETRTIMMU,	psxSETRTIMMU,	psxLUI_a},
	{psxSETRTRS,	psxSETRTRS,		psxSLTI_a,		psxSLTIU_c,		psxSETRT0,		psxSETRTRS,		psxSETRTRS,		psxLUI_a},
	{psxSETRT0,		psxSETRT0,		psxSETRT0,		psxSETRT0,		psxSETRT0,		psxSETRT0,		psxSETRT0,		psxSETRT0},
};

OPFUNC(PSXCPU_ALUIMMResolve)
{
	if(_Rt_ == 0)
	{
		*aResolve = psxNULL;
	}
	else
	{
		uint32_t instruction = (aCode >> 26) - 8;
		uint32_t style = 3;
		     if((_Rs_ != 0) && (_Imm_ != 0))		style = 0;
		else if((_Rs_ == 0) && (_Imm_ != 0))		style = 1;
		else if((_Rs_ != 0) && (_Imm_ == 0))		style = 2;

		*aResolve = ALUIMMTable[style][instruction];
	}

	PASS_IT_ON;
}

/*********************************************************
* Register arithmetic                                    *
* Format:  OP rd, rs, rt                                 *
*********************************************************/
//a: _Rs_ != _Rt_
//b: _Rs_ == _Rt_
//c: _Rs_ = 0
//d: _Rt_ = 0
//e: _Rs_ = 0 and _Rt_ = 0
OPFUNC(psxSETRD0)	{ _rRd_ = 0;					}
OPFUNC(psxSETRDRS)	{ _rRd_ = _rRs_;				}
OPFUNC(psxSETRDRT)	{ _rRd_ = _rRt_;				}

OPFUNC(psxADD_a)	{ _rRd_ =  _rRs_ + _rRt_;		}				// Rd = Rs + Rt		(Exception on Integer Overflow)
OPFUNC(psxADD_b)	{ _rRd_ =  _rRs_ + _rRs_;		}				// Rd = Rs + Rt		(Exception on Integer Overflow)

OPFUNC(psxADDU_a) 	{ _rRd_ =  _rRs_ + _rRt_;		}				// Rd = Rs + Rt
OPFUNC(psxADDU_b) 	{ _rRd_ =  _rRs_ + _rRs_;		}				// Rd = Rs + Rt

OPFUNC(psxSUB_a) 	{ _rRd_ =  _rRs_ - _rRt_;		}				// Rd = Rs - Rt		(Exception on Integer Overflow)
OPFUNC(psxSUB_c) 	{ _rRd_ =  0 - _rRt_;			}				// Rd = Rs - Rt		(Exception on Integer Overflow)

OPFUNC(psxSUBU_a) 	{ _rRd_ =  _rRs_ - _rRt_;		}				// Rd = Rs - Rt
OPFUNC(psxSUBU_c) 	{ _rRd_ =  0 - _rRt_;			}				// Rd = Rs - Rt

OPFUNC(psxAND_a) 	{ _rRd_ =  _rRs_ & _rRt_;		}				// Rd = Rs And Rt
OPFUNC(psxOR_a) 	{ _rRd_ =  _rRs_ | _rRt_;		}				// Rd = Rs Or  Rt
OPFUNC(psxXOR_a) 	{ _rRd_ =  _rRs_ ^ _rRt_;		}				// Rd = Rs Xor Rt

OPFUNC(psxNOR_a) 	{ _rRd_ =~(_rRs_ | _rRt_); 		}				// Rd = Rs Nor Rt
OPFUNC(psxNOR_b) 	{ _rRd_ =~_rRs_; 				}				// Rd = Rs Nor Rt
OPFUNC(psxNOR_c) 	{ _rRd_ =~_rRt_;	 			}				// Rd = Rs Nor Rt
OPFUNC(psxNOR_d) 	{ _rRd_ =~_rRs_; 				}				// Rd = Rs Nor Rt
OPFUNC(psxNOR_e) 	{ _rRd_ = 0xFFFFFFFF; 			}				// Rd = Rs Nor Rt

OPFUNC(psxSLT_a) 	{ _rRd_ =  _rRsS_ < _rRtS_;		}				// Rd = Rs < Rt		(Signed)
OPFUNC(psxSLT_c) 	{ _rRd_ =  0 < _rRtS_;			}				// Rd = Rs < Rt		(Signed)
OPFUNC(psxSLT_d) 	{ _rRd_ =  _rRsS_ < 0;			}				// Rd = Rs < Rt		(Signed)

OPFUNC(psxSLTU_a)	{ _rRd_ =  _rRs_ < _rRt_;		}				// Rd = Rs < Rt		(Unsigned)
OPFUNC(psxSLTU_c)	{ _rRd_ =  0 < _rRt_;			}				// Rd = Rs < Rt		(Unsigned)

static const psxOpFunc			ALUREGTable[5][12] = 
{
	{psxADD_a,		psxADDU_a,		psxSUB_a,		psxSUBU_a,		psxAND_a,		psxOR_a,		psxXOR_a,		psxNOR_a,		psxNULL,		psxNULL,		psxSLT_a,		psxSLTU_a},
	{psxADD_b,		psxADDU_b,		psxSETRD0,		psxSETRD0,		psxSETRDRS,		psxSETRDRS,		psxSETRD0,		psxNOR_b,		psxNULL,		psxNULL,		psxSETRD0,		psxSETRD0},
	{psxSETRDRT,	psxSETRDRT,		psxSUB_c,		psxSUBU_c,		psxSETRD0,		psxSETRDRT,		psxSETRDRT,		psxNOR_c,		psxNULL,		psxNULL,		psxSLT_c,		psxSLTU_c},
	{psxSETRDRS,	psxSETRDRS,		psxSETRDRS,		psxSETRDRS,		psxSETRD0,		psxSETRDRS,		psxSETRDRS,		psxNOR_d,		psxNULL,		psxNULL,		psxSLT_d,		psxSETRD0},
	{psxSETRD0,		psxSETRD0,		psxSETRD0,		psxSETRD0,		psxSETRD0,		psxSETRD0,		psxSETRD0,		psxNOR_e,		psxNULL,		psxNULL,		psxSETRD0,		psxSETRD0}
};

OPFUNC(PSXCPU_ALUREGResolve)
{
	if(_Rd_ == 0)
	{
		*aResolve = psxNULL;
	}
	else
	{
		uint32_t instruction = _Funct_ - 32;
		uint32_t style = 4;
		     if((_Rs_ && _Rt_) && (_Rs_ != _Rt_))	style = 0;
		else if((_Rs_ && _Rt_) && (_Rs_ == _Rt_))	style = 1;
		else if(!_Rs_)								style = 2;
		else if(!_Rt_)								style = 3;

		*aResolve = ALUREGTable[style][instruction];
	}

	PASS_IT_ON;
}

/*********************************************************
* Instruction Logic                                      *
*********************************************************/
/*********************************************************
* Branch and jump                                        *
*********************************************************/
#define _SetLink(x)				psxRegs.GPR.r[x] = _PC_ + 4;								// Sets the return address in the link register
#define _JumpTarget_			((_Target_ * 4) + (_PC_ & 0xf0000000))						// Calculates the target during a jump instruction
#define _BranchTarget_			((s16)_Im_ * 4 + _PC_)										// Calculates the target during a branch instruction
#define RepBranchi32(op)      	if(_rRsS_ op _rRtS_) doBranch(aCode, _BranchTarget_);
#define RepZBranchi32(op)		if(_rRsS_ op 0) doBranch(aCode, _BranchTarget_);
#define RepZBranchLinki32(op)	if(_rRsS_ op 0) { _SetLink(31); doBranch(aCode, _BranchTarget_); }

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, offset                                 *
*********************************************************/
OPFUNC(psxBGEZ)				{ RepZBranchi32(>=)											}				// Branch if Rs >= 0
OPFUNC(psxBGEZAL)			{ RepZBranchLinki32(>=)										}				// Branch if Rs >= 0 and link
OPFUNC(psxBGTZ)				{ RepZBranchi32(>)											}				// Branch if Rs >  0
OPFUNC(psxBLEZ)				{ RepZBranchi32(<=)											}				// Branch if Rs <= 0
OPFUNC(psxBLTZ)				{ RepZBranchi32(<)											}				// Branch if Rs <  0
OPFUNC(psxBLTZAL)			{ RepZBranchLinki32(<)										}				// Branch if Rs <  0 and link

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, rt, offset                             *
*********************************************************/
OPFUNC(psxBEQ)				{ RepBranchi32(==)											}				// Branch if Rs == Rt
OPFUNC(psxBNE)				{ RepBranchi32(!=)											}				// Branch if Rs != Rt

/*********************************************************
* Jump to target                                         *
* Format:  OP target                                     *
*********************************************************/
OPFUNC(psxJ)				{               doBranch(aCode, _JumpTarget_);				}
OPFUNC(psxJAL)				{ _SetLink(31); doBranch(aCode, _JumpTarget_);				}

/*********************************************************
* Register jump                                          *
* Format:  OP rs, rd                                     *
*********************************************************/
OPFUNC(psxJR)				{doBranch(aCode, _rRs_); psxJumpTest();						}

OPFUNC(psxJALR_a)			{_SetLink(_Rd_); doBranch(aCode, _rRs_);}									//_Rd_ is not Zero, _Rd_ != _Rs_
OPFUNC(psxJALR_b)			{u32 temp = _rRs_; _SetLink(_Rd_); doBranch(aCode, temp);}					//_Rd_ is not Zero, _Rd_ = _Rs_
OPFUNC(psxJALR_c)			{doBranch(aCode, _rRs_);}													//_Rd_ is Zero
OPFUNC(PSXCPU_ResolveJALR)
{
	if(_Rd_ == 0)
	{
		*aResolve = psxJALR_c;
	}
	else
	{
		*aResolve = (_Rd_ == _Rs_) ? psxJALR_b : psxJALR_a;
	}

	PASS_IT_ON;
}

/*********************************************************
* Shift                                                  *
*********************************************************/
/*********************************************************
* Shift arithmetic with constant shift                   *
* Format:  OP rd, rt, sa                                 *
*********************************************************/
OPFUNC(psxSLL_a)	{_rRd_ = _rRt_ << _Sa_;			}				// Rd = Rt << sa
OPFUNC(psxSRA_a) 	{_rRdS_ = _rRtS_ >> _Sa_;		}				// Rd = Rt >> sa (arithmetic)
OPFUNC(psxSRL_a) 	{_rRd_ = _rRt_ >> _Sa_;			}				// Rd = Rt >> sa (logical)

/*********************************************************
* Shift arithmetic with variant register shift           *
* Format:  OP rd, rt, rs                                 *
*********************************************************/
OPFUNC(psxSLLV_a) 	{_rRd_ = _rRt_ << _rRs_;		}				// Rd = Rt << rs
OPFUNC(psxSRAV_a) 	{_rRdS_ = _rRtS_ >> _rRs_;		}				// Rd = Rt >> rs (arithmetic)
OPFUNC(psxSRLV_a) 	{_rRd_ = _rRt_ >> _rRs_;		}				// Rd = Rt >> rs (logical)
OPFUNC(psxSLLV_b) 	{_rRd_ = _rRt_ << _rRt_;		}				// Rd = Rt << rs
OPFUNC(psxSRAV_b) 	{_rRdS_ = _rRtS_ >> _rRt_;		}				// Rd = Rt >> rs (arithmetic)
OPFUNC(psxSRLV_b) 	{_rRd_ = _rRt_ >> _rRt_;		}				// Rd = Rt >> rs (logical)


OPFUNC(PSXCPU_ResolveShift)
{
	if(_Rd_ == 0)
	{
		*aResolve = psxNULL;
	}
	else
	{
		switch(_Funct_)
		{
			case	0:		*aResolve = psxSLL_a;									break;
			case	2:		*aResolve = psxSRL_a;									break;
			case	3:		*aResolve = psxSRA_a;									break;
			case	4:		*aResolve = (_Rt_ != _Rs_) ? psxSLLV_a	: psxSLLV_b;	break;
			case	6:		*aResolve = (_Rt_ != _Rs_) ? psxSRLV_a	: psxSRLV_b;	break;
			case	7:		*aResolve = (_Rt_ != _Rs_) ? psxSRAV_a	: psxSRAV_b;	break;
		}
	}

	PASS_IT_ON;
}

/*********************************************************
* Register mult/div & Register trap logic                *
* Format:  OP rs, rt                                     *
*********************************************************/
OPFUNC(psxDIV)
{
	if (LIKELY(_rRtS_ != 0))
	{
		_rLoS_ = _rRsS_ / _rRtS_;
		_rHiS_ = _rRsS_ % _rRtS_;
	}
	else
	{
		_rLoS_ = 0xffffffff;
		_rHiS_ = _rRsS_;
	}
}

OPFUNC(psxDIVU)
{
	if (LIKELY(_rRt_ != 0))
	{
		_rLo_ = _rRs_ / _rRt_;
		_rHi_ = _rRs_ % _rRt_;
	}
	else
	{
		_rLo_ = 0xffffffff;
		_rHi_ = _rRs_;
	}
}

//TODO: ? Better way to handle sign
//TODO: Use a union for lo and hi
OPFUNC(psxMULT)
{
	u64 res = (s64)((s64)_rRsS_ * (s64)_rRtS_);

	psxRegs.GPR.n.lo = (u32)(res & 0xffffffff);
	psxRegs.GPR.n.hi = (u32)((res >> 32) & 0xffffffff);
}

OPFUNC(psxMULTU)
{
	u64 res = (u64)((u64)_rRs_ * (u64)_rRt_);

	psxRegs.GPR.n.lo = (u32)(res & 0xffffffff);
	psxRegs.GPR.n.hi = (u32)((res >> 32) & 0xffffffff);
}


/*********************************************************
* Special purpose instructions                           *
* Format:  OP                                            *
*********************************************************/
OPFUNC(psxBREAK) {
	// Break exception - psx rom doens't handles this
}

OPFUNC(psxSYSCALL) {
	psxRegs.pc -= 4;
	psxException(0x20, branch);
}

OPFUNC(psxRFE) {
//	SysPrintf("psxRFE\n");
	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status & 0xfffffff0) |
						  ((psxRegs.CP0.n.Status & 0x3c) >> 2);
}

/*********************************************************
* Move from HI/LO to GPR                                 *
* Format:  OP rd                                         *
*********************************************************/
OPFUNC(psxMFHI_a) { _rRd_ = _rHi_; } // Rd = Hi
OPFUNC(psxMFLO_a) { _rRd_ = _rLo_; } // Rd = Lo

OPFUNC(PSXCPU_ResolveHILO)
{
	if(_Rd_ == 0)
	{
		*aResolve = psxNULL;
	}
	else
	{
		*aResolve = (_Funct_ == 16) ? psxMFHI_a : psxMFLO_a;
	}

	PASS_IT_ON;
}


/*********************************************************
* Move to GPR to HI/LO & Register jump                   *
* Format:  OP rs                                         *
*********************************************************/
OPFUNC(psxMTHI) { _rHi_ = _rRs_; } // Hi = Rs
OPFUNC(psxMTLO) { _rLo_ = _rRs_; } // Lo = Rs

/*********************************************************
* Load and store for GPR                                 *
* Format:  OP rt, offset(base)                           *
*********************************************************/
#define _oB_ (_rRs_ + _Imm_)
u32 TOTAL_INLINE testBranchDelay(uint32_t aCode)
{
	if(UNLIKELY(branch == 0))
	{
		psxRegs.pc -= 4;
		doBranch(aCode, psxRegs.pc + 4);
		return 0;
	}

	return 1;
}


//a0: _Rt_ is zero, No branch, hardware read
//a1: _Rt_ is non-zero, No branch, hardware read
//b1: _Rt_ is non-zero, No branch, memory read
OPFUNC(psxLB_a0)		{/*psxRegs.cycle += 0;*/ psxHwRead8(_oB_);}
OPFUNC(psxLB_a1)		{/*psxRegs.cycle += 0;*/ _rRt_ = psxHwRead8(_oB_); _rRt_ |= (_rRt_ & 0x80) ? 0xFFFFFF00 : 0;}
OPFUNC(psxLB_b1)		{/*psxRegs.cycle += 0;*/ _rRt_ = PSXMEM_Memory.ReadTable[_oB_ >> 16][_oB_ & 0xFFFF]; _rRt_ |= (_rRt_ & 0x80) ? 0xFFFFFF00 : 0;}

OPFUNC(psxLBU_a0)		{/*psxRegs.cycle += 0;*/ psxHwRead8(_oB_);}
OPFUNC(psxLBU_a1)		{/*psxRegs.cycle += 0;*/ _rRt_ = psxHwRead8(_oB_);}
OPFUNC(psxLBU_b1)		{/*psxRegs.cycle += 0;*/ _rRt_ = PSXMEM_Memory.ReadTable[_oB_ >> 16][_oB_ & 0xFFFF];}

OPFUNC(psxLH_a0)		{psxRegs.cycle += 1; psxHwRead16(_oB_);}
OPFUNC(psxLH_a1)		{psxRegs.cycle += 1; _rRt_ = psxHwRead16(_oB_); _rRt_ |= (_rRt_ & 0x8000) ? 0xFFFF0000 : 0;}
OPFUNC(psxLH_b1)		{psxRegs.cycle += 1; _rRt_ = GETLE16(&PSXMEM_Memory.ReadTable[_oB_ >> 16][_oB_ & 0xFFFF]); _rRt_ |= (_rRt_ & 0x8000) ? 0xFFFF0000 : 0;}

OPFUNC(psxLHU_a0)		{psxRegs.cycle += 1; psxHwRead16(_oB_);}
OPFUNC(psxLHU_a1)		{psxRegs.cycle += 1; _rRt_ = psxHwRead16(_oB_);}
OPFUNC(psxLHU_b1)		{psxRegs.cycle += 1; _rRt_ = GETLE16(&PSXMEM_Memory.ReadTable[_oB_ >> 16][_oB_ & 0xFFFF]);}

OPFUNC(psxLW_a0)		{psxRegs.cycle += 1; psxHwRead32(_oB_);}
OPFUNC(psxLW_a1)		{psxRegs.cycle += 1; _rRt_ = psxHwRead32(_oB_);}
OPFUNC(psxLW_b1)		{psxRegs.cycle += 1; _rRt_ = GETLE32(&PSXMEM_Memory.ReadTable[_oB_ >> 16][_oB_ & 0xFFFF]);}

u32 LWL_MASK[4] = { 0xffffff, 0xffff, 0xff, 0 };
u32 LWL_SHIFT[4] = { 24, 16, 8, 0 };

OPFUNC(psxLWL_a0)		{psxRegs.cycle += 1; psxHwRead32(_oB_ & ~3);}
OPFUNC(psxLWL_a1)
{
	psxRegs.cycle += 1;
	uint32_t mem = psxHwRead32(_oB_ & ~3);
	uint32_t shift = _oB_ & 3;
	_rRt_ =	(_rRt_ & LWL_MASK[shift]) | (mem << LWL_SHIFT[shift]);

}

OPFUNC(psxLWL_b1)
{
	psxRegs.cycle += 1;
	uint32_t addr = _oB_ & ~3;
	uint32_t shift = _oB_ & 3;
	uint32_t mem = GETLE32(&PSXMEM_Memory.ReadTable[addr >> 16][addr & 0xFFFF]);
	_rRt_ =	(_rRt_ & LWL_MASK[shift]) | (mem << LWL_SHIFT[shift]);
}

u32 LWR_MASK[4] = { 0, 0xff000000, 0xffff0000, 0xffffff00 };
u32 LWR_SHIFT[4] = { 0, 8, 16, 24 };

OPFUNC(psxLWR_a0)		{psxRegs.cycle += 1; psxHwRead32(_oB_ & ~3);}
OPFUNC(psxLWR_a1)
{
	psxRegs.cycle += 1;
	uint32_t mem = psxHwRead32(_oB_ & ~3);
	uint32_t shift = _oB_ & 3;
	_rRt_ =	(_rRt_ & LWR_MASK[shift]) | (mem >> LWR_SHIFT[shift]);
}

OPFUNC(psxLWR_b1)
{
	psxRegs.cycle += 1;
	uint32_t addr = _oB_ & ~3;
	uint32_t mem = GETLE32(&PSXMEM_Memory.ReadTable[addr >> 16][addr & 0xFFFF]);
	uint32_t shift = _oB_ & 3;
	_rRt_ =	(_rRt_ & LWR_MASK[shift]) | (mem >> LWR_SHIFT[shift]);
}

OPFUNC(PSXCPU_ResolveLoad)
{
	if(_Rt_ == 0 && (_oB_ > 0x1F801000 && _oB_ < 0x1F808000))
	{
		switch(aCode >> 26)
		{
			case 32:	*aResolve = psxLB_a0; break;
			case 33:	*aResolve = psxLH_a0; break;
			case 34:	*aResolve = psxLWL_a0; break;
			case 35:	*aResolve = psxLW_a0; break;
			case 36:	*aResolve = psxLBU_a0; break;
			case 37:	*aResolve = psxLHU_a0; break;
			case 38:	*aResolve = psxLWR_a0; break;
		}
	}
	else if(_Rt_ == 0)
	{
		*aResolve = psxNULL;
	}
	else if(_oB_ > 0x1F801000 && _oB_ < 0x1F808000)
	{
		switch(aCode >> 26)
		{
			case 32:	*aResolve = psxLB_a1; break;
			case 33:	*aResolve = psxLH_a1; break;
			case 34:	*aResolve = psxLWL_a1; break;
			case 35:	*aResolve = psxLW_a1; break;
			case 36:	*aResolve = psxLBU_a1; break;
			case 37:	*aResolve = psxLHU_a1; break;
			case 38:	*aResolve = psxLWR_a1; break;
		}
	}
	else
	{
		switch(aCode >> 26)
		{
			case 32:	*aResolve = psxLB_b1; break;
			case 33:	*aResolve = psxLH_b1; break;
			case 34:	*aResolve = psxLWL_b1; break;
			case 35:	*aResolve = psxLW_b1; break;
			case 36:	*aResolve = psxLBU_b1; break;
			case 37:	*aResolve = psxLHU_b1; break;
			case 38:	*aResolve = psxLWR_b1; break;
		}
	}

	PASS_IT_ON;
}

//a0: _Rt_ is zero, No branch, hardware write
//a1: _Rt_ is non-zero, No branch, hardware write
//b0: _Rt_ is zero, No branch, memory write
//b1: _Rt_ is non-zero, No branch, memory write
OPFUNC(psxSB_a0)		{psxRegs.cycle += 1; psxHwWrite8(_oB_, 0);}
OPFUNC(psxSB_a1)		{psxRegs.cycle += 1; psxHwWrite8(_oB_, _rRt_);}
OPFUNC(psxSB_b0)		{psxRegs.cycle += 1; PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF] = 0; *PSXMop(_oB_) = PSXCPU_Resolve;}
OPFUNC(psxSB_b1)		{psxRegs.cycle += 1; PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF] = _rRt_; *PSXMop(_oB_) = PSXCPU_Resolve;}

OPFUNC(psxSH_a0)		{psxRegs.cycle += 1; psxHwWrite16(_oB_, 0);}
OPFUNC(psxSH_a1)		{psxRegs.cycle += 1; psxHwWrite16(_oB_, _rRt_);}
OPFUNC(psxSH_b0)		{psxRegs.cycle += 1; PUTLE16(&PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF], 0); *PSXMop(_oB_) = PSXCPU_Resolve;}
OPFUNC(psxSH_b1)		{psxRegs.cycle += 1; PUTLE16(&PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF], _rRt_); *PSXMop(_oB_) = PSXCPU_Resolve;}

OPFUNC(psxSW_a0)		{psxRegs.cycle += 1; psxHwWrite32(_oB_, 0);}
OPFUNC(psxSW_a1)		{psxRegs.cycle += 1; psxHwWrite32(_oB_, _rRt_);}
OPFUNC(psxSW_b0)		{psxRegs.cycle += 1; PUTLE32(&PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF], 0); *PSXMop(_oB_) = PSXCPU_Resolve;}
OPFUNC(psxSW_b1)		{psxRegs.cycle += 1; PUTLE32(&PSXMEM_Memory.WriteTable[_oB_ >> 16][_oB_ & 0xFFFF], _rRt_); *PSXMop(_oB_) = PSXCPU_Resolve;}
OPFUNC(psxSW_cf)		{psxMemWrite32(_oB_, _rRt_);}

OPFUNC(PSXCPU_ResolveStore)
{
	*aResolve = psxNULL;

	if(_oB_ == 0xFFFE0130)
	{
		switch(aCode >> 26)
		{
			case 43:	*aResolve = psxSW_cf; break;
		}
	}
	else if(_Rt_ == 0 && (_oB_ > 0x1F801000 && _oB_ < 0x1F808000))
	{
		switch(aCode >> 26)
		{
			case 40:	*aResolve = psxSB_a0; break;
			case 41:	*aResolve = psxSH_a0; break;
			case 43:	*aResolve = psxSW_a0; break;
		}
	}
	else if(_Rt_ == 0)
	{
		switch(aCode >> 26)
		{
			case 40:	*aResolve = psxSB_b0; break;
			case 41:	*aResolve = psxSH_b0; break;
			case 43:	*aResolve = psxSW_b0; break;
		}
	}
	else if(_oB_ > 0x1F801000 && _oB_ < 0x1F808000)
	{
		switch(aCode >> 26)
		{
			case 40:	*aResolve = psxSB_a1; break;
			case 41:	*aResolve = psxSH_a1; break;
			case 43:	*aResolve = psxSW_a1; break;
		}
	}
	else
	{
		switch(aCode >> 26)
		{
			case 40:	*aResolve = psxSB_b1; break;
			case 41:	*aResolve = psxSH_b1; break;
			case 43:	*aResolve = psxSW_b1; break;
		}
	}

	PASS_IT_ON;
}



u32 SWL_MASK[4] = { 0xffffff00, 0xffff0000, 0xff000000, 0 };
u32 SWL_SHIFT[4] = { 24, 16, 8, 0 };

OPFUNC(psxSWL) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_rRt_ >> SWL_SHIFT[shift]) |
			     (  mem & SWL_MASK[shift]) );
	/*
	Mem = 1234.  Reg = abcd

	0   123a   (reg >> 24) | (mem & 0xffffff00)
	1   12ab   (reg >> 16) | (mem & 0xffff0000)
	2   1abc   (reg >>  8) | (mem & 0xff000000)
	3   abcd   (reg      ) | (mem & 0x00000000)
	*/
}

u32 SWR_MASK[4] = { 0, 0xff, 0xffff, 0xffffff };
u32 SWR_SHIFT[4] = { 0, 8, 16, 24 };

OPFUNC(psxSWR) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_rRt_ << SWR_SHIFT[shift]) |
			     (  mem & SWR_MASK[shift]) );

	/*
	Mem = 1234.  Reg = abcd

	0   abcd   (reg      ) | (mem & 0x00000000)
	1   bcd4   (reg <<  8) | (mem & 0x000000ff)
	2   cd34   (reg << 16) | (mem & 0x0000ffff)
	3   d234   (reg << 24) | (mem & 0x00ffffff)
	*/
}

/*********************************************************
* Moves between GPR and COPx                             *
* Format:  OP rt, fs                                     *
*********************************************************/
OPFUNC(psxMFC0)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
	
		_rRtS_ = (int)_rFs_;
	}
}

OPFUNC(psxCFC0)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
	
		_rRtS_ = (int)_rFs_;
	}
}

void psxTestSWInts() {
	// the next code is untested, if u know please
	// tell me if it works ok or not (linuzappz)
	if (psxRegs.CP0.n.Cause & psxRegs.CP0.n.Status & 0x0300 &&
		psxRegs.CP0.n.Status & 0x1) {
		psxException(psxRegs.CP0.n.Cause, branch);
	}
}

__inline void MTC0(int reg, u32 val) {
//	SysPrintf("MTC0 %d: %x\n", reg, val);
	switch (reg) {
		case 12: // Status
			psxRegs.CP0.r[12] = val;
			psxTestSWInts();
			break;

		case 13: // Cause
			psxRegs.CP0.n.Cause = val & ~(0xfc00);
			psxTestSWInts();
			break;

		default:
			psxRegs.CP0.r[reg] = val;
			break;
	}
}

OPFUNC(psxMTC0) { MTC0(_Rd_, _rRt_); }
OPFUNC(psxCTC0) { MTC0(_Rd_, _rRt_); }



OPFUNC(psxMFC2)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		gteMFC2(aCode);
	}
}


OPFUNC(psxCFC2)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		gteCFC2(aCode);
	}
}


/*********************************************************
* Others											     *
*********************************************************/
OPFUNC(PSXCPU_SPCResolve)
{
	*aResolve = psxSPC[_Funct_];
	PASS_IT_ON;
}

OPFUNC(PSXCPU_REGIMMResolve)
{
	*aResolve = psxREG[_Rt_];
	PASS_IT_ON;
}

OPFUNC(PSXCPU_COP0Resolve)
{
	*aResolve = psxCP0[_Rs_];
	PASS_IT_ON;
}

OPFUNC(PSXCPU_BASICResolve)
{
	*aResolve = psxCP2BSC[_Rs_];
	PASS_IT_ON;
}

OPFUNC(psxCOP2)		{if ((psxRegs.CP0.n.Status & 0x40000000) != 0 ) psxCP2[_Funct_](aCode, aResolve);}
OPFUNC(psxHLE)		{psxHLEt[psxRegs.code & 0x07]();}

psxOpFunc psxBSC[64] = {
	PSXCPU_SPCResolve,		PSXCPU_REGIMMResolve,	psxJ,					psxJAL,					psxBEQ,					psxBNE,					psxBLEZ,				psxBGTZ,
	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,	PSXCPU_ALUIMMResolve,
	PSXCPU_COP0Resolve,		psxNULL,				psxCOP2,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		PSXCPU_ResolveLoad,		psxNULL,
	PSXCPU_ResolveStore,	PSXCPU_ResolveStore,	psxSWL,					PSXCPU_ResolveStore,	psxNULL,				psxNULL,				psxSWR,					psxNULL, 
	psxNULL,				psxNULL,				gteLWC2,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				gteSWC2,				psxHLE,					psxNULL,				psxNULL,				psxNULL,				psxNULL 
};


psxOpFunc psxSPC[64] = {
	PSXCPU_ResolveShift,	psxNULL,				PSXCPU_ResolveShift,	PSXCPU_ResolveShift,	PSXCPU_ResolveShift,	psxNULL,				PSXCPU_ResolveShift,	PSXCPU_ResolveShift,
	psxJR,					PSXCPU_ResolveJALR,		psxNULL,				psxNULL,				psxSYSCALL,				psxBREAK,				psxNULL,				psxNULL,
	PSXCPU_ResolveHILO,		psxMTHI,				PSXCPU_ResolveHILO,		psxMTLO,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxMULT,				psxMULTU,				psxDIV,					psxDIVU,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,
	psxNULL,				psxNULL,				PSXCPU_ALUREGResolve,	PSXCPU_ALUREGResolve,	psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL
};

psxOpFunc psxREG[32] = {
	psxBLTZ,				psxBGEZ,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxBLTZAL,				psxBGEZAL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL
};

psxOpFunc psxCP0[32] = {
	psxMFC0,				psxNULL,				psxCFC0,				psxNULL,				psxMTC0,				psxNULL,				psxCTC0,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxRFE,					psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL
};

psxOpFunc psxCP2[64] = {
	PSXCPU_BASICResolve,	gteRTPS,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				gteNCLIP,				psxNULL, // 00
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				gteOP,					psxNULL,				psxNULL,				psxNULL, // 08
	gteDPCS,				gteINTPL,				gteMVMVA,				gteNCDS,				gteCDP,					psxNULL,				gteNCDT,				psxNULL, // 10
	psxNULL,				psxNULL,				psxNULL,				gteNCCS,				gteCC,					psxNULL,				gteNCS,					psxNULL, // 18
	gteNCT,					psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL, // 20
	gteSQR,					gteDCPL,				gteDPCT,				psxNULL,				psxNULL,				gteAVSZ3,				gteAVSZ4,				psxNULL, // 28 
	gteRTPT,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL, // 30
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				gteGPF,					gteGPL,					gteNCCT  // 38
};

psxOpFunc psxCP2BSC[32] = {
	psxMFC2,				psxNULL,				psxCFC2,				psxNULL,				gteMTC2,				psxNULL,				gteCTC2,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,
	psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL,				psxNULL
};


///////////////////////////////////////////
int wanna_leave = 0;

void				PSXCPU_Resolve						(uint32_t aOpCode, psxOpFunc* aResolve)
{
	*aResolve = psxBSC[aOpCode >> 26];
	(*aResolve)(aOpCode, aResolve);
}

int					PSXCPU_Init							()
{
	return 0;
}

void				PSXCPU_Reset						()
{
	psxRegs.ICache_valid = FALSE;
}

void				PSXCPU_Execute						()
{
	wanna_leave = 0;
	while(LIKELY(wanna_leave == 0))
	{
		execI();
	}
}

void				PSXCPU_ExecuteBlock					()
{
	branch2 = 0;
	while(!branch2)
	{
		execI();
	}
}

void				PSXCPU_Clear						(uint32_t aAddress, uint32_t aSize)
{
	for(int i = 0; i != aSize; i ++)
	{
		*PSXMop(aAddress + i) = PSXCPU_Resolve;
	}
}

void				PSXCPU_Shutdown						()
{
}

