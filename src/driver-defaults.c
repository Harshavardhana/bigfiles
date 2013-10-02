#include "bigobjects/driver.h"

int32_t default_put (driver_t *this)
{
        int32_t ret = 0;

        if (!this)
                ret = -1;

        return ret;
}

int32_t default_get (driver_t *this)
{
        int32_t ret = 0;

        if (!this)
                ret = -1;

        return ret;
}

int32_t default_delete (driver_t *this)
{
        int32_t ret = 0;

        if (!this)
                ret = -1;

        return ret;
}
