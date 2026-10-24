if(CONFIG_LAYOUT_SHIFT)
  if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources_ifdef(CONFIG_LAYOUT_SHIFT app PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/layout_shift/src/layout_shift_map.c
      ${CMAKE_CURRENT_LIST_DIR}/layout_shift/src/behavior_layout_shift_key_press.c
      ${CMAKE_CURRENT_LIST_DIR}/layout_shift/src/behavior_layout_shift_toggle.c
    )
  endif()
endif()

# Manual OLED on/off behavior. Compiles as a harmless no-op on non-display builds.
target_sources(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/behavior_oled_toggle.c
  ${CMAKE_CURRENT_LIST_DIR}/src/behavior_oled_brightness.c
)

# Runtime OLED and split-half sleep timeout controls.
target_sources(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/behavior_timeout_cycle.c
)


# Persistent BASE display mode toggle (Bongo Cat/WPM <-> bordered BASE label).
target_sources(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/behavior_base_view_toggle.c
)
