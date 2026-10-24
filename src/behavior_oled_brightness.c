#define DT_DRV_COMPAT zmk_behavior_oled_brightness

#include <errno.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/display.h>

LOG_MODULE_REGISTER(zmk_behavior_oled_brightness, CONFIG_ZMK_LOG_LEVEL);

#define OLED_BRIGHTNESS_DEFAULT 128
#define OLED_BRIGHTNESS_STEP 32

enum oled_brightness_direction {
    OLED_BRIGHTNESS_DOWN = 0,
    OLED_BRIGHTNESS_UP = 1,
};

static uint8_t oled_brightness = OLED_BRIGHTNESS_DEFAULT;
K_MUTEX_DEFINE(oled_brightness_mutex);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY) && DT_HAS_CHOSEN(zephyr_display)
#define OLED_BRIGHTNESS_HAS_DISPLAY 1
static const struct device *const oled_display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
#else
#define OLED_BRIGHTNESS_HAS_DISPLAY 0
#endif

static int apply_oled_brightness(void) {
#if OLED_BRIGHTNESS_HAS_DISPLAY
    if (!device_is_ready(oled_display)) {
        return -ENODEV;
    }

    k_mutex_lock(&oled_brightness_mutex, K_FOREVER);
    uint8_t value = oled_brightness;
    k_mutex_unlock(&oled_brightness_mutex);

    int err = display_set_contrast(oled_display, value);
    if (err) {
        LOG_WRN("Could not set OLED brightness to %u (%d)", value, err);
    }
    return err;
#else
    return -ENODEV;
#endif
}

static void apply_oled_brightness_work_cb(struct k_work *work) {
    ARG_UNUSED(work);
    (void)apply_oled_brightness();
}

K_WORK_DEFINE(apply_oled_brightness_work, apply_oled_brightness_work_cb);

#if IS_ENABLED(CONFIG_SETTINGS)
static int oled_brightness_settings_set(const char *name, size_t len,
                                        settings_read_cb read_cb, void *cb_arg) {
    if (strcmp(name, "level") != 0) {
        return -ENOENT;
    }
    if (len != sizeof(oled_brightness)) {
        return -EINVAL;
    }

    uint8_t saved_brightness;
    int err = read_cb(cb_arg, &saved_brightness, sizeof(saved_brightness));
    if (err < 0) {
        return err;
    }

    k_mutex_lock(&oled_brightness_mutex, K_FOREVER);
    oled_brightness = saved_brightness;
    k_mutex_unlock(&oled_brightness_mutex);

    /* The SSD1306 device is initialized before Zephyr loads persistent settings. */
    (void)apply_oled_brightness();
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(oled_brightness, "oled_brightness", NULL,
                               oled_brightness_settings_set, NULL, NULL);
#endif

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata oled_brightness_values[] = {
    {
        .display_name = "Dim OLED",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = OLED_BRIGHTNESS_DOWN,
    },
    {
        .display_name = "Brighten OLED",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = OLED_BRIGHTNESS_UP,
    },
};

static const struct behavior_parameter_metadata_set oled_brightness_param_set = {
    .param1_values = oled_brightness_values,
    .param1_values_len = ARRAY_SIZE(oled_brightness_values),
};

static const struct behavior_parameter_metadata oled_brightness_metadata = {
    .sets_len = 1,
    .sets = &oled_brightness_param_set,
};
#endif

static int oled_brightness_pressed(struct zmk_behavior_binding *binding,
                                   struct zmk_behavior_binding_event event) {
    ARG_UNUSED(event);

    if (binding->param1 > OLED_BRIGHTNESS_UP) {
        return -EINVAL;
    }

    k_mutex_lock(&oled_brightness_mutex, K_FOREVER);
    int next = oled_brightness;
    if (binding->param1 == OLED_BRIGHTNESS_UP) {
        next = MIN(next + OLED_BRIGHTNESS_STEP, UINT8_MAX);
    } else {
        next = MAX(next - OLED_BRIGHTNESS_STEP, 0);
    }
    bool changed = next != oled_brightness;
    oled_brightness = next;
    uint8_t saved_brightness = oled_brightness;
    k_mutex_unlock(&oled_brightness_mutex);

    if (!changed) {
        return ZMK_BEHAVIOR_OPAQUE;
    }

#if IS_ENABLED(CONFIG_SETTINGS)
    int err = settings_save_one("oled_brightness/level", &saved_brightness,
                                sizeof(saved_brightness));
    if (err) {
        LOG_WRN("Could not save OLED brightness (%d)", err);
    }
#else
    ARG_UNUSED(saved_brightness);
#endif

    if (zmk_display_is_initialized()) {
        k_work_submit_to_queue(zmk_display_work_q(), &apply_oled_brightness_work);
    } else {
        (void)apply_oled_brightness();
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int oled_brightness_released(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api oled_brightness_driver_api = {
    .binding_pressed = oled_brightness_pressed,
    .binding_released = oled_brightness_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &oled_brightness_metadata,
#endif
};

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                        POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &oled_brightness_driver_api);
#endif
