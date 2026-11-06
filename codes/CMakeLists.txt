cmake_minimum_required(VERSION 3.24)
project(Pokrocile_C_Skoleni C)

# Vynuceni moderniho standardu C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Pridani striktnich varovani kompilatoru
add_compile_options(-Wall -Wextra -pedantic)

# ==============================================================================
# SEKCE 01: Moznosti programovaciho jazyka C
# ==============================================================================
add_executable(m01_s01_c23_features m01_s01_c23_features.c)
add_executable(m01_s02_c_abi_export m01_s02_c_abi_export.c m01_s02_c_abi_export.h)
add_executable(m01_s03_explicit_memory m01_s03_explicit_memory.c)
add_executable(m01_EX_packet_filter ex01_c23_packet_filter.c)

# ==============================================================================
# SEKCE 02: Datove typy na ruznych architekturach
# ==============================================================================
add_executable(m02_s01_arch_types_demo m02_s01_arch_types_demo.c)
add_executable(m02_EX_arch_telemetry ex02_arch_types_telemetry.c)

# ==============================================================================
# SEKCE 03: Prace s bity a binarnimi daty
# ==============================================================================
add_executable(m03_s01_bitfields m03_s01_bitfields.c)
add_executable(m03_s02_endianness m03_s02_endianness.c)
add_executable(m03_s03_serialization m03_s03_serialization.c)
add_executable(m03_EX_can_serializer ex03_binary_can_serializer.c)

# ==============================================================================
# SEKCE 04: Zpracovavani chyb
# ==============================================================================
add_executable(m04_s01_error_handling_nodiscard m04_s01_error_handling_nodiscard.c)
add_executable(m04_s01_error_unwinding_goto m04_s01_error_unwinding_goto.c)
add_executable(m04_EX_spi_flash_goto ex04_error_goto_spi_flash.c)

# ==============================================================================
# SEKCE 05: Ukazatele do pameti a zivotnost objektu
# ==============================================================================
add_executable(m05_s01_lifetime_dangling m05_s01_lifetime_dangling.c)
add_executable(m05_s02_ownership_borrowing m05_s02_ownership_borrowing.c)
add_executable(m05_s03_pointer_provenance m05_s03_pointer_provenance.c)
add_executable(m05_EX_dma_lifetime ex05_pointer_lifetime_dma.c)

# ==============================================================================
# SEKCE 06: Pravidla pro aliasing
# ==============================================================================
add_executable(m06_s01_strict_aliasing_punning m06_s01_strict_aliasing_punning.c)
add_executable(m06_s02_restrict_optimization m06_s02_restrict_optimization.c)
add_executable(m06_EX_aliasing_dsp ex06_aliasing_dsp_filter.c)

# ==============================================================================
# SEKCE 07: Alokace pameti
# ==============================================================================
add_executable(m07_s01_memory_pool m07_s01_memory_pool.c)
add_executable(m07_s02_arena_allocator m07_s02_arena_allocator.c)
add_executable(m07_s03_auto_cleanup m07_s03_auto_cleanup.c)
add_executable(m07_EX_memory_pool ex07_static_memory_pool.c)

# ==============================================================================
# SEKCE 08: Hromadna alokace pameti
# ==============================================================================
add_executable(m08_s01_array_decay m08_s01_array_decay.c)
add_executable(m08_s02_struct_padding m08_s02_struct_padding.c)
add_executable(m08_s03_memory_copy_realloc m08_s03_memory_copy_realloc.c)
add_executable(m08_EX_padding_window ex08_padding_and_sliding_window.c)

# ==============================================================================
# SEKCE 09: Dynamicke datove struktury pro embedded
# ==============================================================================
add_executable(m09_s01_bounded_array_demo m09_s01_bounded_array_demo.c)
add_executable(m09_s02_packet_ring_buffer_demo m09_s02_packet_ring_buffer_demo.c)
add_executable(m09_s03_index_linked_list_demo m09_s03_index_linked_list_demo.c)
add_executable(m09_EX_index_list_pop ex09_index_list_remove.c)

# ==============================================================================
# SEKCE 10: Specifika embedded programovani (volatile a MMIO)
# ==============================================================================
add_executable(m10_s01_volatile_mmio_demo m10_s01_volatile_mmio_demo.c)
add_executable(m10_EX_uart_mmio ex10_volatile_uart_mmio.c)

