/**
 * @file ex14_pid_context_invariants.c
 * @brief Reference solution for Section 14 Exercise: Context Pointer & Invariants (PID Controller).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Encapsulated state (No global variables -> allows multiple independent instances)
typedef struct {
    float kp;
    float ki;
    float integral_sum;
    float out_min;
    float out_max;
    bool is_initialized;
} PidController;

static float clamp_value(float val, float min_val, float max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

// 1. Initialization enforcing the structural invariant: (out_min < out_max)
bool pid_init(PidController *pid, float kp, float ki, float out_min, float out_max) {
    if (pid == nullptr || out_min >= out_max) {
        return false; // Invariant violation rejected
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->integral_sum = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;
    pid->is_initialized = true;
    return true;
}

// 2. State mutation strictly preserving the anti-windup invariant
float pid_compute(PidController *pid, float setpoint, float measurement) {
    if (pid == nullptr || !pid->is_initialized) {
        return 0.0f;
    }

    float error = setpoint - measurement;

    // Accumulate integral and enforce invariant bounds immediately (Anti-windup)
    pid->integral_sum += (pid->ki * error);
    pid->integral_sum = clamp_value(pid->integral_sum, pid->out_min, pid->out_max);

    float output = (pid->kp * error) + pid->integral_sum;
    return clamp_value(output, pid->out_min, pid->out_max);
}

int main(void) {
    printf("=== Exercise 14: Encapsulated PID Controller & Invariants ===\n\n");

    PidController zone1_heater;
    PidController zone2_heater;

    // Verify invariant protection on invalid bounds (min >= max)
    if (!pid_init(&zone1_heater, 2.0f, 0.5f, 100.0f, 0.0f)) {
        printf("  [Invariant Check] Safely rejected invalid bounds (min 100 >= max 0).\n\n");
    }

    // Properly initialize two independent instances
    pid_init(&zone1_heater, 2.0f, 0.5f, 0.0f, 100.0f);
    pid_init(&zone2_heater, 1.5f, 0.2f, 0.0f, 50.0f);

    for (int step = 1; step <= 3; ++step) {
        float pwm1 = pid_compute(&zone1_heater, 80.0f, 25.0f); // High error -> saturates at 100%
        float pwm2 = pid_compute(&zone2_heater, 40.0f, 30.0f); // Moderate error

        printf("  Step %d -> Zone 1 PWM: %5.1f %% (Integral: %5.1f) | Zone 2 PWM: %4.1f %%\n",
               step, pwm1, zone1_heater.integral_sum, pwm2);
    }

    return 0;
}
