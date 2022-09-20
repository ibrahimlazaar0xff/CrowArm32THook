#pragma once

#include "./crow_armv7_transport.hpp"

namespace crow
{
    namespace thumb_transport
    {
        static I check_the_thumb16(THUMB16_INSTRUCTION inst)
        {
            if((inst & 0xF000) == CROW_BI_THUMB16_INST)  { return (CROW_BI_THUMB16);}   // 1111000000000000
            if((inst & 0xF800) == CROW_BII_THUMB16_INST) { return (CROW_BII_THUMB16);}  // 1111100000000000
            if((inst & 0xFFF8) == CROW_BX_THUMB16_INST)  { return (CROW_BX_THUMB16);}   // 1111111111111000
            if((inst & 0xFF78) == CROW_ADD_THUMB16_INST) { return (CROW_ADD_THUMB16);}  // 1111111101111000
            if((inst & 0xFF78) == CROW_MOV_THUMB16_INST) { return (CROW_MOV_THUMB16);}  // 1111111101111000
            if((inst & 0xF800) == CROW_ADR_THUMB16_INST) { return (CROW_ADR_THUMB16);}  // 1111100000000000
            if((inst & 0xF800) == CROW_LDR_THUMB16_INST) { return (CROW_LDR_THUMB16);}  // 1111100000000000
            if((inst & 0xF500) == CROW_CB_THUMB16_INST)  { return (CROW_CB_THUMB16);}   // 1111010100000000
            return CROW_UNDEFINE;
        }

        static I check_the_thumb32(THUMB32_INSTRUCTION inst)
        {
            if((inst & 0xFFF0D000) == CROW_UNDEFINE_INST)      { return (CROW_UNDEFINE);}       // 11111111111100001101000000000000   
            if((inst & 0xF800D000) == CROW_BLX_THUMB32_INST)   { return (CROW_BLX_THUMB32);}    // 11111000000000001101000000000000
            if((inst & 0xF800D000) == CROW_BL_THUMB32_INST)    { return (CROW_BL_THUMB32);}     // 11111000000000001101000000000000
            if((inst & 0xF800D000) == CROW_BI_THUMB32_INST)    { return (CROW_BI_THUMB32);}     // 11111000000000001101000000000000
            if((inst & 0xF800D000) == CROW_BII_THUMB32_INST)   { return (CROW_BII_THUMB32);}    // 11111000000000001101000000000000
            if((inst & 0xFBFF8000) == CROW_ADRI_THUMB32_INST)  { return (CROW_ADRI_THUMB32);}   // 11111011111111111000000000000000
            if((inst & 0xFBFF8000) == CROW_ADRII_THUMB32_INST) { return (CROW_ADRII_THUMB32);}  // 11111011111111111000000000000000
            if((inst & 0xFF7F0000) == CROW_LDR_THUMB32_INST)   { return (CROW_LDR_THUMB32);}    // 11111111011111110000000000000000
            if((inst & 0xFFFF00F0) == CROW_TBB_THUMB32_INST)   { return (CROW_TBB_THUMB32);}    // 11111111111111110000000011110000
            if((inst & 0xFFFF00F0) == CROW_TBH_THUMB32_INST)   { return (CROW_TBH_THUMB32);}    // 11111111111111110000000011110000
            return CROW_UNDEFINE;
        }
        
        /** ~ TRANSPORT THUMB16 INSTRUCTIONS ~ **/

