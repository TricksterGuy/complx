#ifndef MEMORY_VIEW_CONTAINER_HPP
#define MEMORY_VIEW_CONTAINER_HPP

#include "MemoryGrid.hpp"
#include "MemoryView.hpp"
#include <lc3.hpp>

enum ViewMode
{
  SHOW_ALL,
  SHOW_MODIFIED,
  SHOW_NONZERO,
};

void OnGoto(MemoryGrid* memory);
void OnUpdateHideAddresses(MemoryGrid* memory, MemoryView* memoryView, int mode);
void OnHideAddressesCustom(MemoryGrid* memory, MemoryView* memoryView);
void OnDumbDisassemble(MemoryGrid* memory);
void OnNormalDisassemble(MemoryGrid* memory);
void OnCDisassemble(MemoryGrid* memory);
void OnInstructionHighlight(MemoryGrid* memory, bool set);
void OnFlipMemory(MemoryGrid* memory, bool set);

#endif
