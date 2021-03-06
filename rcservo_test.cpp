#include "CppUTest/TestHarness.h"
#include <math.h>
#include "rcservo.h"


static const char defaultcofing[] = "\x96\xcd\x13\x88\xcd\x13\x88\xcd\x3a\x98\xcd\x23\x28\xcd\x52\x08\xcc\xc8";

TEST_GROUP(RCServoTest)
{
    rcservo_t s;

    void init_zero(rcservo_t *s)
    {
        s->calib.gain_pos = 0;
        s->calib.gain_neg = 0;
        s->calib.zero = 0;
        s->calib.min = 0;
        s->calib.max = 0;
        s->calib.update_period = 0;
        s->enable_output = true;
        s->pos = 0;
    }

    void setup(void)
    {
        rcservo_init(&s);
    }
};


TEST(RCServoTest, Init)
{
    init_zero(&s);
    rcservo_init(&s);
    CHECK_EQUAL(5000, s.calib.gain_pos);
    CHECK_EQUAL(5000, s.calib.gain_neg);
    CHECK_EQUAL(15000, s.calib.zero);
    CHECK_EQUAL(9000, s.calib.min);
    CHECK_EQUAL(21000, s.calib.max);
    CHECK_EQUAL(200, s.calib.update_period);
    CHECK_EQUAL(false, s.enable_output);
    CHECK_EQUAL(15000, s.pos);
}

TEST(RCServoTest, Enable)
{
    rcservo_enable(&s);
    CHECK_EQUAL(true, s.enable_output);
}

TEST(RCServoTest, Disable)
{
    rcservo_enable(&s);
    rcservo_disable(&s);
    CHECK_EQUAL(false, s.enable_output);
}

TEST(RCServoTest, Load)
{
    init_zero(&s);
    POINTERS_EQUAL(&defaultcofing[strlen(defaultcofing)], rcservo_load_calibration(&s, defaultcofing));
    CHECK_EQUAL(5000, s.calib.gain_pos);
    CHECK_EQUAL(5000, s.calib.gain_neg);
    CHECK_EQUAL(15000, s.calib.zero);
    CHECK_EQUAL(9000, s.calib.min);
    CHECK_EQUAL(21000, s.calib.max);
    CHECK_EQUAL(200, s.calib.update_period);
}

TEST(RCServoTest, LoadErrorType)
{
    static char errorconfig[] = "\x96\xcd\x13\x88\xcd\x13\x88\xcd\x3a\x98\x09\xcd\x52\x08\xcc\xc8";
    POINTERS_EQUAL(NULL, rcservo_load_calibration(&s, errorconfig));
}

TEST(RCServoTest, LoadErrorArrayLen)
{
    static char errorconfig[] = "\x95\xcd\x13\x88\xcd\x13\x88\xcd\x3a\x98\xcd\x23\x28\xcd\x52\x08\xcc\xc8";
    POINTERS_EQUAL(NULL, rcservo_load_calibration(&s, errorconfig));
}

TEST(RCServoTest, Store)
{
    char buf[RC_SERVO_CALIBRATION_BUFFER_SIZE];
    POINTERS_EQUAL(&buf[strlen(defaultcofing)], rcservo_save_calibration(&s, buf));
    CHECK(memcmp(buf, defaultcofing, strlen(defaultcofing)) == 0);
}

TEST(RCServoTest, SetRaw)
{
    rcservo_set_raw(&s, 10000);
    CHECK_EQUAL(10000, s.pos);
    rcservo_set_raw(&s, 30000);
    CHECK_EQUAL(30000, s.pos);
    rcservo_set_raw(&s, 1000);
    CHECK_EQUAL(1000, s.pos);
}

TEST(RCServoTest, GetPWM)
{
    rcservo_set_raw(&s, 13000);
    CHECK_EQUAL(13000, rcservo_get_pwm(&s));
}

TEST(RCServoTest, GetUpdatePeriod)
{
    CHECK_EQUAL(200000, rcservo_get_update_period(&s));
}

// default config set tests
TEST_GROUP(RCServoDefaultSetTest)
{
    rcservo_t s;

    void setup(void)
    {
        rcservo_init(&s);
    }
};

TEST(RCServoDefaultSetTest, SetZero)
{
    rcservo_set(&s, 0);
    DOUBLES_EQUAL(15000, s.pos, 1);
}

TEST(RCServoDefaultSetTest, SetMax)
{
    rcservo_set(&s, 1);
    DOUBLES_EQUAL(20000, s.pos, 1);
}

TEST(RCServoDefaultSetTest, SetHalf)
{
    rcservo_set(&s, 0.5);
    DOUBLES_EQUAL(17500, s.pos, 1);
}

TEST(RCServoDefaultSetTest, SetMin)
{
    rcservo_set(&s, -1);
    DOUBLES_EQUAL(10000, s.pos, 1);
}

TEST(RCServoDefaultSetTest, SetLimitMax)
{
    rcservo_set(&s, 2);
    DOUBLES_EQUAL(21000, s.pos, 1);
}

TEST(RCServoDefaultSetTest, SetLimitMin)
{
    rcservo_set(&s, -2);
    DOUBLES_EQUAL(9000, s.pos, 1);
}

// custom config set tests
TEST_GROUP(RCServoCustomSetTest)
{
    rcservo_t s;

    void setup(void)
    {
        rcservo_init(&s);
        s.calib.gain_pos = 1000;
        s.calib.gain_neg = 1000;
        s.calib.zero = 13000;
        s.calib.min = 10000;
        s.calib.max = 20000;
        s.calib.update_period = 100;
    }
};

TEST(RCServoCustomSetTest, SetZero)
{
    rcservo_set(&s, 0);
    DOUBLES_EQUAL(13000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetMax)
{
    rcservo_set(&s, 1);
    DOUBLES_EQUAL(14000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetHalf)
{
    rcservo_set(&s, 0.5);
    DOUBLES_EQUAL(13500, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetMin)
{
    rcservo_set(&s, -1);
    DOUBLES_EQUAL(12000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetLimitMax)
{
    rcservo_set(&s, 10);
    DOUBLES_EQUAL(20000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetLimitMin)
{
    rcservo_set(&s, -10);
    DOUBLES_EQUAL(10000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetNan)
{
    rcservo_set(&s, NAN);
    DOUBLES_EQUAL(13000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetInf)
{
    rcservo_set(&s, INFINITY);
    DOUBLES_EQUAL(20000, s.pos, 1);
}

TEST(RCServoCustomSetTest, SetNegInf)
{
    rcservo_set(&s, -INFINITY);
    DOUBLES_EQUAL(10000, s.pos, 1);
}