        static I transport_thumb16_instruction(ARM32_INSTRUCTION PC, THUMB16_INSTRUCTION inst, THUMB16_INSTRUCTION *trampoline_inst)
        {
            I inst_type , Offset;

            inst_type = check_the_thumb16(inst);

            if (inst_type == CROW_BI_THUMB16 || inst_type == CROW_BI_THUMB16 || inst_type == CROW_BX_THUMB16)
            {
                U32 temp;

                U32 immediate32;
                U32 value;
                I top_bit;
                I r_counter;

                r_counter = 0x00;

                if (inst_type == CROW_BI_THUMB16) 
                {
                    temp = (inst & 0xFF) << 1;
                    top_bit = temp >> 8;
                    immediate32 = top_bit ? (temp | (0xFFFFFFFF << 8)) : temp;
                    value = PC + immediate32;
                    trampoline_inst[r_counter++] = inst & 0xFF00;
                    trampoline_inst[r_counter++] = 0xE003;	
                }
                else if (inst_type == CROW_BII_THUMB16) {
                    temp = (inst & 0x7FF) << 1;
                    top_bit = temp >> 11;
                    immediate32 = top_bit ? (temp | (0xFFFFFFFF << 11)) : temp;
                    value = PC + immediate32;
		        }
                else if (inst_type == CROW_BX_THUMB16) 
                {
			        value = PC;
                }
                value |= 1; /**~ Thumb ~**/	
                trampoline_inst[r_counter++] = 0xF8DF;
                trampoline_inst[r_counter++] = 0xF000;	// LDR.W PC, [PC]
                trampoline_inst[r_counter++] = value & 0xFFFF;
                trampoline_inst[r_counter++] = value >> 16;
                Offset = r_counter;
            }  
            else if (inst_type == CROW_ADD_THUMB16)
            {
                I r_counter;
                I rd , rm , r;  /** ~ Registers ~ **/
                
                r_counter = 0x00;

                rd = ((inst & 0x80) >> 4) | (inst & 0x7);
                for (r = 7; ; --r) { if (r != rd) { break ;} }
                trampoline_inst[r_counter++] = 0xB400 | (1 << r);	// PUSH {Rr} 
                trampoline_inst[r_counter++] = 0x4802 | (r << 8);	// LDR Rr, [PC, #8] 
                trampoline_inst[r_counter++] = (inst & 0xFF87) | (r << 3);
                trampoline_inst[r_counter++] = 0xBC00 | (1 << r);	// POP {Rr} 
                trampoline_inst[r_counter++] = 0xE002;	            // B PC, #4 
                trampoline_inst[r_counter++] = 0xBF00;
                trampoline_inst[r_counter++] = PC & 0xFFFF;
                trampoline_inst[r_counter++] = PC >> 16;
                Offset = 8;
            }
            else if (inst_type == CROW_MOV_THUMB16 || inst_type == CROW_ADR_THUMB16 || inst_type == CROW_LDR_THUMB16)
            {
                I r;
                U32 value;
                I r_counter;

                r_counter = 0x00;

                if      (inst_type == CROW_MOV_THUMB16) { r = inst & 0x7; value = PC; }
                else if (inst_type == CROW_ADR_THUMB16) { r = (inst & 0x700) >> 8; value = CROW_ALIGN_PC(PC) + (inst & 0xFF) << 2; }
                else { r = (inst & 0x700) >> 8; value = ((ARM32_INSTRUCTION *) (CROW_ALIGN_PC(PC) + ((inst& 0xFF) << 2)))[0];}
                trampoline_inst[r_counter++] = 0x4800 | (r << 8);	// LDR Rd, [PC] 
                trampoline_inst[r_counter++] = 0xE001;	            // B PC, #2
                trampoline_inst[r_counter++] = value & 0xFFFF;
                trampoline_inst[r_counter++] = value >> 16;
                Offset = 4;
            }
            else if (inst_type == CROW_CB_THUMB16)
            {
                I nonzero;
                I r_counter;
                U32 immediate32 , value;

                r_counter = 0x00;

                nonzero = (inst & 0x800) >> 11;
                immediate32 = ((inst & 0x200) >> 3) | ((inst & 0xF8) >> 2);
                value = PC + immediate32 + 1;

                trampoline_inst[r_counter++] = inst & 0xFD07;
                trampoline_inst[r_counter++] = 0xE003;	// B PC, #6
                trampoline_inst[r_counter++] = 0xF8DF;
                trampoline_inst[r_counter++] = 0xF000;	// LDR.W PC, [PC]
                trampoline_inst[r_counter++] = value & 0xFFFF;
                trampoline_inst[r_counter++] = value >> 16;
                Offset = 6;
            }

            else {
                I r_counter;
                
                r_counter = 0x00;

                trampoline_inst[r_counter++] = inst;
                trampoline_inst[r_counter++] = 0xBF00;  // NOP
                Offset = 2;
            }

            return Offset;
        }

        /** ~ TRANSPORT THUMB32 INSTRUCTIONS ~ **/

