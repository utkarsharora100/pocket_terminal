#include "../ui/vars.h"
#include "../ui/actions.h"
#include "pty_hal.h"
#include <string.h>
#include <stdio.h>
#include "../ui/screens.h"
#include "../ui/styles.h"
/* ── input_terminal — user input string ── */
static char input_terminal[1024] = {0};

const char *get_var_input_terminal(void) { return input_terminal; }
void set_var_input_terminal(const char *value) {
    strncpy(input_terminal, value, sizeof(input_terminal) - 1);
    input_terminal[sizeof(input_terminal) - 1] = '\0';
}

/* ── output_terminal — PTY output string ── */
static char output_terminal[4096] = {0};

const char *get_var_output_terminal(void) { return output_terminal; }
void set_var_output_terminal(const char *value) {
    strncpy(output_terminal, value, sizeof(output_terminal) - 1);
    output_terminal[sizeof(output_terminal) - 1] = '\0';
}

/* ── PTY output callback — called from pty_hal read thread ── */
void app_ui_on_pty_output(const char *chunk)
{
    size_t cur = strlen(output_terminal);
    size_t add = strlen(chunk);

    if (cur + add >= sizeof(output_terminal)) {
        size_t trim = sizeof(output_terminal) / 4;
        memmove(output_terminal, output_terminal + trim, cur - trim + 1);
        cur -= trim;
    }

    strncat(output_terminal, chunk, sizeof(output_terminal) - cur - 1);
    set_var_output_terminal(output_terminal);
}

/* ── send_command action ── */
void action_send_command(lv_event_t *e)
{
    char out[1026];
    snprintf(out, sizeof(out), "%s\n", get_var_input_terminal());
    pty_write(out);
    set_var_input_terminal("");
}
static char send_terminal[1024] = {0};

const char *get_var_send_terminal(void) { return send_terminal; }
void set_var_send_terminal(const char *value) {
    strncpy(send_terminal, value, sizeof(send_terminal) - 1);
    send_terminal[sizeof(send_terminal) - 1] = '\0';
}

void eez_impl_post_init(void)
{
    lv_obj_set_style_text_font(objects.terminal_area,
                               &lv_font_unscii_8,
                               LV_PART_MAIN);
}
