#pragma once

#include "./crow_armv7_transport.hpp"

namespace crow 
{
    namespace arm32_transport
    {
        static I check_the_arm32(ARM32_INSTRUCTION inst)
        {
            if((inst & 0xFE000000)== CROW_BLX_ARM_INST)   { return (CROW_BLX_ARM);}    // 11111010000000000000000000000000
            if((inst & 0xF000000) == CROW_BL_ARM_INST)    { return (CROW_BL_ARM);}     // 00001011000000000000000000000000
            if((inst & 0xF000000) == CROW_B_ARM_INST)     { return (CROW_B_ARM);}      // 00001010000000000000000000000000
            if((inst & 0xFF000FF) == CROW_BX_ARM_INST)    { return (CROW_BX_ARM);}     // 00000001001000000000000000011111
            if((inst & 0xFEF0010) == CROW_ADD_ARM_INST)   { return (CROW_ADD_ARM);}    // 00000000100011110000000000000000
            if((inst & 0xFFF0000) == CROW_ADRI_ARM_INST)  { return (CROW_ADRI_ARM);}   // 00000010100011110000000000000000
            if((inst & 0xFFF0000) == CROW_ADRII_ARM_INST) { return (CROW_ADRII_ARM);}  // 00000010010011110000000000000000
            if((inst & 0xE5F0000) == CROW_LDR_ARM_INST)   { return (CROW_LDR_ARM);}    // 00000100000111110000000000000000
            if((inst & 0xFE00FFF) == CROW_MOV_ARM_INST)   { return (CROW_MOV_ARM);}    // 00000001101000000000000000001111
            return CROW_UNDEFINE;
        }
        static void crow_transport_arm_instruction(ARM32_INSTRUCTION target_addr, ARM32_INSTRUCTION* orig_inst, U32 len, ARM32_INSTRUCTION* trampoline_inst, U32* orig_limit, U32* trampoline_limit, U32* count)
        {
            return;
        }
    }
}