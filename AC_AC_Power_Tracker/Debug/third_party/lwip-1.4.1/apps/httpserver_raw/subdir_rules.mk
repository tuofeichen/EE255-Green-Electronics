################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
third_party/lwip-1.4.1/apps/httpserver_raw/httpd.obj: /Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party/lwip-1.4.1/apps/httpserver_raw/httpd.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/chentuofei/workspace_v7/AC_AC_Power_Tracker" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/examples/boards/ek-tm4c1294xl" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party/lwip-1.4.1/src/include" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party/lwip-1.4.1/src/include/ipv4" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party/lwip-1.4.1/apps" --include_path="/Users/chentuofei/ti/TivaWare_C_Series-2.1.3.156/third_party" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=EK_TM4C129_BP1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="third_party/lwip-1.4.1/apps/httpserver_raw/httpd.d_raw" --obj_directory="third_party/lwip-1.4.1/apps/httpserver_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


