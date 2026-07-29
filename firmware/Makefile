BUILD_TYPE ?= Release
FILENAME ?= nmea_sim.elf

.PHONY: build tests flash flash-debug debug lsp

clean:
	rm -rf bin build

build:
	cmake --preset $(BUILD_TYPE)
	cmake --build --preset $(BUILD_TYPE)

tests:
	cmake --preset Tests
	cmake --build --preset Tests
	ctest --test-dir build/Tests --output-on-failure

flash:
	openocd -f interface/stlink.cfg -c "transport select hla_swd" -f target/stm32f4x.cfg -c "program bin/${BUILD_TYPE}/${FILENAME} verify reset exit"

flash-debug:
	openocd -f interface/stlink.cfg -c "transport select hla_swd" -f target/stm32f4x.cfg -c "program bin/Debug/${FILENAME} verify; reset halt"

debug:
	arm-none-eabi-gdb -ex "target remote :3333" bin/Debug/${FILENAME}

lsp:
	ln -sf build/$(BUILD_TYPE)/compile_commands.json compile_commands.json
