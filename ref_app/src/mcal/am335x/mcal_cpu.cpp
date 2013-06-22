///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2007 - 2013.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <mcal_cpu.h>
#include <mcal_osc.h>
#include <mcal_port.h>
#include <mcal_wdg.h>
#include <am335x_hw_regs.h>

void mcal::cpu::init()
{
  mcal::port::init(nullptr);
  mcal::wdg::init(nullptr);
  mcal::osc::init(nullptr);
}

void mcal::cpu::switch_to_privileged_mode()
{
  // This API can be used to switch from user mode to privileged mode
  // The priviledge mode will be system mode. System mode will share 
  // the same resources as user mode, but with privileges.
  asm volatile("SWI 458752");
}

void mcal::cpu::switch_to_user_mode()
{
  // This API can be used to switch from any previleged mode of ARM to 
  // user mode. After this API is called, the program will continue
  // to operate in non-privileged mode, until any exception occurs.
  // After the exception is serviced, execution will continue in user
  // mode.
  asm volatile("    mrs     r0, CPSR\n\t"
               "    bic     r0, #0x0F\n\t"
               "    orr     r0, #0x10\n\t "
               "    msr     CPSR, r0");
}

std::uint32_t mcal::cpu::read_dfsr()
{
  std::uint32_t stat;

  // IRQ and FIQ in CPSR
  asm volatile("mrc p15, #0, %[result], c5, c0, #0\n\t" : [result] "=r" (stat));

  return stat;
}

std::uint32_t mcal::cpu::read_dfar()
{
  std::uint32_t stat;

  // IRQ and FIQ in CPSR
  asm volatile("mrc p15, #0, %[result], c6, c0, #0\n\t" : [result] "=r" (stat));

  return stat;
}

extern "C" void CPUAbortHandler()
{
  // This API is called when the CPU is aborted or during execution
  // of any undefined instruction. Both IRQ and FIQ will be disabled
  // when the CPU gets an abort and calls this API. 
  const std::uint32_t fault_type = mcal::cpu::read_dfsr();

  GPIO1->CLEARDATAOUT = 0x01E00000u;
  GPIO1->SETDATAOUT   = (fault_type & 0x0FUL) << 21U;
}

std::uint32_t mcal::cpu::int_status()
{
  // Wrapper function for the IRQ status
  volatile std::uint32_t stat;

  // IRQ and FIQ in CPSR
  asm volatile("mrs  r0, CPSR\n\t"
               "and  %[result], r0, #0xC0" : [result] "=r" (stat));

  return stat;
}

void mcal::cpu::irqd()
{
  // Wrapper function for the IRQ disable function

  // Disable IRQ in CPSR
  asm volatile("mrs  r0, CPSR\n\t"
               "orr  r0, #0x80\n\t"
               "msr  CPSR, r0");
}

void mcal::cpu::irqe()
{
  // Wrapper function for the IRQ enable function

  // Enable IRQ in CPSR
  asm volatile("mrs  r0, CPSR\n\t"
               "bic  r0, #0x80\n\t"
               "msr  CPSR, r0");
}

void mcal::cpu::fiqd()
{
  // Wrapper function for the FIQ disable function

  // Disable FIQ in CPSR
  asm volatile("mrs  r0, CPSR\n\t"
               "orr  r0, #0x40\n\t"
               "msr  CPSR, r0");
}

void mcal::cpu::fiqe()
{
  // Wrapper function for the FIQ enable function

  // Enable FIQ in CPSR
  asm volatile("mrs  r0, CPSR\n\t"
               "bic  r0, #0x40\n\t"
               "msr  CPSR, r0");
}