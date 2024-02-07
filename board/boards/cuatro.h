// ////////////////////////// //
// Cuatro (STM32H7) + Harness //
// ////////////////////////// //

void cuatro_set_led(uint8_t color, bool enabled) {
  switch (color) {
    case LED_RED:
      set_gpio_output(GPIOD, 15, !enabled);
      break;
     case LED_GREEN:
      set_gpio_output(GPIOD, 14, !enabled);
      break;
    case LED_BLUE:
      set_gpio_output(GPIOE, 2, !enabled);
      break;
    default:
      break;
  }
}

void cuatro_enable_can_transceiver(uint8_t transceiver, bool enabled) {
  if (transceiver == 1U) {
    set_gpio_output(GPIOB, 7, !enabled);
  } else if (transceiver == 3U) {
    set_gpio_output(GPIOD, 8, !enabled);
  } else {
    red_enable_can_transceiver(transceiver, enabled);
  }
}

void cuatro_init(void) {
  red_chiplet_init();

  // CAN FD 0 transceiver enable (rest are done in red init)
  set_gpio_pullup(GPIOB, 7, PULL_NONE);
  set_gpio_mode(GPIOB, 7, MODE_OUTPUT);
  set_gpio_pullup(GPIOD, 8, PULL_NONE);
  set_gpio_mode(GPIOD, 8, MODE_OUTPUT);

  // FDCAN2
  set_gpio_pullup(GPIOD, 12, PULL_NONE);
  set_gpio_alternate(GPIOD, 12, GPIO_AF9_FDCAN2);
  set_gpio_pullup(GPIOD, 13, PULL_NONE);
  set_gpio_alternate(GPIOD, 13, GPIO_AF9_FDCAN2);

  // C2: SOM GPIO used as input (fan control at boot)
  set_gpio_mode(GPIOC, 2, MODE_INPUT);
  set_gpio_pullup(GPIOC, 2, PULL_DOWN);

  // SOM bootkick + reset lines
  set_gpio_mode(GPIOC, 12, MODE_OUTPUT);
  tres_set_bootkick(BOOT_BOOTKICK);

  // SOM debugging UART
  gpio_uart7_init();
  uart_init(&uart_ring_som_debug, 115200);

  // SPI init
  gpio_spi_init();

  // fan setup
  set_gpio_alternate(GPIOC, 8, GPIO_AF2_TIM3);

  // Initialize IR PWM and set to 0%
  set_gpio_alternate(GPIOC, 9, GPIO_AF2_TIM3);
  pwm_init(TIM3, 4);
  tres_set_ir_power(0U);

  // Clock source
  clock_source_init();
}

const board board_cuatro = {
  .harness_config = &red_chiplet_harness_config,
  .has_obd = true,
  .has_spi = true,
  .has_canfd = true,
  .has_rtc_battery = true,
  .fan_max_rpm = 6600U,
  .avdd_mV = 1800U,
  .fan_stall_recovery = false,
  .fan_enable_cooldown_time = 3U,
  .init = cuatro_init,
  .init_bootloader = unused_init_bootloader,
  .enable_can_transceiver = cuatro_enable_can_transceiver,
  .enable_can_transceivers = red_chiplet_enable_can_transceivers,
  .set_led = cuatro_set_led,
  .set_can_mode = red_chiplet_set_can_mode,
  .check_ignition = red_check_ignition,
  .read_current = unused_read_current,
  .set_fan_enabled = tres_set_fan_enabled,
  .set_ir_power = tres_set_ir_power,
  .set_siren = unused_set_siren,
  .set_bootkick = tres_set_bootkick,
  .read_som_gpio = tres_read_som_gpio
};