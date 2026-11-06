/**
 * @file ex20_tagged_union_commands.c
 * @brief Reference solution for Section 20 Exercise: Tagged Union Command Dispatcher.
 */
#include <stdio.h>
#include <stdint.h>

// 1. C23 underlying type specification for compact 1-byte tag
typedef enum : uint8_t {
    CMD_MOVE,
    CMD_SET_TEMP,
    CMD_EMERGENCY_STOP
} CommandType;

// 2. Tagged Union with C11/C23 anonymous union
typedef struct {
    CommandType type;
    union {
        struct {
            int16_t x;
            int16_t y;
        } move;
        float target_temp;
        uint32_t stop_reason;
    };
} ActuatorCommand;

// 3. Exhaustive switch dispatch
void execute_command(const ActuatorCommand *cmd) {
    if (cmd == nullptr) return;

    switch (cmd->type) {
        case CMD_MOVE:
            printf("  [Action] Moving actuator to X=%d, Y=%d\n", cmd->move.x, cmd->move.y);
            break;
        case CMD_SET_TEMP:
            printf("  [Action] Setting target temperature to %.1f C\n", cmd->target_temp);
            break;
        case CMD_EMERGENCY_STOP:
            printf("  [ALERT]  Emergency STOP! Reason code: 0x%08X\n", cmd->stop_reason);
            break;
    }
}

int main(void) {
    printf("=== Exercise 20: Tagged Union Command Dispatcher ===\n\n");
    printf("Total sizeof(ActuatorCommand): %zu bytes (Union shares payload space!)\n\n",
           sizeof(ActuatorCommand));

    ActuatorCommand queue[3] = {
            { .type = CMD_MOVE,           .move = { .x = 120, .y = -45 } },
            { .type = CMD_SET_TEMP,       .target_temp = 65.5f },
            { .type = CMD_EMERGENCY_STOP, .stop_reason = 0xDEAD0001U }
    };

    for (size_t i = 0; i < 3U; ++i) {
        execute_command(&queue[i]);
    }

    return 0;
}