# ==============================================================================
# SEKCE 11: Preruseni (ISR) a sdilena data
# ==============================================================================
add_executable(m11_s02_isr_critical_section_demo m11_s02_isr_critical_section_demo.c)
add_executable(m11_s03_isr_atomics_c23_demo m11_s03_isr_atomics_c23_demo.c)
add_executable(m11_s01_spsc_ring_buffer_demo m11_s01_spsc_ring_buffer_demo.c)
add_executable(m11_EX_adc_spsc ex11_isr_adc_spsc.c)

# ==============================================================================
# SEKCE 12: Ulohy a synchronizace v RTOS
# ==============================================================================
add_executable(m12_s01_rtos_sync_demo m12_s01_rtos_sync_demo.c)
add_executable(m12_s02_rtos_static_task_demo m12_s02_rtos_static_task_demo.c)
add_executable(m12_s03_rtos_deadlock_hierarchy_demo m12_s03_rtos_deadlock_hierarchy_demo.c)
add_executable(m12_s04_rtos_event_groups_demo m12_s04_rtos_event_groups_demo.c)
add_executable(m12_EX_rtos_hierarchy ex12_rtos_lock_hierarchy.c)

# ==============================================================================
# SEKCE 13: Cache, DMA a zarovnani dat
# ==============================================================================
add_executable(m13_s01_cmsis_cache_maintenance_demo m13_s01_cmsis_cache_maintenance_demo.c)
add_executable(m13_s02_cache_alignas_demo m13_s02_cache_alignas_demo.c)
add_executable(m13_s03_mpu_noncacheable_section_demo m13_s03_mpu_noncacheable_section_demo.c)
add_executable(m13_EX_dma_cache_safety ex13_dma_cache_safety.c)

# ==============================================================================
# SEKCE 14: Struktury a abstrakce
# ==============================================================================
add_executable(m14_s01_context_pointer m14_s01_context_pointer.c)
add_executable(m14_s02_data_invariants m14_s02_data_invariants.c)
add_executable(m14_s03_network_api m14_s03_network_api.c m14_s03_opaque_main.c)
add_executable(m14_EX_pid_invariants ex14_pid_context_invariants.c)

# ==============================================================================
# SEKCE 15: Modularita a navrh testovatelneho firmware
# ==============================================================================
add_executable(m15_s01_modular_main m15_s01_math_utils.c m15_s01_modular_main.c)
add_executable(m15_s02_header_only_main m15_s02_header_only_main.c)
add_executable(m15_s03_hal_interface_di_demo m15_s03_hal_interface_di_demo.c)
add_executable(m15_EX_modular_hal_di ex15_modular_hal_di.c)

# ==============================================================================
# SEKCE 16: Psani testovatelneho kodu a mockovani
# ==============================================================================
add_executable(m16_s01_linker_wrap_mock_demo m16_s01_linker_wrap_mock_demo.c)
target_link_options(m16_s01_linker_wrap_mock_demo PRIVATE -Wl,--wrap=hardware_i2c_write)
add_executable(m16_s02_unity_mock_demo m16_s02_unity_mock_demo.c)
add_executable(m16_EX_unit_test_mock ex16_unit_test_i2c_mock.c)

# ==============================================================================
# SEKCE 17: Regresni testovani a ladeni chyb
# ==============================================================================
add_executable(m17_s01_regression_test_demo m17_s01_regression_test_demo.c)
add_executable(m17_s02_qemu_semihosting_demo m17_s02_qemu_semihosting_demo.c)
add_executable(m17_s03_hardfault_decoder_demo m17_s03_hardfault_decoder_demo.c)
add_executable(m17_EX_regression_fix ex17_regression_underflow_fix.c)

# ==============================================================================
# SEKCE 18: Tvorba korektnich programu a diagnostika
# ==============================================================================
add_executable(m18_s01_undefined_behavior m18_s01_undefined_behavior.c)
add_executable(m18_s02_ub_optimization_and_misra_demo m18_s02_ub_optimization_and_misra_demo.c)
add_executable(m18_EX_misra_ub_safe ex18_misra_ub_refactoring.c)

# ==============================================================================
# SEKCE 19: Polymorfismus v jazyce C
# ==============================================================================
add_executable(m19_s01_simulated_inheritance m19_s01_simulated_inheritance.c)
add_executable(m19_s02_container_of_macro m19_s02_container_of_macro.c)
add_executable(m19_s03_intrusive_list m19_s03_intrusive_list.c)
add_executable(m19_EX_intrusive_timers ex19_intrusive_timer_list.c)

