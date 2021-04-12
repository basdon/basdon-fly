#include "_test_api.c"

#include "_test_mock_samp_h.c"

#define SAMP_C_SKIP_MIXED_NATIVES
#include "../samp.c"

static
void _get_SendClientMessage_msg(struct BUFFERED_RPC *rpcbuf, struct RPCDATA_SendClientMessage *rpcdata)
{
	_assert(rpcbuf->RPC == RPC_SendClientMessage);
	*rpcdata = *((struct RPCDATA_SendClientMessage*) rpcbuf->data);
}

static
void test_SendClientMessage()
{
	struct RPCDATA_SendClientMessage rpcdata;
	struct BUFFERED_RPC rpcbuf;

	_test_name = "shorter than 144";
	{
		SendClientMessage(0, -1, "---------------------------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(72, rpcdata.message_length);
		_assert_strcmpex("---------------------------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "exactly 144";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "---------------------------------------------------------------------end");
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "---------------------------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "145";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "----------------------------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "----------------------------------------------------------------------en", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(1, rpcdata.message_length);
		_assert_strcmpex("d", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "512";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
					 "-----------------------------------------------------------------------a"
					 "------------------------------------------------------------------------"
					 "-----------------------------------------------------------------------b"
					 "------------------------------------------------------------------------"
					 "-----------------------------------------------------------------------c"
					 "-----------------------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------b", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------c", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(68, rpcdata.message_length);
		_assert_strcmpex("-----------------------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "600 this should cut off";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------a"
		                         "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------b"
		                         "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------c"
		                         "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------d"
		                         "---------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------b", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------c", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------d", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "450 one color change";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------a"
		                         "------------------------------------------------------------------------"
		                         "------------------------------------------{00ff00}---------------------b"
		                         "------------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------c"
		                         "---------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "------------------------------------------{00ff00}---------------------b", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(0x00FF00FF, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------c", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(18, rpcdata.message_length);
		_assert_equals(0x00FF00FF, rpcdata.color);
		_assert_strcmpex("---------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "450 three color changes";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "-------------------------------{ff0000}--------------------------------a"
		                         "------------------------------------------------------------------------"
		                         "------------------------------------------{00ff00}---------------------b"
		                         "------------------------------------------------------------------------"
		                         "------------{0000ff}---------------------------------------------------c"
		                         "---------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "-------------------------------{ff0000}--------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(0xFF0000FF, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "------------------------------------------{00ff00}---------------------b", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(0x00FF00FF, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "------------{0000ff}---------------------------------------------------c", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(18, rpcdata.message_length);
		_assert_equals(0x0000FFFF, rpcdata.color);
		_assert_strcmpex("---------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 skip bogus color code";
	{
		SendClientMessage(0, -1, "--------------------------------{ff0000}------{cdefgh}---{zzzzzz}-------"
		                         "--{abc}--{00000ff}-----------------------------------------------------a"
		                         "-----------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("--------------------------------{ff0000}------{cdefgh}---{zzzzzz}-------"
		                 "--{abc}--{00000ff}-----------------------------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(56, rpcdata.message_length);
		_assert_equals(0xFF0000FF, rpcdata.color);
		_assert_strcmpex("-----------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 color brace out of bounds check (underflow)";
	{
		SendClientMessage(0, -1, "fff}--------------------------------------------------------------------"
		                         "-----------------------------------------------------------------------a"
		                         "-----------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("fff}--------------------------------------------------------------------"
		                 "-----------------------------------------------------------------------a", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(56, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("-----------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 color change on boundary no space";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "--------------------------------------------------------------------{ff0"
		                         "000}-------------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(140, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "--------------------------------------------------------------------", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(60, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("{ff0000}-------------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 don't split word space after";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "------------------------------------------------------------------ quick"
		                         " brown ----------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(144, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "------------------------------------------------------------------ quick", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(55, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("brown ----------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 split word space before";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "----------------------------------------------------------------- quick "
		                         "brown -----------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(143, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "----------------------------------------------------------------- quick", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(56, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("brown -----------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}

	_test_name = "200 split word middle";
	{
		SendClientMessage(0, -1, "------------------------------------------------------------------------"
		                         "-------------------------------------------------------------------- qui"
		                         "ck brown --------------------------------------------end");

		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(140, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("------------------------------------------------------------------------"
		                 "--------------------------------------------------------------------", rpcdata.message);
		_assert(_sampc_get_next_buffered_rpc(&rpcbuf));
		_get_SendClientMessage_msg(&rpcbuf, &rpcdata);
		_assert_equals(59, rpcdata.message_length);
		_assert_equals(-1, rpcdata.color);
		_assert_strcmpex("quick brown --------------------------------------------end", rpcdata.message);
		_assert(!_sampc_get_next_buffered_rpc(&rpcbuf));
	}
}

void test()
{
	test_SendClientMessage();
}
