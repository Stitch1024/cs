cmake_minimum_required(VERSION 3.10)

project(uart_communication LANGUAGES C)

# Common source files
set(UART_COMMON_SOURCES
    uart_tx.c
    uart_rx.c
    uart_bus.c
)

# Linked demo (sender + receiver in one program)
add_executable(uart_linked
    main.c
    ${UART_COMMON_SOURCES}
)

# Standalone sender
add_executable(uart_sender
    uart_sender.c
    ${UART_COMMON_SOURCES}
)

# Standalone receiver
add_executable(uart_receiver
    uart_receiver.c
    ${UART_COMMON_SOURCES}
)