# ==============================================================================
# SEKCE 20: Vyctovy polymorfismus
# ==============================================================================
add_executable(m20_s01_enum_type_tag m20_s01_enum_type_tag.c)
add_executable(m20_s02_tagged_union m20_s02_tagged_union.c)
add_executable(m20_EX_tagged_commands ex20_tagged_union_commands.c)

# ==============================================================================
# SEKCE 21: Rekurzivni volani
# ==============================================================================
add_executable(m21_s01_recursion_types m21_s01_recursion_types.c)
add_executable(m21_s02_recursion_base_case m21_s02_recursion_base_case.c)
add_executable(m21_s03_tail_call_optimization m21_s03_tail_call_optimization.c)
add_executable(m21_EX_tail_power ex21_tail_recursion_power.c)

# ==============================================================================
# SEKCE 22: Moznosti generickeho programovani
# ==============================================================================
add_executable(m22_s01_generic_keyword m22_s01_generic_keyword.c)
add_executable(m22_s02_macro_templates m22_s02_macro_templates.c)
add_executable(m22_s03_type_erasure_alignment m22_s03_type_erasure_alignment.c)
add_executable(m22_EX_generic_clamp ex22_generic_clamp_macro.c)

# ==============================================================================
# SEKCE 23: Objektove orientovane programovani
# ==============================================================================
add_executable(m23_s01_type_tags m23_s01_type_tags.c)
add_executable(m23_s02_vmt_dispatch m23_s02_vmt_dispatch.c)
add_executable(m23_s03_inheritance_override m23_s03_inheritance_override.c)
add_executable(m23_EX_vmt_channel ex23_oop_vmt_channel.c)

# ==============================================================================
# SEKCE 24: Bonus - Externi knihovny
# ==============================================================================
include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)

# 1. SDL3
FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.0
)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

add_executable(m24_s01_sdl3_demo m24_s01_sdl3_demo.c)
target_link_libraries(m24_s01_sdl3_demo PRIVATE SDL3::SDL3-static)

# 2. ZLIB a libpng (s vyuzitim OVERRIDE_FIND_PACKAGE pro CMake 3.24+)
#FetchContent_Declare(ZLIB
#        GIT_REPOSITORY https://github.com/madler/zlib.git
#        GIT_TAG v1.3.1
#        OVERRIDE_FIND_PACKAGE
#)
#FetchContent_MakeAvailable(ZLIB)
#
#FetchContent_Declare(libpng
#        GIT_REPOSITORY https://github.com/glennrp/libpng.git
#        GIT_TAG v1.6.43
#)
#set(PNG_TESTS OFF CACHE BOOL "" FORCE)
#set(PNG_SHARED OFF CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(libpng)
#
#add_executable(m24_s02_libpng_demo m24_s02_libpng_demo.c)
#target_link_libraries(m24_s02_libpng_demo PRIVATE png_static)

# 3. libcurl
FetchContent_Declare(curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_7_1
)
set(CURL_DISABLE_TESTS ON CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(curl)

add_executable(m24_s03_libcurl_demo m24_s03_libcurl_demo.c)
target_link_libraries(m24_s03_libcurl_demo PRIVATE libcurl)

# 4. SQLite
FetchContent_Declare(sqlite3_src
        URL https://sqlite.org/2024/sqlite-amalgamation-3450300.zip
)
FetchContent_MakeAvailable(sqlite3_src)
add_library(sqlite3_lib STATIC ${sqlite3_src_SOURCE_DIR}/sqlite3.c)
target_include_directories(sqlite3_lib PUBLIC ${sqlite3_src_SOURCE_DIR})

add_executable(m24_s04_sqlite_demo m24_s04_sqlite_demo.c)
target_link_libraries(m24_s04_sqlite_demo PRIVATE sqlite3_lib)

add_executable(m24_EX_sqlite_logger ex24_sqlite_telemetry_logger.c)
target_link_libraries(m24_EX_sqlite_logger PRIVATE sqlite3_lib)

# ==============================================================================
# SEKCE 25: Finalni souhrnny projekt (Capstone - 120 min)
# ==============================================================================
add_executable(m25_FINAL_CAPSTONE_SKELETON ex25_final_capstone_skeleton.c)
add_executable(m25_FINAL_CAPSTONE_SOLUTION ex25_final_capstone_project.c)
