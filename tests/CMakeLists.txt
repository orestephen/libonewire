cmake_minimum_required(VERSION 3.0)

add_executable(test_master_tx test_device.c test_timer.c test_master_tx.c)
target_include_directories(test_master_tx PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(test_master_tx libonewire)
add_test(test_master_tx test_master_tx)

add_executable(test_master_rx test_device.c test_timer.c test_master_rx.c)
target_include_directories(test_master_rx PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(test_master_rx libonewire)
add_test(test_master_rx test_master_rx)
