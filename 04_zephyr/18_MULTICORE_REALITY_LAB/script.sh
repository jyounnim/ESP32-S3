# Source: 18_MULTICORE_REALITY_LAB.md
# Section: 실제로 두 코어를 함께 써보고 싶다면

west build -b esp32s3_devkitc/esp32s3/procpu --sysbuild samples/drivers/ipm/ipm_esp32
west flash
west espressif monitor
