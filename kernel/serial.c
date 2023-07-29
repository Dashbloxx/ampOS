#include <kernel/serial.h>
#include <kernel/arch/i386/ports.h>

int serial_initialize(serial_port_t serial_port)
{
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x80);
    outb(serial_port + 0, 0x03);
    outb(serial_port + 1, 0x00);
    outb(serial_port + 3, 0x03);
    outb(serial_port + 2, 0xC7);
    outb(serial_port + 4, 0x0B);
    outb(serial_port + 4, 0x1E);
    outb(serial_port + 0, 0xAE);

    if(inb(serial_port + 0) != 0xAE)
    {
        return 1;
    }

    outb(serial_port + 4, 0x0F);
    return 0;
}

int serial_received(serial_port_t serial_port)
{
    return inb(serial_port + 5) & 1;
}
 
char read_serial(serial_port_t serial_port)
{
    while (serial_received(serial_port) == 0);

    return inb(serial_port);
}

int is_transmit_empty(serial_port_t serial_port)
{
    return inb(serial_port + 5) & 0x20;
}
 
void write_serial(serial_port_t serial_port, char character)
{
    while (is_transmit_empty(serial_port) == 0);

    outb(serial_port, character);
}