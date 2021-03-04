################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/ebad/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs" --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs/Debug" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/nortos" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/nortos/posix" --include_path="/home/ebad/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=none --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=BOARD_DISPLAY_USE_LCD=1 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1912234047: ../vlconf.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/home/ebad/ti/ccs1020/ccs/utils/sysconfig_1.7.0/sysconfig_cli.sh" -s "/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-1912234047 ../vlconf.syscfg
syscfg/ti_drivers_config.h: build-1912234047
syscfg/syscfg_c.rov.xs: build-1912234047
syscfg/: build-1912234047

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/ebad/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs" --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs/Debug" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/source" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/nortos" --include_path="/home/ebad/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/nortos/posix" --include_path="/home/ebad/ti/ccs1020/ccs/tools/compiler/ti-cgt-arm_20.2.2.LTS/include" --advice:power=none --define=BOARD_DISPLAY_USE_UART_ANSI=1 --define=BOARD_DISPLAY_USE_LCD=1 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="/home/ebad/vl6180x_MSP_EXP432P401R_nortos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


