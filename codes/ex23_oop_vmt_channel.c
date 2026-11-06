/**
 * @file ex23_oop_vmt_channel.c
 * @brief Reference solution for Section 23 Exercise: Virtual Method Table (VMT) & Method Override.
 */
#include <stdio.h>
#include <stdint.h>

typedef struct Channel Channel;

// 1. Virtual Method Table (VMT)
typedef struct {
    void (*send)(Channel *self, uint8_t byte);
} ChannelVTable;

// 2. Base class
struct Channel {
    const ChannelVTable *vptr;
};

void channel_send(Channel *ch, uint8_t byte) {
    if (ch != nullptr && ch->vptr != nullptr && ch->vptr->send != nullptr) {
        ch->vptr->send(ch, byte); // Dynamic dispatch via VMT
    }
}

// Base implementation
static void raw_channel_send_impl(Channel *self, uint8_t byte) {
    (void)self;
    printf("  [RawChannel]       Sent plain byte:     0x%02X\n", byte);
}

static const ChannelVTable RAW_CHANNEL_VTABLE = { .send = raw_channel_send_impl };

void raw_channel_init(Channel *ch) {
    ch->vptr = &RAW_CHANNEL_VTABLE;
}

// 3. Derived class (Inheritance: 'base' MUST be the first member)
typedef struct {
    Channel base;
    uint8_t xor_key;
} EncryptedChannel;

// Overridden method implementation
static void encrypted_channel_send_impl(Channel *self, uint8_t byte) {
    EncryptedChannel *enc = (EncryptedChannel *)self; // Safe downcast (offset 0)
    uint8_t cipher_byte = byte ^ enc->xor_key;
    printf("  [EncryptedChannel] Sent encrypted byte: 0x%02X (key: 0x%02X)\n",
           cipher_byte, enc->xor_key);
}

static const ChannelVTable ENCRYPTED_CHANNEL_VTABLE = { .send = encrypted_channel_send_impl };

void encrypted_channel_init(EncryptedChannel *enc, uint8_t key) {
    enc->base.vptr = &ENCRYPTED_CHANNEL_VTABLE;
    enc->xor_key = key;
}

int main(void) {
    printf("=== Exercise 23: OOP Virtual Method Table (VMT) ===\n\n");

    Channel plain_ch;
    raw_channel_init(&plain_ch);

    EncryptedChannel secure_ch;
    encrypted_channel_init(&secure_ch, 0x5AU);

    // Polymorphic array of base class pointers
    Channel *channels[2] = { &plain_ch, (Channel *)&secure_ch };

    for (size_t i = 0; i < 2U; ++i) {
        channel_send(channels[i], 0x41U); // Send 'A' (0x41) through both channels
    }

    return 0;
}
