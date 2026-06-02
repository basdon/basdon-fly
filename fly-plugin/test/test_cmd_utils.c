#include "../a_samp.h"
#include "../playerdata.h"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "../game_sa_data.c"
#pragma GCC diagnostic warning "-Wunused-function"
#pragma GCC diagnostic warning "-Wunused-variable"
#include "../cmd_utils.c"

struct playerdata *pdata[MAX_PLAYERS];

void reset_cmdctx(struct COMMANDCONTEXT *cmdctx, char *value)
{
	cmdctx->playerid = 1;
	cmdctx->cmdtext = value;
	cmdctx->parseidx = 0;
}
/*jeanine:p:i:2;p:1;a:b;x:-107.00;y:2.00;*/
void test_cmd_get_int_param()
{
	struct COMMANDCONTEXT cmdctx;
	int result, value;

	_test_suite = "cmd_get_int_param";
	_test_name = "empty input";
	{
		reset_cmdctx(&cmdctx, "");
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(0, result);
	}

	_test_name = "simple";
	{
		reset_cmdctx(&cmdctx, "123 something");
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(123, value);
		_assert_equals(3, cmdctx.parseidx);
	}

	_test_name = "not a number";
	{
		reset_cmdctx(&cmdctx, "something 123");
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(0, result);
	}

	_test_name = "parseidx set";
	{
		reset_cmdctx(&cmdctx, "something 123");
		cmdctx.parseidx = 10;
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(123, value);
	}

	_test_name = "starts with whitespace";
	{
		reset_cmdctx(&cmdctx, "  123");
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(123, value);
	}

	_test_name = "all of the above but negative number";
	{
		reset_cmdctx(&cmdctx, "yeah   -123");
		cmdctx.parseidx = 5;
		result = cmd_get_int_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(-123, value);
	}
}
/*jeanine:p:i:3;p:1;a:b;x:-29.00;y:2.00;*/
void test_cmd_get_player_param()
{
	struct COMMANDCONTEXT cmdctx;
	int result, value;

	_test_suite = "cmd_get_player_param";
	_test_name = "empty input";
	{
		reset_cmdctx(&cmdctx, "");
		result = cmd_get_player_param(&cmdctx, &value);
		_assert_equals(0, result);
	}

	_test_name = "nobody online";
	{
		reset_cmdctx(&cmdctx, "2");
		result = cmd_get_player_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(INVALID_PLAYER_ID, value);
	}

	_test_name = "by playerid";
	{
		pdata[22] = malloc(sizeof(struct playerdata));
		reset_cmdctx(&cmdctx, "22");
		result = cmd_get_player_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(22, value);
		pdata[22] = NULL;
	}

	_test_name = "by name substring, should be case-insensitive (use normname field)";
	{
		pdata[2] = malloc(sizeof(struct playerdata));
		strcpy(pdata[2]->name, "AbC");
		strcpy(pdata[2]->normname, "abc");
		reset_cmdctx(&cmdctx, "bc yeah");
		result = cmd_get_player_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(2, value);
		_assert_equals(2, cmdctx.parseidx);
		pdata[2] = NULL;
	}

	_test_name = "by name substring if name only consist of numbers";
	{
		pdata[2] = malloc(sizeof(struct playerdata));
		strcpy(pdata[2]->normname, "23");
		reset_cmdctx(&cmdctx, "23");
		result = cmd_get_player_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(2, value);
		pdata[2] = NULL;
	}
}
/*jeanine:p:i:4;p:1;a:b;x:49.00;y:2.00;*/
void test_cmd_get_str_param()
{
	struct COMMANDCONTEXT cmdctx;
	char value[144];
	int result;

	_test_suite = "cmd_get_str_param";
	_test_name = "empty input";
	{
		reset_cmdctx(&cmdctx, "");
		result = cmd_get_str_param(&cmdctx, value);
		_assert_equals(0, result);
	}

	_test_name = "simple";
	{
		reset_cmdctx(&cmdctx, "bla123 second");
		result = cmd_get_str_param(&cmdctx, value);
		_assert_equals(1, result);
		_assert_strcmp("bla123", value);
		_assert_equals(6, cmdctx.parseidx);
	}

	_test_name = "parseidx set";
	{
		reset_cmdctx(&cmdctx, "one two three");
		cmdctx.parseidx = 3;
		result = cmd_get_str_param(&cmdctx, value);
		_assert_equals(1, result);
		_assert_strcmp("two", value);
	}

	_test_name = "starts with whitespace";
	{
		reset_cmdctx(&cmdctx, "  123");
		result = cmd_get_str_param(&cmdctx, value);
		_assert_equals(1, result);
		_assert_strcmp("123", value);
	}
}
/*jeanine:p:i:5;p:1;a:b;x:128.00;y:2.00;*/
void test_cmd_get_str_param_n()
{
	struct COMMANDCONTEXT cmdctx;
	int result, out_len, out_actual_len;
	char value[144];

	_test_suite = "cmd_get_str_param_n";
	_test_name = "empty input";
	{
		reset_cmdctx(&cmdctx, "");
		result = cmd_get_str_param_n(&cmdctx, value, 5, &out_len, &out_actual_len);
		_assert_equals(0, result);
	}

	_test_name = "shorter (with parseidx and leading whitespace)";
	{
		reset_cmdctx(&cmdctx, "aaa  one two");
		cmdctx.parseidx = 3;
		result = cmd_get_str_param_n(&cmdctx, value, 5, &out_len, &out_actual_len);
		_assert_equals(1, result);
		_assert_strcmp("one", value);
		_assert_equals(3, out_len);
		_assert_equals(3, out_actual_len);
	}

	_test_name = "output limited";
	{
		reset_cmdctx(&cmdctx, "clockwork");
		result = cmd_get_str_param_n(&cmdctx, value, 5, &out_len, &out_actual_len);
		_assert_equals(1, result);
		_assert_equals(9, cmdctx.parseidx);
		_assert_strcmp("clock", value);
		_assert_equals(5, out_len);
		_assert_equals(9, out_actual_len);
	}

	_test_name = "NULL passed as actual len parameter";
	{
		reset_cmdctx(&cmdctx, "clockwork");
		result = cmd_get_str_param_n(&cmdctx, value, 5, &out_len, NULL);
		_assert_equals(1, result);
		_assert_equals(9, cmdctx.parseidx);
		_assert_strcmp("clock", value);
		_assert_equals(5, out_len);
	}
}
/*jeanine:p:i:6;p:1;a:b;x:215.00;y:2.00;*/
void test_cmd_get_vehiclemodel_param()
{
	struct COMMANDCONTEXT cmdctx;
	int result, value;

	_test_suite = "cmd_get_vehiclemodel_param";
	_test_name = "empty input";
	{
		reset_cmdctx(&cmdctx, "");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(0, result);
	}

	_test_name = "exact modelname with parseidx and leading whitespace";
	{
		reset_cmdctx(&cmdctx, "one  regina yeah");
		cmdctx.parseidx = 3;
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_REGINA, value);
		_assert_equals(11, cmdctx.parseidx);
	}

	_test_name = "by model id";
	{
		reset_cmdctx(&cmdctx, " 411");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_INFERNUS, value);
	}

	_test_name = "partial match";
	{
		reset_cmdctx(&cmdctx, "qui");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_MARQUIS, value);
	}

	/*hy would result in "dinghy" initially, but we want to prioritize
	  matches where the needle occurs earlier, so this should result
	  in "hydra" instead*/
	_test_name = "partial match (with prioritization)";
	{
		reset_cmdctx(&cmdctx, "hy");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_HYDRA, value);
	}

	_test_name = "stop at space";
	{
		reset_cmdctx(&cmdctx, "monster b");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_MONSTER, value);
	}

	_test_name = "replace underscores with space";
	{
		reset_cmdctx(&cmdctx, "monster_b");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_MONSTERB, value);
	}

	/*this was a fun bug: in vehicle data, "admiral" comes after
	  "monster". Due to unconditional lowercasing of the names
	  being matched against, "Monster\0Admiral" became "monster admiral",
	  resulting in being a match for "monster a" and thus getting
	  the normal Monster instead of Monster A.*/
	_test_name = "underscore space replacement bug";
	{
		reset_cmdctx(&cmdctx, "monster_a");
		result = cmd_get_vehiclemodel_param(&cmdctx, &value);
		_assert_equals(1, result);
		_assert_equals(MODEL_MONSTERA, value);
	}
}
/*jeanine:p:i:1;p:0;a:b;y:1.00;*/
void test_cmd_utils()
{
	test_cmd_get_int_param();
	test_cmd_get_player_param();
	test_cmd_get_str_param();
	test_cmd_get_str_param_n();
	test_cmd_get_vehiclemodel_param();
}
