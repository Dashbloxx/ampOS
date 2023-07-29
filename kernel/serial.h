#pragma once

typedef enum
{
    COM1 = 0x3F8,
    COM2 = 0x2F8
} serial_port_t;

int serial_initialize(serial_port_t serial_port);
int serial_received(serial_port_t serial_port);
char read_serial(serial_port_t serial_port);
int is_transmit_empty(serial_port_t serial_port);
void write_serial(serial_port_t serial_port, char character);

static void com1_write(char character)
{
    write_serial(COM1, character);
}

static void com1_initialize()
{
    serial_initialize(COM1);
}