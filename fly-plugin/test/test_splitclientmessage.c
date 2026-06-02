#include "testapi.c"

struct RPCDATA_SendClientMessage {
	int color;
	int message_length;
	char message[144];
};

#include "../util_splitclientmessage.c"

int main(int argc, char **argv)
{
	struct RPCDATA_SendClientMessage rpcdata[4];
	int res;

	_test_suite = "splitclientmessage";
	_test_name = "shorter than 144";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"---------------------------------------------------------------------end"
		);

		_assert_equals(1, res);
		_assert_equals(-1, rpcdata[0].color);
		_assert_equals(72, rpcdata[0].message_length);
		_assert_strcmpex(
			"---------------------------------------------------------------------end",
			rpcdata[0].message
		);
	}

	_test_name = "exactly 144";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"---------------------------------------------------------------------end"
		);

		_assert_equals(1, res);
		_assert_equals(-1, rpcdata[0].color);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"---------------------------------------------------------------------end",
			rpcdata[0].message
		);
	}

	_test_name = "145";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"----------------------------------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(-1, rpcdata[0].color);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"----------------------------------------------------------------------en",
			rpcdata[0].message
		);
		_assert_equals(-1, rpcdata[1].color);
		_assert_equals(1, rpcdata[1].message_length);
		_assert_strcmpex(
			"d",
			rpcdata[1].message
		);
	}

	_test_name = "512";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------b"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c"
			"-----------------------------------------------------------------end"
		);

		_assert_equals(4, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(144, rpcdata[1].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------b",
			rpcdata[1].message
		);
		_assert_equals(144, rpcdata[2].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c",
			rpcdata[2].message
		);
		_assert_equals(68, rpcdata[3].message_length);
		_assert_strcmpex(
			"-----------------------------------------------------------------end",
			rpcdata[3].message
		);
	}

	_test_name = "600 this should cut off";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------b"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------d"
			"---------------------end"
		);

		_assert_equals(4, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(144, rpcdata[1].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------b",
			rpcdata[1].message
		);
		_assert_equals(144, rpcdata[2].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c",
			rpcdata[2].message
		);
		_assert_equals(144, rpcdata[3].message_length);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------d",
			rpcdata[3].message
		);
	}

	_test_name = "450 one color change";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a"
			"------------------------------------------------------------------------"
			"------------------------------------------{00ff00}---------------------b"
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c"
			"---------------end"
		);

		_assert_equals(4, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(144, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"------------------------------------------{00ff00}---------------------b",
			rpcdata[1].message
		);
		_assert_equals(144, rpcdata[2].message_length);
		_assert_equals(0x00FF00FF, rpcdata[2].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-----------------------------------------------------------------------c",
			rpcdata[2].message
		);
		_assert_equals(18, rpcdata[3].message_length);
		_assert_equals(0x00FF00FF, rpcdata[3].color);
		_assert_strcmpex(
			"---------------end",
			rpcdata[3].message
		);
	}

	_test_name = "450 three color changes";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"-------------------------------{ff0000}--------------------------------a"
			"------------------------------------------------------------------------"
			"------------------------------------------{00ff00}---------------------b"
			"------------------------------------------------------------------------"
			"------------{0000ff}---------------------------------------------------c"
			"---------------end"
		);

		_assert_equals(4, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"-------------------------------{ff0000}--------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(144, rpcdata[1].message_length);
		_assert_equals(0xFF0000FF, rpcdata[1].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"------------------------------------------{00ff00}---------------------b",
			rpcdata[1].message
		);
		_assert_equals(144, rpcdata[2].message_length);
		_assert_equals(0x00FF00FF, rpcdata[2].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"------------{0000ff}---------------------------------------------------c",
			rpcdata[2].message
		);
		_assert_equals(18, rpcdata[3].message_length);
		_assert_equals(0x0000FFFF, rpcdata[3].color);
		_assert_strcmpex(
			"---------------end",
			rpcdata[3].message
		);
	}

	_test_name = "200 skip bogus color code";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"--------------------------------{ff0000}------{cdefgh}---{zzzzzz}-------"
			"--{abc}--{00000ff}-----------------------------------------------------a"
			"-----------------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"--------------------------------{ff0000}------{cdefgh}---{zzzzzz}-------"
			"--{abc}--{00000ff}-----------------------------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(56, rpcdata[1].message_length);
		_assert_equals(0xFF0000FF, rpcdata[1].color);
		_assert_strcmpex(
			"-----------------------------------------------------end",
			rpcdata[1].message
		);
	}

	_test_name = "200 color brace out of bounds check (underflow)";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"fff}--------------------------------------------------------------------"
			"-----------------------------------------------------------------------a"
			"-----------------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"fff}--------------------------------------------------------------------"
			"-----------------------------------------------------------------------a",
			rpcdata[0].message
		);
		_assert_equals(56, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"-----------------------------------------------------end",
			rpcdata[1].message
		);
	}

	_test_name = "200 color change on boundary no space";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"--------------------------------------------------------------------{ff0"
			"000}-------------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(140, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"--------------------------------------------------------------------",
			rpcdata[0].message
		);
		_assert_equals(60, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"{ff0000}-------------------------------------------------end",
			rpcdata[1].message
		);
	}

	_test_name = "200 don't split word space after";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"------------------------------------------------------------------ quick"
			" brown ----------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(144, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"------------------------------------------------------------------ quick",
			rpcdata[0].message
		);
		_assert_equals(55, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"brown ----------------------------------------------end",
			rpcdata[1].message
		);
	}

	_test_name = "200 split word space before";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"----------------------------------------------------------------- quick "
			"brown -----------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(143, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"----------------------------------------------------------------- quick",
			rpcdata[0].message
		);
		_assert_equals(56, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"brown -----------------------------------------------end",
			rpcdata[1].message
		);
	}

	_test_name = "200 split word middle";
	{
		res = util_splitclientmessage(
			rpcdata, 4, -1,
			"------------------------------------------------------------------------"
			"-------------------------------------------------------------------- qui"
			"ck brown --------------------------------------------end"
		);

		_assert_equals(2, res);
		_assert_equals(140, rpcdata[0].message_length);
		_assert_equals(-1, rpcdata[0].color);
		_assert_strcmpex(
			"------------------------------------------------------------------------"
			"--------------------------------------------------------------------",
			rpcdata[0].message
		);
		_assert_equals(59, rpcdata[1].message_length);
		_assert_equals(-1, rpcdata[1].color);
		_assert_strcmpex(
			"quick brown --------------------------------------------end",
			rpcdata[1].message
		);
	}
	return 0;
}
