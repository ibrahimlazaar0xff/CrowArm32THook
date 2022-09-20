#pragma once

#include "../includes/crow_types.hpp"

namespace crow
{
    /** ~ ALIGNING THE PROGRAM COUNTER ~ **/
    #define CROW_ALIGN_PC(PC)	(PC & 0xFFFFFFFC)

    typedef U16 THUMB16_INSTRUCTION;
    typedef U32 THUMB32_INSTRUCTION;
    typedef U32 ARM32_INSTRUCTION;

    /** ~ TRANSPORT THUMB INSTRUCTION ~ **/
    namespace thumb_transport
    {
        static void crow_transport_thumb_instruction(ARM32_INSTRUCTION target_addr, THUMB16_INSTRUCTION* orig_instr, U32 len, THUMB16_INSTRUCTION *trampoline_inst, U32* orig_limit, U32* trampoline_limit, U32* count);
    } // namespace thumb_transport
    
    /** ~ TRANSPORT ARM INSTRUCTION ~ **/
    namespace arm32_transport
    {
        static void crow_transport_arm_instruction(ARM32_INSTRUCTION target_addr, ARM32_INSTRUCTION* orig_inst, U32 len, ARM32_INSTRUCTION* trampoline_inst, U32* orig_limit, U32* trampoline_limit, U32* count);
    } // namespace arm_transport

    enum crow_armv7_inst_types
    {
        /** ARMV7 THUMB 16 INSTRUCTION TYPES ~ **/

        CROW_BI_THUMB16,    /** ~ B <label> ~ **/
        CROW_BII_THUMB16,   /** ~ B <label> ~ **/
        CROW_BX_THUMB16,    /** ~ BX PC ~ **/
        CROW_ADD_THUMB16,   /** ~ ADD <Rdn>, PC (Rd != PC, Rn != PC) ~**/
        CROW_MOV_THUMB16,   /** ~ MOV Rd, PC ~ **/
        CROW_ADR_THUMB16,   /** ~ ADR Rd, <label> ~ **/
        CROW_LDR_THUMB16,   /** ~ LDR Rt, <label> ~**/
        CROW_CB_THUMB16,    /** ~ CB{N}Z <Rn>, <label> ~**/

        /** ARMV7 THUMB 32 INSTRUCTION TYPES ~ **/

        CROW_BLX_THUMB32,   /** ~ BLX <label> ~**/
        CROW_BL_THUMB32,    /** ~ BL <label> ~**/
        CROW_BI_THUMB32,    /** ~ B.W <label> ~**/
        CROW_BII_THUMB32,   /** ~ B.W <label> ~**/
        CROW_ADRI_THUMB32,  /** ~ ADR.W Rd, <label> ~**/
        CROW_ADRII_THUMB32, /** ~ ADR.W Rd, <label> ~**/
        CROW_LDR_THUMB32,   /** ~ LDR.W Rt, <label> ~**/
        CROW_TBB_THUMB32,   /** ~ TBB [PC, Rm] ~**/
        CROW_TBH_THUMB32,   /** ~ TBH [PC, Rm, LSL #1] ~**/

        /** ARMV7 ARM 32 INSTRUCTION TYPES ~ **/

        CROW_BLX_ARM,       /** ~ BLX <label> ~**/
        CROW_BL_ARM,        /** ~ BL <label> ~**/
        CROW_B_ARM,         /** ~ B <label> ~**/
        CROW_BX_ARM,        /** ~ BX PC ~**/
        CROW_ADD_ARM,       /** ~ ADD Rd, PC, Rm (Rd != PC, Rm != PC) ~**/
        CROW_ADRI_ARM,      /** ~ ADR Rd, <label> ~**/
        CROW_ADRII_ARM,     /** ~ ADR Rd, <label> ~**/
        CROW_MOV_ARM,       /** ~ MOV Rd, PC ~**/
        CROW_LDR_ARM,       /** ~ LDR Rt, <label> ~**/
        /** UNDEFINE INSTRUCTION TYPES ~ **/
        CROW_UNDEFINE,      /** ~ UDF INST ~**/
    };

    enum crow_armv7_instructions
    {
        /** ARMV7 THUMB 16 INSTRUCTIONS ~ **/

        CROW_BI_THUMB16_INST    = 0xD000,    
        CROW_BII_THUMB16_INST   = 0xE000,   
        CROW_BX_THUMB16_INST    = 0x4778,    
        CROW_ADD_THUMB16_INST   = 0x4478,   
        CROW_MOV_THUMB16_INST   = 0x4678,   
        CROW_ADR_THUMB16_INST   = 0xA000,   
        CROW_LDR_THUMB16_INST   = 0x4800,   
        CROW_CB_THUMB16_INST    = 0xB100,    

        /** ARMV7 THUMB 32 INSTRUCTIONS ~ **/

        CROW_BLX_THUMB32_INST   = 0xF000C000,   
        CROW_BL_THUMB32_INST    = 0xF000D000,    
        CROW_BI_THUMB32_INST    = 0xF0008000,    
        CROW_BII_THUMB32_INST   = 0xF0009000,   
        CROW_ADRI_THUMB32_INST  = 0xF2AF0000,  
        CROW_ADRII_THUMB32_INST = 0xF20F0000, 
        CROW_LDR_THUMB32_INST   = 0xF85F0000,   
        CROW_TBB_THUMB32_INST   = 0xE8DF0000,   
        CROW_TBH_THUMB32_INST   = 0xE8DF0010,   

        /** ARMV7 ARM 32 INSTRUCTIONS ~ **/

        CROW_BLX_ARM_INST       = 0xFA000000,       
        CROW_BL_ARM_INST        = 0xB000000,        
        CROW_B_ARM_INST         = 0xA000000,         
        CROW_BX_ARM_INST        = 0x120001F,        
        CROW_ADD_ARM_INST       = 0x8F0000,       
        CROW_ADRI_ARM_INST      = 0x28F0000,      
        CROW_ADRII_ARM_INST     = 0x24F0000,   
        CROW_LDR_ARM_INST       = 0x41F0000,   
        CROW_MOV_ARM_INST       = 0x1A0000F,       
      

        CROW_UNDEFINE_INST      = 0xF3B08000,   
    };
    

    void crow_transport_instruction(THUMB32_INSTRUCTION target_addr, VOIDPTR orig_inst, U32 len, VOIDPTR trampoline_inst, U32* orig_limit, U32* trampoline_limit, U32* count)
    {
        if (target_addr & 1 == 0x01) 
        {
            thumb_transport::crow_transport_thumb_instruction(target_addr - 0x01, (THUMB16_INSTRUCTION*) orig_inst, len, (THUMB16_INSTRUCTION*) trampoline_inst, orig_limit, trampoline_limit, count);
        }
        else {
            arm32_transport::crow_transport_arm_instruction(target_addr, (ARM32_INSTRUCTION*) orig_inst, len, (ARM32_INSTRUCTION*) trampoline_inst, orig_limit, trampoline_limit, count);
        }
    }
} // namespace crow
