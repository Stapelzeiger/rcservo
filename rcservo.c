#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "cmp/cmp.h"
#include "rcservo.h"

void rcservo_init(rcservo_t *s)
{
    s->calib.gain_pos = 5000;
    s->calib.gain_neg = 5000;
    s->calib.zero =    15000;
    s->calib.min =      9000;
    s->calib.max =     21000;
    s->calib.update_period = 200;
    s->enable_output = false;
    s->pos = s->calib.zero;
}

void rcservo_enable(rcservo_t *s)
{
    s->enable_output = true;
}

void rcservo_disable(rcservo_t *s)
{
    s->enable_output = false;
}

static bool cmp_read(struct cmp_ctx_s *ctx, void *data, size_t limit)
{
    const char **readp = (const char**)ctx->buf;
    memcpy(data, *readp, limit);
    *readp += limit;
    return true;
}

const void *rcservo_load_calibration(rcservo_t *s, const void *calib)
{
    cmp_ctx_t cmp;
    const void *readp = calib;
    cmp_init(&cmp, &readp, cmp_read, NULL);
    bool err = false;
    uint32_t array_size;
    err = err || !cmp_read_array(&cmp, &array_size);
    err = err || (array_size != 6);
    err = err || !cmp_read_u16(&cmp, &s->calib.gain_pos);
    err = err || !cmp_read_u16(&cmp, &s->calib.gain_neg);
    err = err || !cmp_read_u16(&cmp, &s->calib.zero);
    err = err || !cmp_read_u16(&cmp, &s->calib.min);
    err = err || !cmp_read_u16(&cmp, &s->calib.max);
    err = err || !cmp_read_u8(&cmp, &s->calib.update_period);
    if (err || (char *)calib + RC_SERVO_CALIBRATION_BUFFER_SIZE != readp) {
        return NULL;
    }
    return readp;
}

static size_t cmp_write(struct cmp_ctx_s *ctx, const void *data, size_t count)
{
    char **writep = (char**)ctx->buf;
    memcpy(*writep, data, count);
    *writep += count;
    return count;
}

void *rcservo_save_calibration(const rcservo_t *s, void *calib)
{
    cmp_ctx_t cmp;
    void *writep = calib;
    cmp_init(&cmp, &writep, NULL, cmp_write);
    bool err = false;
    err = err || !cmp_write_array(&cmp, 6);
    err = err || !cmp_write_u16(&cmp, s->calib.gain_pos);
    err = err || !cmp_write_u16(&cmp, s->calib.gain_neg);
    err = err || !cmp_write_u16(&cmp, s->calib.zero);
    err = err || !cmp_write_u16(&cmp, s->calib.min);
    err = err || !cmp_write_u16(&cmp, s->calib.max);
    err = err || !cmp_write_u8(&cmp, s->calib.update_period);
    if (err || (char *)calib + RC_SERVO_CALIBRATION_BUFFER_SIZE != writep) {
        return NULL;
    }
    return writep;
}

void rcservo_set(rcservo_t *s, float pos)
{
    float pulse = s->calib.zero;
    if (isnan(pos)) {
        pos = 0;
    }
    if (pos > 0) {
        pulse += pos * s->calib.gain_pos;
    } else {
        pulse += pos * s->calib.gain_neg;
    }
    if (pulse > s->calib.max) {
        pulse = s->calib.max;
    }
    if (pulse < s->calib.min) {
        pulse = s->calib.min;
    }
    s->pos = pulse;
}

void rcservo_set_raw(rcservo_t *s, uint16_t pwm)
{
    s->pos = pwm;
}

uint16_t rcservo_get_pwm(const rcservo_t *s)
{
    return s->pos;
}

uint32_t rcservo_get_update_period(const rcservo_t *s)
{
    return (uint32_t)s->calib.update_period * 1000;
}