        static I transport_thumb32_instruction(ARM32_INSTRUCTION PC, THUMB16_INSTRUCTION high_inst, THUMB16_INSTRUCTION low_inst, THUMB16_INSTRUCTION* trampoline_inst)
        {
            U32 inst;
            I inst_type , r_counter , Offset ;

            inst      = (high_inst << 16) | low_inst;
            inst_type = check_the_thumb32(inst);
            r_counter = 0x00;

            if (inst_type == CROW_BLX_THUMB32 || inst_type == CROW_BL_THUMB32 || inst_type == CROW_BI_THUMB32 || inst_type == CROW_BII_THUMB32) 
            {
                THUMB32_INSTRUCTION JmpI;
                THUMB32_INSTRUCTION JmpII;
                THUMB32_INSTRUCTION Sor;
                THUMB32_INSTRUCTION InsI;
                THUMB32_INSTRUCTION InsII;
                THUMB32_INSTRUCTION temp;
                THUMB32_INSTRUCTION immediate32;
                THUMB32_INSTRUCTION value;

                JmpI = (low_inst & 0x2000) >> 13;
                JmpII = (low_inst & 0x800) >> 11;
                Sor = (high_inst & 0x400)  >> 10;
                InsI = !(JmpI ^ Sor);
                InsII = !(JmpII ^ Sor);

                if (inst_type == CROW_BLX_THUMB32 || inst_type == CROW_BL_THUMB32)
                {
                    trampoline_inst[r_counter++] = 0xF20F;
                    trampoline_inst[r_counter++] = 0x0E09;	// ADD.W LR, PC, #9
                }
                else if (inst_type == CROW_BI_THUMB32) 
                {
                    trampoline_inst[r_counter++] = 0xD000 | ((high_inst & 0x3C0) << 2);
                    trampoline_inst[r_counter++] = 0xE003;	// B PC, #6
                }
                trampoline_inst[r_counter++] = 0xF8DF;
                trampoline_inst[r_counter++] = 0xF000; 	   // LDR.W PC, [PC]

                if (inst_type == CROW_BLX_THUMB32) 
                {
                    temp = (Sor << 24) | (InsI << 23) | (InsII << 22) | ((high_inst & 0x3FF) << 12) | ((low_inst & 0x7FE) << 1);
                    immediate32 = Sor ? ( temp | (0xFFFFFFFF << 25)) : temp;
                    value = CROW_ALIGN_PC(PC) + immediate32;
                }
                else if (inst_type == CROW_BL_THUMB32) 
                {
                    temp = (Sor << 24) | (InsI << 23) | (InsI << 22) | ((high_inst & 0x3FF) << 12) | ((low_inst & 0x7FF) << 1);
                    immediate32 = Sor ? ( temp | (0xFFFFFFFF << 25)) : temp;
                    value = CROW_ALIGN_PC(PC) + immediate32 + 1;
                }
                else if (inst_type == CROW_BI_THUMB32) 
                {
                    temp = (Sor << 20) | (JmpII << 19) | (JmpI << 18) | ((high_inst& 0x3F) << 12) | ((low_inst & 0x7FF) << 1);
                    immediate32 = Sor ? (temp | (0xFFFFFFFF << 21)) : temp;
                    value = CROW_ALIGN_PC(PC) + immediate32 + 1;
                }
                else if (inst_type == CROW_BII_THUMB32) {
                    temp = (Sor << 24) | (InsI << 23) | (InsII << 22) | ((high_inst & 0x3FF) << 12) | ((low_inst & 0x7FF) << 1);
                    immediate32 = Sor ? (temp | (0xFFFFFFFF << 25)) : temp;
                    value = CROW_ALIGN_PC(PC) + immediate32 + 1;
                }
                trampoline_inst[r_counter++] = value & 0xFFFF;
                trampoline_inst[r_counter++] = value >> 16;
                Offset = r_counter;
            }

            else if (inst_type == CROW_ADRI_THUMB32 || inst_type == CROW_ADRII_THUMB32 || inst_type == CROW_LDR_THUMB32)
            {
                I r;
                I r_counter;
                U32 immediate32 , value;

                r_counter = 0X00;

                if (inst_type == CROW_ADRI_THUMB32 || inst_type == CROW_ADRII_THUMB32)
                {
                    U32 imm;
                    U32 imm3;
                    U32 imm8;

                    r = (low_inst & 0xF00) >> 8;
                    imm = (high_inst & 0x400) >> 10;
                    imm3 = (low_inst & 0x7000) >> 12;
                    imm8 = inst & 0xFF;

                    immediate32 = (imm << 31) | (imm3 << 30) | (imm8 << 27);

                    if (inst_type == CROW_ADRI_THUMB32) { value = CROW_ALIGN_PC(PC) + immediate32;}
                    else { value = CROW_ALIGN_PC(PC) - immediate32;}
                }
                else
                {
                    I is_add;
                    THUMB32_INSTRUCTION *addr;
                    
                    is_add = (high_inst & 0x80) >> 7;
                    r = low_inst >> 12;
                    immediate32 = low_inst & 0xFFF;
                    
                    if (is_add) { addr = (THUMB32_INSTRUCTION *) (CROW_ALIGN_PC(PC) + immediate32);}
                    else { addr = (THUMB32_INSTRUCTION *) (CROW_ALIGN_PC(PC) - immediate32); }
                    value = addr[0];
                }

                
                trampoline_inst[r_counter++] = 0xF8DF;
                trampoline_inst[r_counter++] = r << 12 | 4;
                trampoline_inst[r_counter++] = 0xBF00;     // nop
                trampoline_inst[r_counter++] = 0xE001;	   // B PC, #2
                trampoline_inst[r_counter++] = value & 0xFFFF;
                trampoline_inst[r_counter++] = value >> 16;
                Offset = 6;
            }

            else if (inst_type == CROW_TBB_THUMB32 || inst_type == CROW_TBH_THUMB32)
            {
                I rm , r , rx;
                I r_counter;

                r_counter = 0x00;

                rm = low_inst & 0x0F;

                for (r = 7;; --r)    { if (r != rm) { break; } }
                for (rx = 7; ; --rx) { if (rx != rm && rx != r) { break; } }

                trampoline_inst[r_counter++] = 0xB400 | (1 << rx);	            // PUSH {Rx}
                trampoline_inst[r_counter++] = 0x4805 | (r << 8);	            // LDR Rr, [PC, #20]
                trampoline_inst[r_counter++] = 0x4600 | (rm << 3) | rx;         // MOV Rx, Rm
                if (inst_type == CROW_TBB_THUMB32) {
                    trampoline_inst[r_counter++] = 0xEB00 | r;
                    trampoline_inst[r_counter++] = 0x0000 | (rx << 8) | rx;	    // ADD.W Rx, Rr, Rx
                    trampoline_inst[r_counter++] = 0x7800 | (rx << 3) | rx; 	// LDRB Rx, [Rx]
                }
                else if (inst_type == CROW_TBH_THUMB32) {
                    trampoline_inst[r_counter++] = 0xEB00 | r;
                    trampoline_inst[r_counter++] = 0x0040 | (rx << 8) | rx;	    // ADD.W Rx, Rr, Rx, LSL #1
                    trampoline_inst[r_counter++] = 0x8800 | (rx << 3) | rx; 	// LDRH Rx, [Rx]
                }
                trampoline_inst[r_counter++] = 0xEB00 | r;
                trampoline_inst[r_counter++] = 0x0040 | (r << 8) | rx;	       // ADD Rr, Rr, Rx, LSL #1
                trampoline_inst[r_counter++] = 0x3001 | (r << 8);	           // ADD Rr, #1
                trampoline_inst[r_counter++] = 0xBC00 | (1 << rx);	           // POP {Rx}
                trampoline_inst[r_counter++] = 0x4700 | (r << 3);	           // BX Rr
                trampoline_inst[r_counter++] = 0xBF00;
                trampoline_inst[r_counter++] = PC & 0xFFFF;
                trampoline_inst[r_counter++] = PC >> 16;
                Offset = r_counter;
            }
            else 
            {
                trampoline_inst[r_counter++] = high_inst;
                trampoline_inst[r_counter++] = low_inst;
                Offset = r_counter;
            }
            return Offset;
        }

