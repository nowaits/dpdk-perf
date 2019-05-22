#include <vnet/plugin/plugin.h>
#include <vnet/api_errno.h>

#include <vnet/vnet.h>

static clib_error_t *
command_fn(vlib_main_t *vm,
    unformat_input_t *input, vlib_cli_command_t *cmd)
{
    vlib_cli_output(vm,  "OK");

    return 0;
}

VLIB_CLI_COMMAND(set_pm_command, static) = {
    .path = "show process",
    .short_help = "show process test",
    .function = command_fn,
};


static clib_error_t *
process_init(vlib_main_t *vm)
{
    return 0;
}

VLIB_INIT_FUNCTION(process_init);

VLIB_PLUGIN_REGISTER() = {
    .version = "1.0.0",
};
