#define DT_DRV_COMPAT zmk_behavior_base_view_toggle

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>

#if IS_ENABLED(CONFIG_ZMK_DISPLAY)
#include <hana_base_view.h>
#endif

static int on_base_view_toggle_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
#if IS_ENABLED(CONFIG_ZMK_DISPLAY)
    hana_base_view_cycle();
#endif
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_base_view_toggle_released(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_base_view_toggle_driver_api = {
    .binding_pressed = on_base_view_toggle_pressed,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
    .binding_released = on_base_view_toggle_released,
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
};

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                        POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_base_view_toggle_driver_api);
#endif
