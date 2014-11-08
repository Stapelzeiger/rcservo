#ifndef RC_SERVO_H
#define RC_SERVO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    struct {
        uint16_t gain_pos;      /* [100ns] gain for input pos > 0 */
        uint16_t gain_neg;      /* [100ns] gain for input pos < 0 */
        uint16_t zero;          /* [100ns] zero input pulse width */
        uint16_t min;           /* [100ns] mechanical min pulse width */
        uint16_t max;           /* [100ns] mechanical max pulse width */
        uint8_t update_period;  /* [100us] minimum interval between pulses */
    } calib;
    uint16_t pos;               /* [100ns] current pulse width */
    bool enable_output;         /* pulse output enabled */
} rcservo_t;

/*
 * the servo calibration is stored in a message pack array:
 * [u16:gain_pos, u16:gain_neg, u16:zero, u16:min, u16:max, u8:update_period]
 * buffer size: array(size 6):1 + elements: (id:1+u16:2)*5 + (id:1+u8:1) = 18
 */
#define RC_SERVO_CALIBRATION_BUFFER_SIZE 18

#ifdef __cplusplus
extern "C" {
#endif

/* initialize servo to default configuration
 * [-1, 1] full scale maps to [1ms, 2ms], zero at 1.5ms, limits: [0.9ms, 2.1ms]
 * update period: 20ms, position at 0 (1.5ms), output disabled */
void rcservo_init(rcservo_t *s);
/* enable servo output */
void rcservo_enable(rcservo_t *s);
/* disable servo output */
void rcservo_disable(rcservo_t *s);
/* load message pack serialized configuration */
const void *rcservo_load_calibration(rcservo_t *s, const void *calib);
/* save message pack serialized configuration */
void *rcservo_save_calibration(const rcservo_t *s, void *calib);
/* set servo to position, full scale is about [-1, 1] */
void rcservo_set(rcservo_t *s, float pos);
/* set raw pulse length in 100ns resolution. min/max limit don't apply! */
void rcservo_set_raw(rcservo_t *s, uint16_t pwm);
/* get pulse length in 100ns resolution */
uint16_t rcservo_get_pwm(const rcservo_t *s);
/* get update period in 100ns resolution */
uint32_t rcservo_get_update_period(const rcservo_t *s);

#ifdef __cplusplus
}
#endif

#endif /* RC_SERVO_H */
