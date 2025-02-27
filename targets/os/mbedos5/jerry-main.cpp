/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"

#include "jerry-core/include/jerryscript.h"
#include "jerry-core/include/jerryscript-port.h"
#include "jerryscript-ext/handler.h"

/**
 * Standalone Jerry exit codes
 */
#define JERRY_STANDALONE_EXIT_CODE_OK   (0)
#define JERRY_STANDALONE_EXIT_CODE_FAIL (1)

/**
 * Register a JavaScript function in the global object.
 */
static void
register_js_function (const char *name_p, /**< name of the function */
                      jerry_external_handler_t handler_p) /**< function callback */
{
  jerry_value_t result_val = jerryx_handler_register_global (name_p, handler_p);

  if (jerry_value_is_exception (result_val))
  {
    jerry_port_log (JERRY_LOG_LEVEL_WARNING, "[Warning] Failed to register '%s' method.", name_p);
  }

  jerry_value_free (result_val);
} /* register_js_function */

int main()
{
  Serial device(USBTX, USBRX);  // tx, rx
  device.baud(115200);

  /* Initialize engine */
  jerry_init (JERRY_INIT_EMPTY);

  jerry_value_t ret_value = jerry_undefined ();
  const jerry_char_t script[] = "print ('Hello, World!');";
  jerry_port_log (JERRY_LOG_LEVEL_DEBUG, "This test run the following script code: [%s]\n\n", script);

  /* Register the print function in the global object. */
  register_js_function ("print", jerryx_handler_print);

  /* Setup Global scope code */
  ret_value = jerry_parse (script, sizeof (script) - 1, NULL);

  if (!jerry_value_is_exception (ret_value))
  {
    /* Execute the parsed source code in the Global scope */
    ret_value = jerry_run (ret_value);
  }

  int ret_code = JERRY_STANDALONE_EXIT_CODE_OK;

  if (jerry_value_is_exception (ret_value))
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "[Error] Script Error!");

    ret_code = JERRY_STANDALONE_EXIT_CODE_FAIL;
  }

  jerry_value_free (ret_value);

  /* Cleanup engine */
  jerry_cleanup ();

  return ret_code;
}
