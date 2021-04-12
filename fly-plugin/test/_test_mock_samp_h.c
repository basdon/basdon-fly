/*SAMP_SendRPCToPlayer is defined in samp.h*/

#undef SAMP_SendRPCToPlayer

#define MAX_BUFFERED_RPCS 20

struct BUFFERED_RPC {
	int RPC;
	void *data;
	short playerid;
	short numberOfBitsUsed;
};
struct BUFFERED_RPC _buffered_rpcs[MAX_BUFFERED_RPCS];
int _num_buffered_rpcs;

static
void SAMP_SendRPCToPlayer(int RPC, struct BitStream *bs, int playerid, int unk)
{
	struct BUFFERED_RPC *rpcbuf;
	int byteSize;

	if (_num_buffered_rpcs == MAX_BUFFERED_RPCS) {
		printf("increase MAX_BUFFERED_RPCS\n");
		exit(1);
	}
	byteSize = (bs->numberOfBitsUsed + (8 - bs->numberOfBitsUsed) % 8) / 8;
	rpcbuf = &_buffered_rpcs[_num_buffered_rpcs++];
	rpcbuf->RPC = RPC;
	rpcbuf->playerid = playerid;
	rpcbuf->data = malloc(byteSize);
	rpcbuf->numberOfBitsUsed = bs->numberOfBitsUsed;
	memcpy(rpcbuf->data, bs->ptrData, byteSize);
}

static
void *_sampc_clear_buffered_rpcs()
{
	_num_buffered_rpcs = 0;
}

static
int _sampc_get_next_buffered_rpc(struct BUFFERED_RPC *rpcbuf)
{
	int i;

	if (!_num_buffered_rpcs) {
		return 0;
	}
	_num_buffered_rpcs--;
	*rpcbuf = _buffered_rpcs[0];
	for (i = 0; i < _num_buffered_rpcs; i++) {
		_buffered_rpcs[i] = _buffered_rpcs[i + 1];
	}
	return 1;
}
