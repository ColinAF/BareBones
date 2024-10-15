typedef struct terminal terminal_t;  /**< Opaque struct for terminal state */

/**
 * @brief Initializes the terminal by clearing the screen and setting up default values.
 */
void 
terminal_initialize(terminal_t* term);

/**
 * @brief Clears the terminal screen by filling it with blank spaces.
 */
void 
terminal_clear(terminal_t* term);

/**
 * @brief Writes a null-terminated string to the terminal.
 * @param data The string to write.
 */
void 
terminal_write_string(terminal_t* term, const char* data);