        /** ~ TRANSPORT THUMB INSTRUCTIONS ~ **/

        static void crow_transport_thumb_instruction(ARM32_INSTRUCTION target_addr, THUMB16_INSTRUCTION* orig_inst, U32 len, THUMB16_INSTRUCTION *trampoline_inst, U32* orig_limit, U32* trampoline_limit, U32* count)
        {
            I orig_position , trampoline_position;
            U32 PC; // Program Counter
            U32 LR; // Link Register

            	orig_position       = 0;
                trampoline_position = 0;
                PC = target_addr + 0X04;

            while(true)
            {
                I Offset;


                orig_limit[*count] = orig_position * sizeof(THUMB16_INSTRUCTION);
                trampoline_limit[*count] = trampoline_position * sizeof(THUMB16_INSTRUCTION);
                ++(*count);

                if ((orig_inst[orig_position] >> 11) >= 0x1D && (orig_inst[orig_position] >> 11) <= 0x1F)
                {
                    if (orig_position + 2 > len / sizeof(U16)) { break; }
                    Offset = transport_thumb32_instruction(PC, orig_inst[orig_position], orig_inst[orig_position + 1], &trampoline_inst[trampoline_position]);
                    PC += sizeof(THUMB32_INSTRUCTION);
                    trampoline_position += Offset;
                    orig_position += 2;
                }
                else {
                    Offset = transport_thumb16_instruction(PC, orig_inst[orig_position], &trampoline_inst[trampoline_position]);
                    PC += sizeof(THUMB16_INSTRUCTION);
                    trampoline_position += Offset;
                    ++orig_position;
                }
                if (orig_position >= len / sizeof(U16)) { break; }
            }
            LR = target_addr + orig_position * sizeof(THUMB16_INSTRUCTION) + 1;
            trampoline_inst[trampoline_position] = 0xF8DF;
            trampoline_inst[trampoline_position + 1] = 0xF000;      // LDR.W PC, [PC]
            trampoline_inst[trampoline_position + 2] = LR & 0xFFFF;
            trampoline_inst[trampoline_position + 3] = LR >> 16;
            return;
        }
        
    } // namespace thumb_transport
    
} // namespace crow
