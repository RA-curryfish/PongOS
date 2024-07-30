#include "idt.h"
#include "../libf.h"

typedef struct
{
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry;

typedef struct
{
    uint16_t limit;
    idt_entry* ptr;
} __attribute__((packed)) idt_descriptor;

idt_entry idt[256];
idt_descriptor idt_descriptor_val = { sizeof(idt) - 1, idt };
extern void __attribute__((cdecl)) load_idt(idt_descriptor* idt_descriptor_val);

void idt_initialize()
{
    load_idt(&idt_descriptor_val);
}

void idt_disable_gate(int interrupt)
{
    FLAG_UNSET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void idt_enable_gate(int interrupt)
{
    FLAG_SET(idt[interrupt].flags, IDT_FLAG_PRESENT);
}

void idt_set_gate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags)
{
    idt[interrupt].base_low = ((uint32_t)base) & 0xFFFF;
    idt[interrupt].segment_selector = segmentDescriptor;
    idt[interrupt].reserved = 0;
    idt[interrupt].flags = flags;
    idt[interrupt].base_high = ((uint32_t)base >> 16) & 0xFFFF;
}